/*
	common_quakedef.h

	common header

	Copyright (C) 1996-1997  Id Software, Inc.
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

#ifndef _COMMON_QUAKEDEF_H
#define _COMMON_QUAKEDEF_H

#define	QUAKE_GAME			// as opposed to utilities

#include <config.h>                     // generated from config.h.in

#include <math.h>
#include <string.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <setjmp.h>
#include <time.h>
#include <cvar.h>
#ifdef HAVE_STRING_H
#include <strings.h>
#endif

#include <qtypes.h>
#include <qstructs.h>

#ifdef _WIN32
#define FNMATCH_FLAGS FNM_CASEFOLD
#else
#define FNMATCH_FLAGS 0
#endif


#ifndef max
#define max(a,b) ((a) > (b) ? (a) : (b))
#define min(a,b) ((a) < (b) ? (a) : (b))
#endif

#ifndef bound
#define bound(a,b,c) (max(a, min(b, c)))
#endif

/* This fixes warnings when compiling with -pedantic */
#if defined(__GNUC__) && !defined(inline)
# define inline __inline__
#endif

/* Win32 have these underscored... */
#if !defined(HAVE_SNPRINTF) && defined(HAVE__SNPRINTF)
# define snprintf _snprintf
#endif
#if !defined(HAVE_VSNPRINTF) && defined(HAVE__VSNPRINTF)
# define vsnprintf _vsnprintf
#endif

#ifndef HAVE_SOCKLEN_T
# ifdef HAVE_SIZE
typedef size_t socklen_t;
# else
typedef unsigned int socklen_t;
# endif
#endif

#define MAX_NUM_ARGVS	50


extern qboolean noclip_anglehack;


//
// host
//
extern	quakeparms_t host_parms;

extern	cvar_t		*sys_ticrate;
extern	cvar_t		*sys_nostdout;
extern	cvar_t		*developer;

extern	qboolean	host_initialized;// true if into command execution
extern	double		host_frametime;
extern	byte		*host_basepal;
extern	byte		*host_colormap;
extern	int		host_framecount;// incremented every frame, never reset
extern	double		realtime;	// not bounded in any way, changed at
										// start of every frame, never reset

void Host_ServerFrame (void);
void Host_InitCommands (void);
void Host_Init (quakeparms_t *parms);
void Host_Shutdown(void);
void Host_Error (char *error, ...);
void Host_EndGame (char *message, ...);
void Host_Frame (float time);
void Host_Quit_f (void);
void Host_ClientCommands (char *fmt, ...);
void Host_ShutdownServer (qboolean crash);

extern qboolean	msg_suppress_1;
// suppresses resolution and cache size console output
//  an fullscreen DIB focus gain/loss
#endif // _COMMON_QUAKEDEF_H
