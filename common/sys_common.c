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
