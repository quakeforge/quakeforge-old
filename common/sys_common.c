/*

 sys_common.c - Common system routines.

 Copyright (C) 1996-1997  Id Software, Inc.
 Copyright (C) 1999-2000  contributors of the QuakeForge project
 Please see the file "AUTHORS" for a list of contributors

 This program is free software; you can redistribute it and/or
 modify it under the terms of the GNU General Public License
 as published by the Free Software Foundation; either version 2
 of the License, or (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  

 See the GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program; if not, write to the Free Software
 Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

*/

#include "quakedef.h"

#include <stdio.h>
#include <stdarg.h>
#ifdef HAVE_UNISTD_H
# include <unistd.h>
#endif
#ifdef HAVE_SYS_STAT_H
# include <sys/stat.h>
#endif
#ifdef HAVE_SYS_TIME_H
# include <sys/time.h>
#endif
#ifdef HAVE_SYS_TIMEB_H
# include <sys/timeb.h>
#endif


int nostdout = 0;
cvar_t	sys_nostdout = {"sys_nostdout","0"};


/*
================
Sys_Printf
================
*/
void Sys_Printf (char *fmt, ...)
{
	va_list	argptr;
	char	text[2048];
	unsigned char	*p;

	if (nostdout || sys_nostdout.value) return;

	va_start(argptr, fmt);
	vsnprintf(text, sizeof(text), fmt, argptr);
	va_end(argptr);

	for (p = (unsigned char *)text; *p; p++) {
		if ((*p > 128 || *p < 32) && *p != 10 && *p != 13 && *p != 9)
			printf("[%02x]", *p);
		else
			putc(*p, stdout);
	}

	/* Make sure output is seen. */
	fflush(stdout);
}


/*
================
Sys_mkdir
================
*/
void Sys_mkdir (char *path)
{
#if defined(_WIN32)
	_mkdir(path);
#elif defined(HAVE_MKDIR)
	mkdir(path, 0777);
#else
# warning No mkdir() - creating directories will not be possible
#endif
}


/*
============
Sys_FileTime

returns -1 if not present
============
*/
int Sys_FileTime (char *path)
{
	int	ret = -1;
#ifdef HAVE_STAT
	struct	stat	buf;
	
	if (stat(path, &buf) == 0) ret = buf.st_mtime;
#else /* HAVE_STAT */
	FILE	*f;
#if defined(_WIN32) && !defined(SERVERONLY)
	int t = VID_ForceUnlockedAndReturnState();
#endif
	
	f = fopen(path, "rb");
	if (f) {
		fclose(f);
		ret = 1;
	}
#if defined(_WIN32) && !defined(SERVERONLY)
	VID_ForceLockState(t);
#endif
#endif /* HAVE_STAT */

	return ret;
}


/*
================
Sys_DoubleTime
================
*/
double Sys_DoubleTime(void)
{
	static int starttime = 0;
	long	secs, usecs;

#if defined(HAVE_GETTIMEOFDAY)
	struct timeval	tp;

	gettimeofday(&tp, NULL);
	secs = tp.tv_sec;
	usecs = tp.tv_usec;
#elif defined(HAVE_FTIME)
	struct timeb tstruct;

	ftime(&tstruct);
	secs = tstruct.time;
	usecs = tstruct.millitm*1000;
#elif defined(HAVE__FTIME)
	struct _timeb tstruct;

	_ftime(&tstruct);
	secs = tstruct.time;
	usecs = tstruct.millitm*1000;
#else
# error You need to implement Sys_DoubleTime()
#endif

	if (!starttime) starttime = secs;
	
	return (secs - starttime) + usecs/1000000.0;
}
