/*
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

#ifndef __MODULES__
#define __MODULES__

#include <net.h>

// Input plugin interface.
typedef struct
{
    void *handle;
    char *filename;
    char *description;

    int (*Init)();
    void (*Shutdown)();
    void (*Commands)();
    void (*SendKeyEvents)();
    void (*Move)(usercmd_t *);
} input_pi;

extern input_pi *IN;

int plugin_load(char *filename);
void plugin_unload(void *handle);


#endif	// __MODULES__:w
