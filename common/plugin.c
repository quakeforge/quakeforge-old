/*
	plugin.c
	
	Dynamic shared object loader
	
	Copyright (C) 1999, 2000 contributors of the QuakeForge project
	Please see the file "AUTHORS" for a list of contributors

	Author: Zephaniah E. Hull	<mwarp@whitestar.soark.net>
	Date:	11 Feb 2000
	
	This file is part of the QuakeForge Core system.
	
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
	Boston, MA  02111-1307, USA.
*/

#include <string.h>
#include <stdio.h>
#include <dlfcn.h>
#include <stdlib.h>
#include <sys/param.h>

#include <config.h>
#include <net.h>
#include <plugin.h>
#include <cvar.h>

cvar_t drv_path = {"drv_path", ".:" LIBDIR "/quakeforge"};

void *_plugin_load(const char *filename)
{
	void *h;
	char path_buf[MAXPATHLEN*2+1+1];
	char *path,*end;
	int len;

	for (path = drv_path.string; *path; path=end) {
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
