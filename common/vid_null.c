/*
	vid_null.c

	null video driver to aid porting efforts

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

#include <quakedef.h>
#include <d_local.h>

extern viddef_t		vid;		// global video state

#define	BASEWIDTH	320
#define	BASEHEIGHT	200

byte	vid_buffer[BASEWIDTH*BASEHEIGHT];
short	zbuffer[BASEWIDTH*BASEHEIGHT];
byte	surfcache[256*1024];

unsigned short	d_8to16table[256];
unsigned	d_8to24table[256];

void
VID_SetPalette ( unsigned char *palette )
{
}

void
VID_ShiftPalette ( unsigned char *palette )
{
}

void
VID_Init ( unsigned char *palette )
{
	vid.maxwarpwidth = vid.width = vid.conwidth = BASEWIDTH;
	vid.maxwarpheight = vid.height = vid.conheight = BASEHEIGHT;
	vid.aspect = 1.0;
	vid.numpages = 1;
	vid.colormap = host_colormap;
	vid.fullbright = 256 - LittleLong (*((int *)vid.colormap + 2048));
	vid.buffer = vid.conbuffer = vid_buffer;
	vid.rowbytes = vid.conrowbytes = BASEWIDTH;

	d_pzbuffer = zbuffer;
	D_InitCaches (surfcache, sizeof(surfcache));
}

void
VID_Shutdown ( void )
{
}

void
VID_Update ( vrect_t *rects )
{
}

/*
================
D_BeginDirectRect
================
*/
void
D_BeginDirectRect ( int x, int y, byte *pbitmap, int width, int height )
{
}


/*
================
D_EndDirectRect
================
*/
void
D_EndDirectRect ( int x, int y, int width, int height )
{
}

int
VID_ExtraOptionDraw ( unsigned int options_draw_cursor )
{
	int	drawn;

	drawn = 0;

/* Port specific Options menu entries */
#if 0
	M_Print (16, options_draw_cursor+=8, "                 Dummy");
	M_DrawCheckbox (220, options_draw_cursor, dummy->value);
	drawn++;
#endif

	return drawn;	// return number of drawn menu entries
}

void
VID_ExtraOptionCmd ( int option_cursor, int dir )
{
/* dir: -1 = LEFT, 0 = ENTER, 1 = RIGHT */
#if 0
	switch(option_cursor) {
	case 0:	// Always start with 0
		dummy->value = !dummy->value;
		break;
	}
#endif
}

void
VID_LockBuffer ( void )
{
}

void
VID_UnlockBuffer ( void )
{
}
