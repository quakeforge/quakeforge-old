/*
	plugin.c

	Dynamic shared object loader

	Copyright (C) 1999,2000  contributors of the QuakeForge project
	Please see the file "AUTHORS" for a list of contributors

	Author: Zephaniah E. Hull	<mwarp@whitestar.soark.net>
	Date:	11 Feb 2000

	This program is free software; you can redistribute it and/or
	modify it under the terms of the GNU General Public License
	as published by the Free Software Foundation; either version 2
	of the License, or (at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

	See the GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program; if not, write to:

		Free Software Foundation, Inc.
		59 Temple Place - Suite 330
		Boston, MA  02111-1307, USA

	$Id$
*/
#include <string.h>
#include <stdio.h>
#ifndef WIN32
#include <sys/param.h>
#else
#include <input.h>
#endif

#include <stdlib.h>

#include <config.h>
#include <net.h>
#include <plugin.h>
#include <cvar.h>

#ifdef HAVE_DLFCN_H
#include <dlfcn.h>
#endif

#ifndef LIBDIR
#define LIBDIR
#endif

cvar_t	*fs_drvpath;

input_pi *IN;

void Plugin_Init ()
{
	fs_drvpath = Cvar_Get ("fs_drvpath",".:" LIBDIR "/quakeforge",0,"None");
}

#ifdef DJGPP

#elif defined(WIN32)

input_pi Winput;
int IN_Init();
void IN_Move(usercmd_t *);
void IN_Commands();
void Sys_SendKeyEvents();
void IN_Shutdown (void);

int plugin_load(char *filename)
{
	IN = &Winput;
	Winput.description = "Windows Input";
	Winput.Init = IN_Init;
	Winput.Move = IN_Move;
	Winput.Commands = IN_Frame;
	Winput.SendKeyEvents = IN_SendKeyEvents;
	Winput.Shutdown = IN_Shutdown;
	return 1;
}

void plugin_unload(void *handle)
{

}

#else

void *_plugin_load(const char *filename)
{
	void *h;
	char path_buf[MAXPATHLEN*2+1+1];
	char *path,*end;
	int len;

	for (path = fs_drvpath->string; *path; path=end) {
		end = strchr(path,':');
		if (!end)
			end = strchr(path,0);
		len=end-path;
		if (len>MAXPATHLEN)
			len=MAXPATHLEN;
		sprintf(path_buf,"%.*s/%.*s",len,path,MAXPATHLEN,filename);
		if ((h = dlopen(path_buf, RTLD_LAZY))) {
			return h;
		}
		if (*end)
			end++;
	}
	return 0;
}

int plugin_load(char *filename)
{
	void *(*gpi) (void);
	void *h;

	if ((h=_plugin_load(filename))) {
		if ((gpi = dlsym(h, "get_input_plugin_info"))) {
			/*
			input_pi *p;

			p = (input_pi *) gpi();
			p->handle = h;
			p->filename = filename;

			IN = p;
		} else if (gpi = dlsym(h, "get_sound_plugin_info")) {
			  sound_pi *p;

			p = (sound_pi *) gpi();
			p->handle = h;
			p->filename = filename;
			*/
		} else {
			dlclose(h);
			return 0;
		}
	} else {
		fprintf(stderr, "%s\n", dlerror());
		return 0;
	}
	return 1;
}

void plugin_unload(void *handle)
{
	dlclose(handle);
}

#endif
