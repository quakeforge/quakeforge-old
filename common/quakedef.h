/*
quakedef.h - primary header for client
Copyright (C) 1996-1997 Id Software, Inc.
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
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

*/

#ifndef _QW_COMMON_QUAKEDEF_H
#define _QW_COMMON_QUAKEDEF_H

#include <common_quakedef.h>

#include <bothdefs.h>
#include <pmove.h>

extern	cvar_t	*password;

qboolean Host_SimulationTime(float time);
void Host_ClearMemory (void);

extern int			current_skill;		// skill level for currently loaded level (in case
										//  the user changes the cvar while the level is
										//  running, this reflects the level actually in use)
extern qboolean		isDedicated;

extern int			minimum_memory;

/*
	Chase camera
*/
extern	cvar_t	*cl_chasecam;

void Chase_Init (void);
void Chase_Reset (void);
void Chase_Update (void);

#endif /* _QW_COMMON_QUAKEDEF_H */
