/*
	gl_screen.c

	master for refresh, status bar, console, chat, notify, etc

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
#include <glquake.h>
#include <qtypes.h>
#include <sys.h>
#include <cvar.h>
#include <keys.h>
#include <draw.h>
#include <sbar.h>
#include <cmd.h>
#include <console.h>
#include <sound.h>
#include <screen.h>
#include <menu.h>
#include <mathlib.h>
#include <input.h>
#include <plugin.h>
#include <time.h>
#include <client.h>

/*

background clear
rendering
turtle/net/ram icons
sbar
centerprint / slow centerprint
notify lines
intermission / finale overlay
loading plaque
console
menu

required background clears
required update regions


syncronous draw mode or async
One off screen buffer, with updates either copied or xblited
Need to double buffer?


async draw will require the refresh area to be cleared, because it will be
xblited, but sync draw can just ignore it.

sync
draw

CenterPrint ()
SlowPrint ()
Screen_Update ();
Con_Printf ();

net
turn off messages option

the refresh is always rendered, unless the console is full screen


console is:
	notify lines
	half
	full

*/


int		glx, gly, glwidth, glheight;

// only the refresh window will be updated unless these variables are flagged
int		scr_copytop;
int		scr_copyeverything;

float	scr_con_current;
float	scr_conlines;		// lines of console to display

float	oldscreensize, oldfov;
cvar_t	*scr_viewsize;
cvar_t	*scr_fov;
cvar_t	*scr_conspeed;
cvar_t	*scr_centertime;
cvar_t	*scr_showram;
cvar_t	*scr_showturtle;
cvar_t	*scr_showpause;
cvar_t	*scr_printspeed;
cvar_t	*scr_allowsnap;
cvar_t	*scr_consize;
cvar_t	*gl_triplebuffer;
extern cvar_t	*crosshair;

qboolean	scr_initialized;	// ready to draw

qpic_t	*scr_ram;
qpic_t	*scr_net;
qpic_t	*scr_turtle;

int		scr_fullupdate;

int		clearconsole;
int		clearnotify;

extern int	sb_lines;

viddef_t	vid;				// global video state

vrect_t		scr_vrect;

qboolean	scr_disabled_for_loading;
qboolean	scr_drawloading;
float		scr_disabled_time;

qboolean	block_drawing;

void SCR_ScreenShot_f (void);
void SCR_RSShot_f (void);

/*
===============================================================================

CENTER PRINTING

===============================================================================
*/

char	scr_centerstring[1024];
float	scr_centertime_start;	// for slow victory printing
float	scr_centertime_off;
int		scr_center_lines;
int		scr_erase_lines;
int		scr_erase_center;

/*
==============
SCR_CenterPrint

Called for important messages that should stay in the center of the screen
for a few moments
==============
*/
void
SCR_CenterPrint ( char *str )
{
	strncpy (scr_centerstring, str, sizeof(scr_centerstring)-1);
	scr_centertime_off = scr_centertime->value;
	scr_centertime_start = cl.time;

// count the number of lines for centering
	scr_center_lines = 1;
	while (*str)
	{
		if (*str == '\n')
			scr_center_lines++;
		str++;
	}
}


void
SCR_DrawCenterString ( void )
{
	char	*start;
	int		l;
	int		j;
	int		x, y;
	int		remaining;

// the finale prints the characters one at a time
	if (cl.intermission)
		remaining = scr_printspeed->value * (cl.time - scr_centertime_start);
	else
		remaining = 9999;

	scr_erase_center = 0;
	start = scr_centerstring;

	if (scr_center_lines <= 4)
		y = vid.height*0.35;
	else
		y = 48;

	do
	{
	// scan the width of the line
		for (l=0 ; l<40 ; l++)
			if (start[l] == '\n' || !start[l])
				break;
		x = (vid.width - l*8)/2;
		for (j=0 ; j<l ; j++, x+=8)
		{
			Draw_Character (x, y, start[j]);
			if (!remaining--)
				return;
		}

		y += 8;

		while (*start && *start != '\n')
			start++;

		if (!*start)
			break;
		start++;	// skip the \n
	} while (1);
}

void
SCR_CheckDrawCenterString ( void )
{
	scr_copytop = 1;
	if (scr_center_lines > scr_erase_lines)
		scr_erase_lines = scr_center_lines;

	scr_centertime_off -= host_frametime;

	if (scr_centertime_off <= 0 && !cl.intermission)
		return;
	if (key_dest != key_game)
		return;

	SCR_DrawCenterString ();
}

//=============================================================================

/*
====================
CalcFov
====================
*/
float
CalcFov ( float fov_x, float width, float height )
{
	float	a;
	float	x;

	if (fov_x < 1 || fov_x > 179)
		Sys_Error ("Bad fov: %f", fov_x);

	x = width/tan(fov_x/360*M_PI);

	a = atan (height/x);

	a = a*360/M_PI;

	return a;
}

/*
=================
SCR_CalcRefdef

Must be called whenever vid changes
Internal use only
=================
*/
static void
SCR_CalcRefdef ( void )
{
	float		size;
	int			h;
	qboolean	full = false;


	scr_fullupdate = 0;		// force a background redraw
	vid.recalc_refdef = 0;

// force the status bar to redraw
	Sbar_Changed ();

//========================================

// bound viewsize
	if (scr_viewsize->value < 30)
		Cvar_Set (scr_viewsize,"30");
	if (scr_viewsize->value > 120)
		Cvar_Set (scr_viewsize,"120");

// bound field of view
	if (scr_fov->value < 10)
		Cvar_Set (scr_fov,"10");
	if (scr_fov->value > 170)
		Cvar_Set (scr_fov,"170");

	size = scr_viewsize->value;

	if (size >= 120)
		sb_lines = 0;		// no status bar at all
	else if (size >= 110)
		sb_lines = 24;		// no inventory
	else
		sb_lines = 24+16+8;

	if (scr_viewsize->value >= 100.0) {
		full = true;
		size = 100.0;
	} else
		size = scr_viewsize->value;
	if (cl.intermission)
	{
		full = true;
		size = 100.0;
		sb_lines = 0;
	}
	size /= 100.0;

	if (!cl_sbar->value && full)
		h = vid.height;
	else
		h = vid.height - sb_lines;

	r_refdef.vrect.width = vid.width * size;
	if (r_refdef.vrect.width < 96)
	{
		size = 96.0 / r_refdef.vrect.width;
		r_refdef.vrect.width = 96;	// min for icons
	}

	r_refdef.vrect.height = vid.height * size;
	if (cl_sbar->value || !full) {
		if (r_refdef.vrect.height > vid.height - sb_lines)
			r_refdef.vrect.height = vid.height - sb_lines;
	} else if (r_refdef.vrect.height > vid.height)
			r_refdef.vrect.height = vid.height;
	r_refdef.vrect.x = (vid.width - r_refdef.vrect.width)/2;
	if (full)
		r_refdef.vrect.y = 0;
	else
		r_refdef.vrect.y = (h - r_refdef.vrect.height)/2;

	r_refdef.fov_x = scr_fov->value;
	r_refdef.fov_y = CalcFov (r_refdef.fov_x, r_refdef.vrect.width, r_refdef.vrect.height);

	scr_vrect = r_refdef.vrect;
}


/*
=================
SCR_SizeUp_f

Keybinding command
=================
*/
void
SCR_SizeUp_f ( void )
{
	scr_viewsize->value = scr_viewsize->value+10;
	vid.recalc_refdef = 1;
}


/*
=================
SCR_SizeDown_f

Keybinding command
=================
*/
void
SCR_SizeDown_f ( void )
{
	scr_viewsize->value = scr_viewsize->value-10;
	vid.recalc_refdef = 1;
}

//============================================================================

void
SCR_InitCvars ( void )
{
	scr_fov = Cvar_Get ("fov","90",CVAR_NONE,"None");
	scr_viewsize = Cvar_Get ("viewsize","100",CVAR_ARCHIVE,"None");
	scr_conspeed = Cvar_Get ("scr_conspeed","300",CVAR_ARCHIVE,"None");
	scr_showram = Cvar_Get ("scr_showram","1",CVAR_NONE,"None");
	scr_showturtle = Cvar_Get ("scr_showturtle","0",CVAR_NONE,"None");
	scr_showpause = Cvar_Get ("scr_showpause","1",CVAR_NONE,"None");
	scr_centertime = Cvar_Get ("scr_centertime","2",CVAR_NONE,"None");
	scr_printspeed = Cvar_Get ("scr_printspeed","8",CVAR_NONE,"None");
	scr_allowsnap = Cvar_Get ("scr_allowsnap","1",CVAR_NONE,"None");
	scr_consize = Cvar_Get ("scr_consize", "0.5", CVAR_NONE,
			"sets console size (0.5 is half screen");
	gl_triplebuffer = Cvar_Get ("gl_triplebuffer","1",CVAR_ARCHIVE,"None");
}

/*
==================
SCR_Init
==================
*/

void
SCR_Init ( void )
{
//
// register our commands
//
	Cmd_AddCommand ("screenshot",SCR_ScreenShot_f);
	Cmd_AddCommand ("snap",SCR_RSShot_f);
	Cmd_AddCommand ("sizeup",SCR_SizeUp_f);
	Cmd_AddCommand ("sizedown",SCR_SizeDown_f);

	scr_ram = Draw_PicFromWad ("ram");
	scr_net = Draw_PicFromWad ("net");
	scr_turtle = Draw_PicFromWad ("turtle");

	scr_initialized = true;
}


/*
==============
SCR_DrawRam
==============
*/
void
SCR_DrawRam ( void )
{
	if (!scr_showram->value)
		return;

	if (!r_cache_thrash)
		return;

	Draw_Pic (scr_vrect.x+32, scr_vrect.y, scr_ram);
}

/*
==============
SCR_DrawTurtle
==============
*/
void
SCR_DrawTurtle ( void )
{
	static int	count;

	if (!scr_showturtle->value)
		return;

	if (host_frametime < 0.1)
	{
		count = 0;
		return;
	}

	count++;
	if (count < 3)
		return;

	Draw_Pic (scr_vrect.x, scr_vrect.y, scr_turtle);
}

/*
==============
SCR_DrawNet
==============
*/
void
SCR_DrawNet ( void )
{
#ifdef QUAKEWORLD
	if (cls.netchan.outgoing_sequence - cls.netchan.incoming_acknowledged < UPDATE_BACKUP-1)
#else
	if (realtime - cl.last_received_message < 0.3)
#endif
		return;
	if (cls.demoplayback)
		return;

	Draw_Pic (scr_vrect.x+64, scr_vrect.y, scr_net);
}

/*
===============
SCR_DrawFPS

Backported by Jules Bean <jules@jellybean.co.uk> from
quakeworld client
===============
*/
void
SCR_DrawFPS ( void )
{
	extern cvar_t *show_fps;
	static double lastframetime;
	double t;
	extern int fps_count;
	static int lastfps;
	int x, y;
	char st[80];

	if (!show_fps->value)
		return;

	t = Sys_DoubleTime();
	if ((t - lastframetime) >= 1.0) {
		lastfps = fps_count;
		fps_count = 0;
		lastframetime = t;
	}

	snprintf(st, sizeof(st), "%3d FPS", lastfps);
	x = vid.width - strlen(st) * 8 - 8;
	y = vid.height - sb_lines - 0;
//	Draw_TileClear(x, y, strlen(st) * 8, 8);
	Draw_String(x, y, st);
}


/*
==============
DrawPause
==============
*/
void
SCR_DrawPause ( void )
{
	qpic_t	*pic;

	if (!scr_showpause->value)	// turn off for screenshots
		return;

	if (!cl.paused)
		return;

	pic = Draw_CachePic ("gfx/pause.lmp");
	Draw_Pic ( (vid.width - pic->width)/2,
		(vid.height - 48 - pic->height)/2, pic);
}



/*
==============
SCR_DrawLoading
==============
*/
void
SCR_DrawLoading ( void )
{
	qpic_t	*pic;

	if (!scr_drawloading)
		return;

	pic = Draw_CachePic ("gfx/loading.lmp");
	Draw_Pic ( (vid.width - pic->width)/2,
		(vid.height - 48 - pic->height)/2, pic);
}



//=============================================================================


/*
==================
SCR_SetUpToDrawConsole
==================
*/
void
SCR_SetUpToDrawConsole ( void )
{
	Con_CheckResize ();

	if (scr_drawloading)
		return;		// never a console with loading plaque

// decide on the height of the console
	if (cls.state != ca_active)
	{
		scr_conlines = vid.height;		// full screen
		scr_con_current = scr_conlines;
	}
	else if (key_dest == key_console)
	{
		scr_conlines = vid.height * max(0.2,
				min(scr_consize->value, 1));
	}
	else
		scr_conlines = 0;			// none visible

	if (scr_conlines < scr_con_current)
	{
		scr_con_current -= scr_conspeed->value * host_frametime * 4;
		if (scr_conlines > scr_con_current)
			scr_con_current = scr_conlines;

	}
	else if (scr_conlines > scr_con_current)
	{
		scr_con_current += scr_conspeed->value * host_frametime * 4;
		if (scr_conlines < scr_con_current)
			scr_con_current = scr_conlines;
	}

	if (clearconsole++ < vid.numpages)
	{
		Sbar_Changed ();
	}
	else if (clearnotify++ < vid.numpages)
	{
	}
	else
		con_notifylines = 0;
}

/*
==================
SCR_DrawConsole
==================
*/
void
SCR_DrawConsole ( void )
{
	if (scr_con_current)
	{
		scr_copyeverything = 1;
		Con_DrawConsole (scr_con_current);
		clearconsole = 0;
	}
	else
	{
		if (key_dest == key_game || key_dest == key_message)
			Con_DrawNotify ();	// only draw notify in game
	}
}


/*
==============================================================================

						SCREEN SHOTS

==============================================================================
*/

typedef struct _TargaHeader {
	unsigned char	id_length, colormap_type, image_type;
	unsigned short	colormap_index, colormap_length;
	unsigned char	colormap_size;
	unsigned short	x_origin, y_origin, width, height;
	unsigned char	pixel_size, attributes;
} TargaHeader;


/*
==================
SCR_ScreenShot_f
==================
*/
void
SCR_ScreenShot_f ( void )
{
	byte	*buffer;
	char	pcxname[80];
	char	checkname[MAX_OSPATH];
	int		i, c, temp;
//
// find a file name to save it to
//
	strcpy(pcxname,"qf000.tga");

	for (i=0 ; i<=999 ; i++)
	{
		pcxname[2] = i / 100 + '0';
		pcxname[3] = i / 10 % 10 + '0';
		pcxname[4] = i % 10 + '0';

		snprintf(checkname, sizeof(checkname), "%s/%s", com_gamedir, pcxname);
		if (Sys_FileTime(checkname) == -1)
			break;	// file doesn't exist
	}
	if (i==1000)
	{
		Con_Printf ("SCR_ScreenShot_f: Couldn't create a PCX file\n");
		return;
	}


	buffer = malloc(glwidth*glheight*3 + 18);
	memset (buffer, 0, 18);
	buffer[2] = 2;	// uncompressed type
	buffer[12] = glwidth&255;
	buffer[13] = glwidth>>8;
	buffer[14] = glheight&255;
	buffer[15] = glheight>>8;
	buffer[16] = 24;	// pixel size

	glReadPixels (glx, gly, glwidth, glheight, GL_RGB, GL_UNSIGNED_BYTE, buffer+18 );

	// swap rgb to bgr
	c = 18+glwidth*glheight*3;
	for (i=18 ; i<c ; i+=3)
	{
		temp = buffer[i];
		buffer[i] = buffer[i+2];
		buffer[i+2] = temp;
	}
	COM_WriteFile (pcxname, buffer, glwidth*glheight*3 + 18 );

	free (buffer);
	Con_Printf ("Wrote %s\n", pcxname);
}

/*
==============
WritePCXfile
==============
*/
void
WritePCXfile ( char *filename, byte *data, int width, int height,
	int rowbytes, byte *palette, qboolean upload )
{
	int		i, j, length;
	pcx_t	*pcx;
	byte		*pack;

	pcx = Hunk_TempAlloc (width*height*2+1000);
	if (pcx == NULL)
	{
		Con_Printf("SCR_ScreenShot_f: not enough memory\n");
		return;
	}

	pcx->manufacturer = 0x0a;	// PCX id
	pcx->version = 5;			// 256 color
 	pcx->encoding = 1;		// uncompressed
	pcx->bits_per_pixel = 8;		// 256 color
	pcx->xmin = 0;
	pcx->ymin = 0;
	pcx->xmax = LittleShort((short)(width-1));
	pcx->ymax = LittleShort((short)(height-1));
	pcx->hres = LittleShort((short)width);
	pcx->vres = LittleShort((short)height);
	Q_memset (pcx->palette,0,sizeof(pcx->palette));
	pcx->color_planes = 1;		// chunky image
	pcx->bytes_per_line = LittleShort((short)width);
	pcx->palette_type = LittleShort(2);		// not a grey scale
	Q_memset (pcx->filler,0,sizeof(pcx->filler));

// pack the image
	pack = &pcx->data;

	data += rowbytes * (height - 1);

	for (i=0 ; i<height ; i++)
	{
		for (j=0 ; j<width ; j++)
		{
			if ( (*data & 0xc0) != 0xc0)
				*pack++ = *data++;
			else
			{
				*pack++ = 0xc1;
				*pack++ = *data++;
			}
		}

		data += rowbytes - width;
		data -= rowbytes * 2;
	}

// write the palette
	*pack++ = 0x0c;	// palette ID byte
	for (i=0 ; i<768 ; i++)
		*pack++ = *palette++;

// write output file
	length = pack - (byte *)pcx;
#ifdef QUAKEWORLD
	if (upload)
		CL_StartUpload((void *)pcx, length);
	else
#endif
		COM_WriteFile (filename, pcx, length);
}



/*
Find closest color in the palette for named color
*/
int
MipColor ( int r, int g, int b )
{
	int i;
	float dist;
	int best = 0;
	float bestdist;
	int r1, g1, b1;
	static int lr = -1, lg = -1, lb = -1;
	static int lastbest;

	if (r == lr && g == lg && b == lb)
		return lastbest;

	bestdist = 256*256*3;

	for (i = 0; i < 256; i++) {
		r1 = host_basepal[i*3] - r;
		g1 = host_basepal[i*3+1] - g;
		b1 = host_basepal[i*3+2] - b;
		dist = r1*r1 + g1*g1 + b1*b1;
		if (dist < bestdist) {
			bestdist = dist;
			best = i;
		}
	}
	lr = r; lg = g; lb = b;
	lastbest = best;
	return best;
}

// from gl_draw.c
extern byte	*draw_chars;		// 8*8 graphic characters

void
SCR_DrawCharToSnap ( int num, byte *dest, int width )
{
	int		row, col;
	byte	*source;
	int		drawline;
	int		x;

	row = num>>4;
	col = num&15;
	source = draw_chars + (row<<10) + (col<<3);

	drawline = 8;

	while (drawline--)
	{
		for (x=0 ; x<8 ; x++)
			if (source[x])
				dest[x] = source[x];
			else
				dest[x] = 98;
		source += 128;
		dest -= width;
	}

}

void
SCR_DrawStringToSnap ( const char *s, byte *buf, int x, int y, int width )
{
	byte *dest;
	const unsigned char *p;

	dest = buf + ((y * width) + x);

	p = (const unsigned char *)s;
	while (*p) {
		SCR_DrawCharToSnap(*p++, dest, width);
		dest += 8;
	}
}


/*
==================
SCR_RSShot_f
==================
*/
void
SCR_RSShot_f ( void )
{
//	int		i;
	int		x, y;
	unsigned char		*src, *dest;
	char		pcxname[80];
//	char		checkname[MAX_OSPATH];
	unsigned char		*newbuf;
	int w, h;
	int dx, dy, dex, dey, nx;
	int r, b, g;
	int count;
	float fracw, frach;
	char st[80];
	time_t now;
#ifdef QUAKEWORLD
	if (CL_IsUploading())
		return;		// already one pending
#endif
	if (cls.state < ca_onserver)
		return;		// gotta be connected

	Con_Printf("Remote screen shot requested.\n");

#if 0
//
// find a file name to save it to
//
	strcpy(pcxname,"mquake00.pcx");

	for (i=0 ; i<=99 ; i++)
	{
		pcxname[6] = i/10 + '0';
		pcxname[7] = i%10 + '0';
		snprintf(checkname, sizeof(checkname), "%s/%s", com_gamedir, pcxname);
		if (Sys_FileTime(checkname) == -1)
			break;	// file doesn't exist
	}
	if (i==100)
	{
		Con_Printf ("SCR_ScreenShot_f: Couldn't create a PCX");
		return;
	}
#endif

//
// save the pcx file
//
	newbuf = malloc(glheight * glwidth * 3);

	glReadPixels (glx, gly, glwidth, glheight, GL_RGB, GL_UNSIGNED_BYTE, newbuf );

	w = (vid.width < RSSHOT_WIDTH) ? glwidth : RSSHOT_WIDTH;
	h = (vid.height < RSSHOT_HEIGHT) ? glheight : RSSHOT_HEIGHT;

	fracw = (float)glwidth / (float)w;
	frach = (float)glheight / (float)h;

	for (y = 0; y < h; y++) {
		dest = newbuf + (w*3 * y);

		for (x = 0; x < w; x++) {
			r = g = b = 0;

			dx = x * fracw;
			dex = (x + 1) * fracw;
			if (dex == dx) dex++;	// at least one
			dy = y * frach;
			dey = (y + 1) * frach;
			if (dey == dy) dey++;	// at least one

			count = 0;
			for (/* */; dy < dey; dy++) {
				src = newbuf + (glwidth * 3 * dy) + dx * 3;
				for (nx = dx; nx < dex; nx++) {
					r += *src++;
					g += *src++;
					b += *src++;
					count++;
				}
			}
			r /= count;
			g /= count;
			b /= count;
			*dest++ = r;
			*dest++ = b;
			*dest++ = g;
		}
	}

	// convert to eight bit
	for (y = 0; y < h; y++) {
		src = newbuf + (w * 3 * y);
		dest = newbuf + (w * y);

		for (x = 0; x < w; x++) {
			*dest++ = MipColor(src[0], src[1], src[2]);
			src += 3;
		}
	}

	time(&now);
	strcpy(st, ctime(&now));
	st[strlen(st) - 1] = 0;
	SCR_DrawStringToSnap (st, newbuf, w - strlen(st)*8, h - 1, w);

	strncpy(st, cls.servername, sizeof(st));
	st[sizeof(st) - 1] = 0;
	SCR_DrawStringToSnap (st, newbuf, w - strlen(st)*8, h - 11, w);
#ifdef QUAKEWORLD
	strncpy(st, name->string, sizeof(st));
#else
	strncpy(st, cl_name->string, sizeof(st));
#endif
	st[sizeof(st) - 1] = 0;
	SCR_DrawStringToSnap (st, newbuf, w - strlen(st)*8, h - 21, w);

	WritePCXfile (pcxname, newbuf, w, h, w, host_basepal, true);

	free(newbuf);

	Con_Printf ("Wrote %s\n", pcxname);
}




//=============================================================================

/*
===============
SCR_BeginLoadingPlaque

================
*/
void
SCR_BeginLoadingPlaque ( void )
{
	S_StopAllSounds (true);

	if (cls.state < ca_connected)
		return;
	if (cls.signon != SIGNONS)
		return;

// redraw with no console and the loading plaque
	Con_ClearNotify ();
	scr_centertime_off = 0;
	scr_con_current = 0;

	scr_drawloading = true;
	scr_fullupdate = 0;
	Sbar_Changed ();
	SCR_UpdateScreen ();
	scr_drawloading = false;

	scr_disabled_for_loading = true;
	scr_disabled_time = realtime;
	scr_fullupdate = 0;
}

/*
===============
SCR_EndLoadingPlaque

================
*/
void
SCR_EndLoadingPlaque ( void )
{
	scr_disabled_for_loading = false;
	scr_fullupdate = 0;
	Con_ClearNotify ();
}

//=============================================================================

char	*scr_notifystring;
qboolean	scr_drawdialog;

void
SCR_DrawNotifyString ( void )
{
	char	*start;
	int		l;
	int		j;
	int		x, y;

	start = scr_notifystring;

	y = vid.height*0.35;

	do
	{
	// scan the width of the line
		for (l=0 ; l<40 ; l++)
			if (start[l] == '\n' || !start[l])
				break;
		x = (vid.width - l*8)/2;
		for (j=0 ; j<l ; j++, x+=8)
			Draw_Character (x, y, start[j]);

		y += 8;

		while (*start && *start != '\n')
			start++;

		if (!*start)
			break;
		start++;		// skip the \n
	} while (1);
}

/*
==================
SCR_ModalMessage

Displays a text string in the center of the screen and waits for a Y or N
keypress.
==================
*/
int
SCR_ModalMessage ( char *text )
{
#ifdef UQUAKE
	if (cls.state == ca_dedicated)
		return true;
#endif

	scr_notifystring = text;

// draw a fresh screen
	scr_fullupdate = 0;
	scr_drawdialog = true;
	SCR_UpdateScreen ();
	scr_drawdialog = false;

	S_ClearBuffer ();		// so dma doesn't loop current sound

	do
	{
		key_count = -1;		// wait for a key down and up
		IN_SendKeyEvents ();
	} while (key_lastpress != 'y' && key_lastpress != 'n' && key_lastpress != K_ESCAPE);

	scr_fullupdate = 0;
	SCR_UpdateScreen ();

	return key_lastpress == 'y';
}


//=============================================================================

/*
===============
SCR_BringDownConsole

Brings the console down and fades the palettes back to normal
================
*/
void
SCR_BringDownConsole ( void )
{
	int		i;

	scr_centertime_off = 0;

	for (i=0 ; i<20 && scr_conlines != scr_con_current ; i++)
		SCR_UpdateScreen ();

	cl.cshifts[0].percent = 0;	// no area contents palette on next frame
	VID_SetPalette (host_basepal);
}

void
SCR_TileClear ( void )
{
	if (r_refdef.vrect.x > 0) {
		// left
		Draw_TileClear (0, 0, r_refdef.vrect.x, vid.height - sb_lines);
		// right
		Draw_TileClear (r_refdef.vrect.x + r_refdef.vrect.width, 0,
			vid.width - r_refdef.vrect.x + r_refdef.vrect.width,
			vid.height - sb_lines);
	}
	if (r_refdef.vrect.y > 0) {
		// top
		Draw_TileClear (r_refdef.vrect.x, 0,
			r_refdef.vrect.x + r_refdef.vrect.width,
			r_refdef.vrect.y);
		// bottom
		Draw_TileClear (r_refdef.vrect.x,
			r_refdef.vrect.y + r_refdef.vrect.height,
			r_refdef.vrect.width,
			vid.height - sb_lines -
			(r_refdef.vrect.height + r_refdef.vrect.y));
	}
}

float oldsbar = 0;

/*
==================
SCR_UpdateScreen

This is called every frame, and can also be called explicitly to flush
text to the screen.

WARNING: be very careful calling this from elsewhere, because the refresh
needs almost the entire 256k of stack space!
==================
*/
void
SCR_UpdateScreen ( void )
{
	if (block_drawing)
		return;

	vid.numpages = 2 + gl_triplebuffer->value;

	scr_copytop = 0;
	scr_copyeverything = 0;

	if (scr_disabled_for_loading)
	{
		if (realtime - scr_disabled_time > 60)
		{
			scr_disabled_for_loading = false;
			Con_Printf ("load failed.\n");
		}
		else
			return;
	}

	if (!scr_initialized || !con_initialized)
		return;		// not initialized yet


	if (oldsbar != cl_sbar->value) {
		oldsbar = cl_sbar->value;
		vid.recalc_refdef = true;
	}

	GL_BeginRendering (&glx, &gly, &glwidth, &glheight);

	//
	// determine size of refresh window
	//
	if (oldfov != scr_fov->value)
	{
		oldfov = scr_fov->value;
		vid.recalc_refdef = true;
	}

	if (vid.recalc_refdef)
		SCR_CalcRefdef ();

//
// do 3D refresh drawing, and then update the screen
//
	SCR_SetUpToDrawConsole ();

	V_RenderView ();

	GL_Set2D ();

	//
	// draw any areas not covered by the refresh
	//
	SCR_TileClear ();
#ifdef QUAKEWORLD
	if (r_netgraph->value)
		R_NetGraph ();
#endif

	if (scr_drawdialog)
	{
		Sbar_Draw ();
		Draw_FadeScreen ();
		SCR_DrawNotifyString ();
		scr_copyeverything = true;
	}
	else if (scr_drawloading)
	{
		SCR_DrawLoading ();
		Sbar_Draw ();
	}
	else if (cl.intermission == 1 && key_dest == key_game)
	{
		Sbar_IntermissionOverlay ();
	}
	else if (cl.intermission == 2 && key_dest == key_game)
	{
		Sbar_FinaleOverlay ();
		SCR_CheckDrawCenterString ();
	}
	else
	{
		if (crosshair->value)
			Draw_Crosshair();

		SCR_DrawRam ();
		SCR_DrawNet ();
		SCR_DrawFPS ();
		SCR_DrawTurtle ();
		SCR_DrawPause ();
		SCR_CheckDrawCenterString ();
		Sbar_Draw ();
		SCR_DrawConsole ();
		M_Draw ();
	}

	V_UpdatePalette ();

	GL_EndRendering ();
}
