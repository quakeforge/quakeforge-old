/*
vid_ggi.c - general LibGGI video driver
Copyright (C) 1999       Marcus Sundberg [mackan@stacken.kth.se]
Portions Copyright (C) 1996-1997  Id Software, Inc.

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

#define _BSD


#include <ctype.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ggi/ggi.h>

#include <quakedef.h>
#include <d_local.h>
#include <sound.h>
#include <keys.h>
#include <cvar.h>
#include <menu.h>
#include <sys.h>
#include <lib_replace.h>
#include <draw.h>
#include <console.h>
#include <client.h>

viddef_t	vid; // global video state
unsigned short	d_8to16table[256];

cvar_t		_windowed_mouse = {"_windowed_mouse","0", true};
cvar_t		m_filter = {"m_filter","0", true};

#define NUM_STDBUTTONS	3
#define NUM_BUTTONS	10

static qboolean	mouse_avail;
static float	mouse_x, mouse_y;
static float	old_mouse_x, old_mouse_y;
static int	p_mouse_x;
static int	p_mouse_y;

static ggi_visual_t		ggivis = NULL;
static ggi_mode			mode;
static const ggi_directbuffer	*dbuf1 = NULL, *dbuf2 = NULL;

static uint8	*drawptr = NULL;
static void	*frameptr[2] = { NULL, NULL };
static void	*oneline = NULL;
static void	*palette = NULL;
static int	curframe = 0;

static int	realwidth, realheight;
static int	doublebuffer;
static int	scale;
static int	stride, drawstride;
static int	pixelsize;
static int	usedbuf, havedbuf;

static long GGI_highhunkmark, GGI_buffersize;

static int	vid_surfcachesize;
static void	*vid_surfcache;

int	VID_options_items = 1;

static void
do_scale8(int xsize, int ysize, uint8 *dest, uint8 *src)
{
	int i, j, destinc = stride*2-xsize*2;
	for (j = 0; j < ysize; j++) {
		for (i = 0; i < xsize; /* i is incremented below */) {
			register uint32 pix1 = src[i++], pix2 = src[i++];
#ifdef GGI_LITTLE_ENDIAN
			*((uint32 *) (dest + stride))
				= *((uint32 *) dest)
				= (pix1 | (pix1 << 8)
				   | (pix2 << 16) | (pix2 << 24));
#else
			*((uint32 *) (dest + stride))
				= *((uint32 *) dest)
				= (pix2 | (pix2 << 8)
				   | (pix1 << 16) | (pix1 << 24));
#endif
			dest += 4;
		}
		dest += destinc;
		src += xsize;
	}
}

static void
do_scale16(int xsize, int ysize, uint8 *dest, uint8 *src)
{
	int i, j, destinc = stride*2-xsize*4;
	uint16 *palptr = palette;
	for (j = 0; j < ysize; j++) {
		for (i = 0; i < xsize; /* i is incremented below */) {
			register uint32 pixel = palptr[src[i++]];
			*((uint32 *) (dest + stride))
				= *((uint32 *) dest)
				= pixel | (pixel << 16);
			dest += 4;
		}
		dest += destinc;
		src += xsize;
	}
}

static void
do_scale32(int xsize, int ysize, uint8 *dest, uint8 *src)
{
	int i, j, destinc = stride*2-xsize*8;
	uint32 *palptr = palette;
	for (j = 0; j < ysize; j++) {
		for (i = 0; i < xsize; /* i is incremented below */) {
			register uint32 pixel = palptr[src[i++]];
			*((uint32 *) (dest + stride))
				= *((uint32 *) (dest)) = pixel;
			dest += 4;
			*((uint32 *) (dest + stride))
				= *((uint32 *) (dest)) = pixel;
			dest += 4;
		}
		dest += destinc;
		src += xsize;
	}
}


static void
do_copy8(int xsize, int ysize, uint8 *dest, uint8 *src)
{
	int i, j;
	uint8 *palptr = palette;
	
	for (j = 0; j < ysize; j++) {
		for (i = 0; i < xsize; i++) {
			dest[i] = palptr[src[i]];
		}
		dest += stride;
		src += xsize;
	}
}

static void
do_copy16(int xsize, int ysize, void *destptr, uint8 *src)
{
	int i, j, destinc = (stride/2 - xsize)/2;
	uint16 *palptr = palette;
	uint32 *dest = destptr;
	
	for (j = 0; j < ysize; j++) {
		for (i = 0; i < xsize;  /* i is incremented below */) {
			register uint32 pixel = palptr[src[i++]];
#ifdef GGI_LITTLE_ENDIAN
			*(dest++) = pixel | (palptr[src[i++]] << 16);
#else
			*(dest++) = (palptr[src[i++]] << 16) | pixel;
#endif
		}
		dest += destinc;
		src += xsize;
	}
}

static void
do_copy32(int xsize, int ysize, uint32 *dest, uint8 *src)
{
	int i, j, destinc = stride/4;
	uint32 *palptr = palette;
	
	for (j = 0; j < ysize; j++) {
		for (i = 0; i < xsize; i++) {
			dest[i] = palptr[src[i]];
		}
		dest += destinc;
		src += xsize;
	}
}


// ========================================================================
// Tragic death handler
// ========================================================================

void ResetFrameBuffer(void)
{
	if (d_pzbuffer)
	{
		D_FlushCaches ();
		Hunk_FreeToHighMark (GGI_highhunkmark);
		d_pzbuffer = NULL;
	}
	GGI_highhunkmark = Hunk_HighMark ();

// alloc an extra line in case we want to wrap, and allocate the z-buffer
	GGI_buffersize = vid.width * vid.height * sizeof (*d_pzbuffer);

	vid_surfcachesize = D_SurfaceCacheForRes (vid.width, vid.height);

	GGI_buffersize += vid_surfcachesize;

	d_pzbuffer = Hunk_HighAllocName (GGI_buffersize, "video");
	if (d_pzbuffer == NULL)
		Sys_Error ("Not enough memory for video mode\n");

	vid_surfcache = (byte *) d_pzbuffer
		+ vid.width * vid.height * sizeof (*d_pzbuffer);

	D_InitCaches(vid_surfcache, vid_surfcachesize);
}



// Called at startup to set up translation tables, takes 256 8 bit RGB values
// the palette data will go away after the call, so it must be copied off if
// the video driver will need it again

void	VID_Init(unsigned char *pal)
{
	int pnum;

	/* Initalize sound */
	S_Init();

	vid.width = GGI_AUTO;
	vid.height = GGI_AUTO;

	srandom(getpid());

	if (ggiInit() < 0) {
		Sys_Error("VID: Unable to init LibGGI\n");
	}
	ggivis = ggiOpen(NULL);
	if (!ggivis) {
		Sys_Error("VID: Unable to open default visual\n");
	}

	/* Go into async mode */
	ggiSetFlags(ggivis, GGIFLAG_ASYNC);

	/* check for command-line window size */
	if ((pnum=COM_CheckParm("-winsize")))
	{
		if (pnum >= com_argc-2)
			Sys_Error("VID: -winsize <width> <height>\n");
		vid.width = Q_atoi(com_argv[pnum+1]);
		vid.height = Q_atoi(com_argv[pnum+2]);
		if (!vid.width || !vid.height)
			Sys_Error("VID: Bad window width/height\n");
	}
	if ((pnum=COM_CheckParm("-width"))) {
		if (pnum >= com_argc-1)
			Sys_Error("VID: -width <width>\n");
		vid.width = Q_atoi(com_argv[pnum+1]);
		if (!vid.width)
			Sys_Error("VID: Bad window width\n");
	}
	if ((pnum=COM_CheckParm("-height"))) {
		if (pnum >= com_argc-1)
			Sys_Error("VID: -height <height>\n");
		vid.height = Q_atoi(com_argv[pnum+1]);
		if (!vid.height)
			Sys_Error("VID: Bad window height\n");
	}

	scale = COM_CheckParm("-scale");

	/* specify a LibGGI mode */
	if ((pnum=COM_CheckParm("-ggimode")))
	{
		if (pnum >= com_argc-1)
			Sys_Error("VID: -ggimode <mode>\n");
		ggiParseMode(com_argv[pnum+1], &mode);
	} else {
		/* This will give the default mode */
		ggiParseMode("", &mode);
		/* Now put in any parameters given above */
		mode.visible.x = vid.width;
		mode.visible.y = vid.height;
	}

	if (scale) {
		mode.visible.x *= 2;
		mode.visible.y *= 2;
	}

	/* We prefer 8 bit mode unless otherwise specified */
	if (mode.graphtype == GT_AUTO) mode.graphtype = GT_8BIT;

	/* We want double buffering if possible */
	if (mode.frames == GGI_AUTO) {
		ggi_mode tmpmode = mode;

		tmpmode.frames = 2;
		if (ggiCheckMode(ggivis, &tmpmode) == 0) {
			mode = tmpmode;
		} else {
			tmpmode.frames = 2;
			if (ggiCheckMode(ggivis, &tmpmode) == 0) {
				mode = tmpmode;
			}
		}
	}

	if (ggiSetMode(ggivis, &mode) != 0) {
		/* Try again with suggested mode */
		if (ggiSetMode(ggivis, &mode) != 0) {
			Sys_Error("VID: LibGGI can't set any modes!\n");
		}
	}

	/* Pixel size must be 1, 2 or 4 bytes */
	if (GT_SIZE(mode.graphtype) != 8 &&
	    GT_SIZE(mode.graphtype) != 16 &&
	    GT_SIZE(mode.graphtype) != 32) {
		if (GT_SIZE(mode.graphtype) == 24) {
			Sys_Error("VID: 24 bits per pixel not supported - try using the palemu target.\n");
		} else {
			Sys_Error("VID: %d bits per pixel not supported by GGI Quake.\n",
				  GT_SIZE(mode.graphtype));
		}
	}

	realwidth  = mode.visible.x;
	realheight = mode.visible.y;
	if (scale) {
		vid.width  = realwidth / 2;
		vid.height = realheight / 2;
	} else {
		vid.width  = realwidth;
		vid.height = realheight;
	}

	if (mode.frames >= 2) doublebuffer = 1;
	else doublebuffer = 0;

	pixelsize = (GT_SIZE(mode.graphtype)+7) / 8;
	if (mode.graphtype != GT_8BIT) {
		if ((palette = malloc(pixelsize*256)) == NULL) {
			Sys_Error("VID: Unable to allocate palette table\n");
		}
	}

	VID_SetPalette(pal);

	usedbuf = havedbuf = 0;
	drawstride = vid.width;
	stride = realwidth*pixelsize;
	if ((dbuf1 = ggiDBGetBuffer(ggivis, 0)) != NULL &&
	    (dbuf1->type & GGI_DB_SIMPLE_PLB)) {
		havedbuf = 1;
		stride = dbuf1->buffer.plb.stride;
		if (doublebuffer) {
			if  ((dbuf2 = ggiDBGetBuffer(ggivis, 1)) == NULL ||
			     !(dbuf2->type & GGI_DB_SIMPLE_PLB)) {
				/* Only one DB? No double buffering then */
				doublebuffer = 0;
			}
		}
		if (doublebuffer) {
			fprintf(stderr,	"VID: Got two DirectBuffers\n");
		} else {
			fprintf(stderr,	"VID: Got one DirectBuffer\n");
		}
		if (doublebuffer && !scale && !palette) {
			usedbuf = 1;
			drawstride = stride;
			frameptr[0] = dbuf1->write;
			if (doublebuffer) {
				frameptr[1] = dbuf2->write;
			} else {
				frameptr[1] = frameptr[0];
			}
			drawptr = frameptr[0];
			fprintf(stderr,	"VID: Drawing into DirectBuffer\n");
		}
	}

	if (!usedbuf) {
		if ((drawptr = malloc(vid.width * vid.height)) == NULL) {
			Sys_Error("VID: Unable to allocate draw buffer\n");			
		}
		if (!havedbuf && (scale || palette)) {
			int linesize = pixelsize*realwidth;
			if (scale) linesize *= 4;
			if ((oneline = malloc(linesize)) == NULL) {
				Sys_Error("VID: Unable to allocate line buffer\n");
			}
		}
		fprintf(stderr,
			"VID: Drawing into offscreen memory\n");
	}

	ResetFrameBuffer();

	curframe = 0;
	vid.maxwarpwidth = WARP_WIDTH;
	vid.maxwarpheight = WARP_HEIGHT;
	vid.numpages = doublebuffer ? 2 : 1;
	vid.colormap = host_colormap;
	vid.buffer = drawptr;
	vid.rowbytes = drawstride;
	vid.direct = drawptr;
	vid.conbuffer = vid.buffer;
	vid.conrowbytes = vid.rowbytes;
	vid.conwidth = vid.width;
	vid.conheight = vid.height;
	vid.aspect = ((float)vid.height / (float)vid.width) * (320.0 / 240.0);
	vid.fullbright = 256 - LittleLong (*((int *)vid.colormap + 2048));
}

void VID_ShiftPalette(unsigned char *pal)
{
	VID_SetPalette(pal);
}



void VID_SetPalette(unsigned char *pal)
{

	int i;
	ggi_color colors[256];

	for (i=0 ; i<256 ; i++) {
		colors[i].r = pal[i*3] * 257;
		colors[i].g = pal[i*3+1] * 257;
		colors[i].b = pal[i*3+2] * 257;
	}
	if (palette) {
		ggiPackColors(ggivis, palette, colors, 256);
	} else {
		ggiSetPalette(ggivis, 0, 256, colors);
	}
}

// Called at shutdown

void	VID_Shutdown (void)
{
	Con_Printf("VID_Shutdown\n");

	if (!usedbuf) {
		free(drawptr);
		drawptr = NULL;
	}
	if (oneline) {
		free(oneline);
		oneline = NULL;
	}
	if (palette) {
		free(palette);
		palette = NULL;
	}
	if (ggivis) {
		ggiClose(ggivis);
		ggivis = NULL;
	}
	ggiExit();
}


// flushes the given rectangles from the view buffer to the screen

void	VID_Update(vrect_t *rects)
{
	int height = 0;

#if 0
// if the window changes dimension, skip this frame

	if (config_notify)
	{
		fprintf(stderr, "config notify\n");
		config_notify = 0;
		vid.width = config_notify_width & ~7;
		vid.height = config_notify_height;
		if (doShm)
			ResetSharedFrameBuffers();
		else
			ResetFrameBuffer();
		vid.rowbytes = x_framebuffer[0]->bytes_per_line;
		vid.buffer = x_framebuffer[curframe]->data;
		vid.conbuffer = vid.buffer;
		vid.conwidth = vid.width;
		vid.conheight = vid.height;
		vid.conrowbytes = vid.rowbytes;
		vid.recalc_refdef = 1;				// force a surface cache flush
		Con_CheckResize();
		Con_Clear_f();
		return;
	}

	// force full update if not 8bit
	if (x_visinfo->depth != 8) {
		extern int scr_fullupdate;

		scr_fullupdate = 0;
	}
#endif

	while (rects) {
		int y = rects->y + rects->height;
		if (y > height) height = y;
		rects = rects->pnext;
	}

	if (!usedbuf) {
		int	i;

		if (havedbuf) {
			if (ggiResourceAcquire(dbuf1->resource,
					       GGI_ACTYPE_WRITE) != 0 ||
			    (doublebuffer ?
			     ggiResourceAcquire(dbuf2->resource,
						GGI_ACTYPE_WRITE) != 0
			     : 0)) {
				ggiPanic("Unable to acquire DirectBuffer!\n");
			}
			/* ->write is allowed to change at acquire time */
			frameptr[0] = dbuf1->write;
			if (doublebuffer) {
				frameptr[1] = dbuf2->write;
			} else {
				frameptr[1] = frameptr[0];
			}
		}
		if (scale) {
			switch (pixelsize) {
			case 1:	if (havedbuf) {
				do_scale8(vid.width, height,
					  frameptr[curframe], drawptr);
			} else {
				uint8 *buf = drawptr;
				for (i=0; i < height; i++) {
					do_scale8(vid.width, 1, oneline,buf);
					ggiPutBox(ggivis, 0, i*2, realwidth,
						  2, oneline);
					buf += vid.width;
				}
			}
			break;
			case 2: if (havedbuf) {
				do_scale16(vid.width, height,
					   frameptr[curframe], drawptr);
			} else {
				uint8 *buf = drawptr;
				for (i=0; i < height; i++) {
					do_scale16(vid.width, 1,
						   oneline, buf);
					ggiPutBox(ggivis, 0, i*2, realwidth,
						  2, oneline);
					buf += vid.width;
				}
			}
			break;
			case 4: if (havedbuf) {
				do_scale32(vid.width, height,
					   frameptr[curframe], drawptr);
			} else {
				uint8 *buf = drawptr;
				for (i=0; i < height; i++) {
					do_scale32(vid.width, 1,
						   oneline, buf);
					ggiPutBox(ggivis, 0, i*2, realwidth,
						  2, oneline);
					buf += vid.width;
				}
			}
			break;
			}
		} else if (palette) {
			switch (pixelsize) {
			case 1:	if (havedbuf) {
				do_copy8(vid.width, height,
					 frameptr[curframe], drawptr);
			} else {
				uint8 *buf = drawptr;
				for (i=0; i < height; i++) {
					do_copy8(vid.width, 1, oneline,buf);
					ggiPutBox(ggivis, 0, i, realwidth,
						  1, oneline);
					buf += vid.width;
				}
			}
			break;
			case 2: if (havedbuf) {
				do_copy16(vid.width, height,
					  frameptr[curframe], drawptr);
			} else {
				uint8 *buf = drawptr;
				for (i=0; i < height; i++) {
					do_copy16(vid.width, 1,
						  oneline, buf);
					ggiPutBox(ggivis, 0, i, realwidth,
						  1, oneline);
					buf += vid.width;
				}
			}
			break;
			case 4: if (havedbuf) {
				do_copy32(vid.width, height,
					  frameptr[curframe], drawptr);
			} else {
				uint8 *buf = drawptr;
				for (i=0; i < height; i++) {
					do_copy32(vid.width, 1,
						  oneline, buf);
					ggiPutBox(ggivis, 0, i, realwidth,
						  1, oneline);
					buf += vid.width;
				}
			}
			break;
			}
		} else {
			ggiPutBox(ggivis, 0, 0, vid.width, height,
				  drawptr);
		}
		if (havedbuf) {
			ggiResourceRelease(dbuf1->resource);
			if (doublebuffer) {
				ggiResourceRelease(dbuf2->resource);
			}
		}

	}

	if (doublebuffer) {
		ggiSetDisplayFrame(ggivis, curframe);
		curframe = !curframe;
		if (usedbuf) {
			vid.buffer = vid.conbuffer = vid.direct
				= drawptr = frameptr[curframe];
		}
		ggiSetWriteFrame(ggivis, curframe);
	}

#if 0
	if (GT_SIZE(mode.graphtype) == 16) {
		do_copy16(vid.width, height,
			  (uint16*)frameptr, drawptr);
	} else if (GT_SIZE(mode.graphtype) == 32) {
		do_copy32(vid.width, height,
			  (uint32*)frameptr, drawptr);
	}
#endif

	ggiFlush(ggivis);
}

void D_BeginDirectRect(int x, int y, byte *pbitmap, int width, int height)
{
// direct drawing of the "accessing disk" icon isn't supported under Linux
}

void D_EndDirectRect (int x, int y, int width, int height)
{
// direct drawing of the "accessing disk" icon isn't supported under Linux
}


/*
***************************************************************************
  Input handling
***************************************************************************
*/

static int XLateKey(ggi_key_event *ev)
{
	int key = 0;

	if (GII_KTYP(ev->label) == GII_KT_DEAD) {
		ev->label = GII_KVAL(ev->label);
	}
	switch(ev->label) {
	case GIIK_P9:
	case GIIK_PageUp:	key = K_PGUP; break;

	case GIIK_P3:
	case GIIK_PageDown:	key = K_PGDN; break;

	case GIIK_P7:
	case GIIK_Home:		key = K_HOME; break;

	case GIIK_P1:
	case GIIK_End:		key = K_END; break;

	case GIIK_P4:
	case GIIK_Left:		key = K_LEFTARROW; break;

	case GIIK_P6:
	case GIIK_Right:	key = K_RIGHTARROW; break;

	case GIIK_P2:
	case GIIK_Down:		key = K_DOWNARROW; break;

	case GIIK_P8:
	case GIIK_Up:		key = K_UPARROW; break;

	case GIIUC_Escape:	key = K_ESCAPE;	break;

	case GIIK_PEnter:
	case GIIUC_Return:	key = K_ENTER; break;

	case GIIUC_Tab:		key = K_TAB; break;

	case GIIK_F1:		key = K_F1; break;
	case GIIK_F2:		key = K_F2; break;
	case GIIK_F3:		key = K_F3; break;
	case GIIK_F4:		key = K_F4; break;
	case GIIK_F5:		key = K_F5; break;
	case GIIK_F6:		key = K_F6; break;
	case GIIK_F7:		key = K_F7; break;
	case GIIK_F8:		key = K_F8; break;
	case GIIK_F9:		key = K_F9; break;
	case GIIK_F10:		key = K_F10; break;
	case GIIK_F11:		key = K_F11; break;
	case GIIK_F12:		key = K_F12; break;

	case GIIUC_BackSpace:	key = K_BACKSPACE; break;

	case GIIK_PSeparator:
	case GIIK_PDecimal:
	case GIIUC_Delete:	key = K_DEL; break;

	case GIIK_Pause:	key = K_PAUSE; break;

	case GIIK_ShiftL:
	case GIIK_ShiftR:	key = K_SHIFT; break;

	case GIIK_Execute: 
	case GIIK_CtrlL: 
	case GIIK_CtrlR:	key = K_CTRL; break;

	case GIIK_AltL:	
	case GIIK_MetaL: 
	case GIIK_AltR:	
	case GIIK_MetaR:
	case GIIK_AltGr:
	case GIIK_ModeSwitch:	key = K_ALT; break;

	case GIIK_P5:
	case GIIK_PBegin:	key = K_AUX32; break;

	case GIIK_Insert:
	case GIIK_P0:		key = K_INS; break;

	case GIIK_PStar:	key = '*'; break;
	case GIIK_PPlus:	key = '+'; break;
	case GIIK_PMinus:	key = '-'; break;
	case GIIK_PSlash:	key = '/'; break;

	case GIIUC_Comma: case GIIUC_Minus: case GIIUC_Period:
		key = ev->label;
		break;
	case GIIUC_Section:	key = '~'; break;

	default:
		if (ev->label >= 0 && ev->label <= 9) return ev->label;
		if (ev->label >= 'A' && ev->label <= 'Z') {
			return ev->label - 'A' + 'a';
		}
		if (ev->label >= 'a' && ev->label <= 'z') return ev->label;

		if (ev->sym <= 0x7f) {
			key = ev->sym;
			if (key >= 'A' && key <= 'Z') {
				key = key - 'A' + 'a';
			}
			return key;
		}
		if (ev->label <= 0x7f) {
			return ev->label;
		}
		break;
	}

	return key;
}

static void GetEvent(void)
{
	ggi_event ev;
	uint32 b;
   
	ggiEventRead(ggivis, &ev, emAll);
	switch(ev.any.type) {
	case evKeyPress:
		Key_Event(XLateKey(&ev.key), true);
		break;

	case evKeyRelease:
		Key_Event(XLateKey(&ev.key), false);
		break;

	case evPtrRelative:
		mouse_x += (float) ev.pmove.x;
		mouse_y += (float) ev.pmove.y;
		break;

	case evPtrAbsolute:
		mouse_x += (float) (ev.pmove.x-p_mouse_x);
		mouse_y += (float) (ev.pmove.y-p_mouse_y);
		p_mouse_x = ev.pmove.x;
		p_mouse_y = ev.pmove.y;
		break;

	case evPtrButtonPress:
		if (!mouse_avail) return;

		b = ev.pbutton.button - 1;

		if (b < NUM_STDBUTTONS) {
			Key_Event(K_MOUSE1 + b, true);
		} else if (b < NUM_BUTTONS) {
			Key_Event(K_AUX32 - NUM_BUTTONS + b, true);
		}
		break;

	case evPtrButtonRelease:
		if (!mouse_avail) return;

		b = ev.pbutton.button - 1;

		if (b < NUM_STDBUTTONS) {
			Key_Event(K_MOUSE1 + b, false);
		} else if (b < NUM_BUTTONS) {
			Key_Event(K_AUX32 - NUM_BUTTONS + b, false);
		}
		break;

#if 0	
	case ConfigureNotify:
//printf("config notify\n");
		config_notify_width = ev.xconfigure.width;
		config_notify_height = ev.xconfigure.height;
		config_notify = 1;
		break;

	default:
#endif
	}
}


void Sys_SendKeyEvents(void)
{
	/* Get events from LibGGI */
	if (ggivis) {
		struct timeval t = {0,0};

		if (ggiEventPoll(ggivis, emAll, &t)) {
			int i = ggiEventsQueued(ggivis, emAll);
			while (i--) GetEvent();
		}
	}
}


void IN_Init(void)
{
	Cvar_RegisterVariable(&m_filter);
	if (COM_CheckParm ("-nomouse")) return;

	mouse_x = mouse_y = 0.0;
	mouse_avail = 1;
}

void IN_Shutdown(void)
{
	mouse_avail = 0;
}

void IN_Commands(void)
{
	/* Mouse buttons are sent from GetEvent() */
}

void IN_Move(usercmd_t *cmd)
{
	if (!mouse_avail) return;
   
	if (m_filter.value) {
		mouse_x = (mouse_x + old_mouse_x) * 0.5;
		mouse_y = (mouse_y + old_mouse_y) * 0.5;
	}

	old_mouse_x = mouse_x;
	old_mouse_y = mouse_y;
   
	mouse_x *= sensitivity.value;
	mouse_y *= sensitivity.value;
   
	if ( (in_strafe.state & 1) || (lookstrafe.value && (in_mlook.state & 1) ))
		cmd->sidemove += m_side.value * mouse_x;
	else
		cl.viewangles[YAW] -= m_yaw.value * mouse_x;
	if (in_mlook.state & 1)
		V_StopPitchDrift ();
   
	if ( (in_mlook.state & 1) && !(in_strafe.state & 1)) {
		cl.viewangles[PITCH] += m_pitch.value * mouse_y;
		if (cl.viewangles[PITCH] > 80)
			cl.viewangles[PITCH] = 80;
		if (cl.viewangles[PITCH] < -70)
			cl.viewangles[PITCH] = -70;
	} else {
		if ((in_strafe.state & 1) && noclip_anglehack)
			cmd->upmove -= m_forward.value * mouse_y;
		else
			cmd->forwardmove -= m_forward.value * mouse_y;
	}
	mouse_x = mouse_y = 0.0;
}

void VID_ExtraOptionDraw(unsigned int options_draw_cursor)
{
	// Windowed Mouse
        M_Print (16, options_draw_cursor+=8, "             Use Mouse");
        M_DrawCheckbox (220, options_draw_cursor, _windowed_mouse.value);
}

void VID_ExtraOptionCmd(int option_cursor)
{
	switch(option_cursor) {
	case 1:	// _windowed_mouse
		Cvar_SetValue ("_windowed_mouse", !_windowed_mouse.value);
		break;

	}
}
