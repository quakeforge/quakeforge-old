/*
	sys_common.c

	Common system routines

	Copyright (C) 1996-1997  Id Software, Inc.
	Copyright (C) 1999,2000  Nelson Rush.
	Copyright (C) 1999,2000  contributors of the QuakeForge project
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
	along with this program; if not, write to:

		Free Software Foundation, Inc.
		59 Temple Place - Suite 330
		Boston, MA  02111-1307, USA

	$Id$
*/

#include <quakedef.h>

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

#ifdef _WIN32
#include <limits.h>		// LONG_MAX
#include <windows.h>		// timeGetTime()
#endif

int nostdout = 0;
//cvar_t	sys_nostdout = {"sys_nostdout","0"};
cvar_t	*sys_nostdout;


char trans_table[256] = {
'\0', '#', '#', '#', '#', '.', '#', '#', '#', 9, 10, '#', ' ', 13, '.',
'.', '[', ']', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '.', '<',
'=', '>', ' ', '!', '"', '#', '$', '%', '&', '\'', '(', ')', '*', '+', ',',
'-', '.', '/', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', ':', ';',
'<', '=', '>', '?', '@', 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J',
'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y',
'Z', '[', '\\', ']', '^', '_', '`', 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h',
'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w',
'x', 'y', 'z', '{', '|', '}', '~', '<', '<', '=', '>', '#', '#', '.', '#',
'#', '#', '#', ' ', '#', ' ', '>', '.', '.', '[', ']', '0', '1', '2', '3',
'4', '5', '6', '7', '8', '9', '.', '<', '=', '>', ' ', '!', '"', '#', '$',
'%', '&', '\'', '(', ')', '*', '+', ',', '-', '.', '/', '0', '1', '2', '3',
'4', '5', '6', '7', '8', '9', ':', ';', '<', '=', '>', '?', '@', 'A', 'B',
'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q',
'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z', '[', '\\', ']', '^', '_', '`',
'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o',
'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z', '{', '|', '}', '~',
'<',
};

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

//	if (nostdout || sys_nostdout->value) return;

	va_start(argptr, fmt);
	vsnprintf(text, sizeof(text), fmt, argptr);
	va_end(argptr);

	for (p = (unsigned char *)text; *p; p++) {
	    putc(trans_table[*p], stdout);
	    /*
		if ((*p > 128 || *p < 32) && *p != 10 && *p != 13 && *p != 9)
			printf("[%02x]", *p);
		else
			putc(*p, stdout);
	    */
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
	QFile	*f;
#if defined(_WIN32) && !defined(SERVERONLY)
	int t = VID_ForceUnlockedAndReturnState();
#endif

	f = Qopen(path, "rb");
	if (f) {
		Qclose(f);
		ret = 1;
	}
#if defined(_WIN32) && !defined(SERVERONLY)
	VID_ForceLockState(t);
#endif
#endif /* HAVE_STAT */

	return ret;
}


#ifdef _WIN32
double Sys_DoubleTime (void)
{
	static DWORD starttime;
	static qboolean first = true;
	DWORD now;

	now = timeGetTime();

	if (first) {
		first = false;
		starttime = now;
		return 0.0;
	}

	if (now < starttime) // wrapped?
		return (now / 1000.0) + (LONG_MAX - starttime / 1000.0);

	if (now - starttime == 0)
		return 0.0;

	return (now - starttime) / 1000.0;
}

#else
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

#endif
