/*
	register_check.c

	Check for registered version of Quake data files

	Copyright (C) 1996-1997  Id Software, Inc.
	Copyright (C) 1999,2000  contributors of the QuakeForge project
	Please see the file "AUTHORS" for a list of contributors

	Author: Zephaniah E. Hull
	Date: 03 Jan 2000

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
*/

#include "cvar.h"
#include "quakefs.h"
#include "quakedef.h"

cvar_t registered = {"registered", "0"};

/*
	register_check
	
	Look for gfx/pop.lmp (only found in ID's registered quake pak files),
	and set the "registered" cvar to 1 if found.
*/
void
register_check ( void ) {

	FILE *h;

	Cvar_RegisterVariable (&registered);

	COM_FOpenFile("gfx/pop.lmp", &h);

	if (h) {
		Cvar_Set ("registered", "1");
		fclose (h);
	}
}
