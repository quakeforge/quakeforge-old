/*
Copyright (C) 1996-1997 Id Software, Inc.

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

cvar_t registered = {"registered","0"};

/*
================
COM_CheckRegistered

Looks for the pop.txt file.
Sets the "registered" cvar.
================
*/
void register_check ()
{
    FILE *h;

    Cvar_RegisterVariable (&registered);

    COM_FOpenFile("gfx/pop.lmp", &h);

    if (!h) {
	Con_Printf ("Playing shareware version.\n");
    } else {
	Cvar_Set ("registered", "1");
	Con_Printf ("Playing registered version.\n");
    }

    fclose (h);
}
