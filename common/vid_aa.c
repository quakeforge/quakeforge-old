/*
Copyright (C) 1996-1997  Id Software, Inc.
Copyright (C) 1999-2000  contributors of the QuakeForge project
Copyright (C) 1999-2000  Nelson Rush.

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

#include <quakedef.h>
#include <d_local.h>

#include <stdio.h>
#include <stdlib.h>
#include <aalib.h>

#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif
#if defined(HAVE_SYS_IO_H)
# include <sys/io.h>
#elif defined(HAVE_ASM_IO_H)
# include <asm/io.h>
#endif

static byte	*vid_surfcache;
static int	VID_highhunkmark;

static int		num_modes, current_mode;
static vga_modeinfo	*modes;

static byte vid_current_palette[768];

static int	UseDisplay = 1;

static cvar_t	*vid_mode;
static cvar_t	*vid_redrawfull;
static cvar_t	*vid_waitforrefresh;

static char	*framebuffer_ptr;

static byte     backingbuf[48*24];

int	VID_options_items = 0;

aa_context *context = NULL;
unsigned char *buffer;
aa_palette palette;
aa_renderparams *params;
int aaversion=0;

void
aa_setpage()
{
	int x,y;
	unsigned char *img = aa_image(context);

	switch(aaversion) {
		case 11:
			for (y = 0; y < 200; y++)
			memcpy(img+y*aa_imgwidth(context),buffer+y*320,320);
			break;
		case 12:
			for (y=0;y<100;y++)
			memcpy(img+y*aa_imgwidth(context),buffer+y*320*2,320);
			break;
		case 13:
			for (y=0;y<66;y++)
			memcpy(img+y*aa_imgwidth(context),buffer+y*320*3,320);
                        break;
		case 14:
			for (y=0;y<50;y++)
			memcpy(img+y*aa_imgwidth(context),buffer+y*320*4,320);
                        break;
		case 21:
			for (y = 0; y < 200; y++)
				for (x=0;x<160;x++)
				img[y*aa_imgwidth(context)+x]=buffer[y*320+x*2];
                        break;
		case 22:
			for (y=0;y<100;y++)
                                for (x=0;x<160;x++)
				img[y*aa_imgwidth(context)+x]=buffer[y*2*320+x*2];
                        break;
		case 23:
			for (y=0;y<66;y++)
                                for (x=0;x<160;x++)
				img[y*aa_imgwidth(context)+x]=buffer[y*3*320+x*2];
                        break;
		case 24:
			for (y=0;y<50;y++)
                                for (x=0;x<160;x++)
				img[y*aa_imgwidth(context)+x]=buffer[y*4*320+x*2];
                        break;
		default:
			fprintf(stderr,"version = %d, unknown version!\n",aaversion);
			exit(-1);
			break;
	}

	aa_renderpalette(context, palette, params, 0, 0,
			aa_scrwidth(context), aa_scrheight(context));
	aa_flush(context);


}

void
D_BeginDirectRect (int x, int y, byte *pbitmap, int width, int height)
{
	int i, j, k, plane, reps, repshift, offset, vidpage, off;

	if (vid.aspect > 1.5) {
		reps = 2;
		repshift = 1;
	} else {
		reps = 1;
		repshift = 0;
	}

	vidpage = 0;
	aa_setpage();

		for (i=0 ; i<(height << repshift) ; i += reps) {
			for (j=0 ; j<reps ; j++) {
				offset = x + ((y << repshift) + i + j)
					* vid.rowbytes;
				off = offset % 0x10000;
				if ((offset / 0x10000) != vidpage) {
					vidpage=offset / 0x10000;
					aa_setpage();
				}
				memcpy (&backingbuf[(i + j) * 24],
					vid.direct + off, width);
				memcpy (vid.direct + off,
					&pbitmap[(i >> repshift)*width],
					width);
			}
		}
}


void
D_EndDirectRect (int x, int y, int width, int height)
{
	int i, j, k, plane, reps, repshift, offset, vidpage, off;

	if (vid.aspect > 1.5) {
		reps = 2;
		repshift = 1;
	} else {
		reps = 1;
		repshift = 0;
	}

	vidpage = 0;
	aa_setpage();

		for (i=0 ; i<(height << repshift) ; i += reps) {
			for (j=0 ; j<reps ; j++) {
				offset = x + ((y << repshift) + i + j)
					* vid.rowbytes;
				off = offset % 0x10000;
				if ((offset / 0x10000) != vidpage) {
					vidpage=offset / 0x10000;
					aa_setpage();
				}
				memcpy (vid.direct + off,
					&backingbuf[(i +j)*24],	width);
			}
		}
}


#if 0
static void
VID_Gamma_f(void)
{
	float	gamma, f, inf;
	unsigned char	palette[768];
	int		i;

	if (Cmd_Argc () == 2) {
		gamma = Q_atof (Cmd_Argv(1));

		for (i=0 ; i<768 ; i++)	{
			f = pow ( (host_basepal[i]+1)/256.0 , gamma );
			inf = f*255 + 0.5;
			if (inf < 0) inf = 0;
			if (inf > 255) inf = 255;
			palette[i] = inf;
		}

		VID_SetPalette (palette);

		/* Force a surface cache flush */
		vid.recalc_refdef = 1;
	}
}
#endif


static void
VID_DescribeMode_f(void)
{
}


static void
VID_DescribeModes_f(void)
{
}


/*
================
VID_NumModes
================
*/
static int
VID_NumModes(void)
{ return 1;
}


static void
VID_NumModes_f(void)
{
	Con_Printf("%d modes\n", VID_NumModes());
}


static void
VID_Debug_f (void)
{
/*	Con_Printf("mode: %d\n",current_mode);
	Con_Printf("height x width: %d x %d\n",vid.height,vid.width);
	Con_Printf("bpp: %d\n",modes[current_mode].bytesperpixel*8);
	Con_Printf("vid.aspect: %f\n",vid.aspect);
*/
}


void
VID_Shutdown(void)
{

	aa_close(context);
	free(buffer);
}


void
VID_ShiftPalette(unsigned char *p)
{
	VID_SetPalette(p);
}


void
VID_SetPalette(byte *palette)
{
	static int tmppal[256*3];
	int *tp;
	int i;

	memcpy(vid_current_palette, palette, sizeof(vid_current_palette));

	tp = tmppal;
	for (i=256*3 ; i ; i--) {
		*(tp++) = *(palette++) >> 2;
	}

	if (UseDisplay) {
		vga_setpalvec(0, 256, tmppal);
	}

}


int
VID_SetMode(int modenum, unsigned char *palette)
{
	int bsize, zsize, tsize;
	int err;

	vid.aspect = ((float)vid.height / (float)vid.width) * (320.0 / 240.0);
	vid.colormap = (pixel_t *) host_colormap;
	vid.fullbright = 256 - LittleLong (*((int *)vid.colormap + 2048));
	vid.conrowbytes = vid.rowbytes;
	vid.conwidth = vid.width;
	vid.conheight = vid.height;
	vid.numpages = 1;

	vid.maxwarpwidth = WARP_WIDTH;
	vid.maxwarpheight = WARP_HEIGHT;

	/* alloc zbuffer and surface cache */
	if (d_pzbuffer) {
		D_FlushCaches();
		Hunk_FreeToHighMark (VID_highhunkmark);
		d_pzbuffer = NULL;
		vid_surfcache = NULL;
	}

	bsize = vid.rowbytes * vid.height;
	tsize = D_SurfaceCacheForRes (vid.width, vid.height);
	zsize = vid.width * vid.height * sizeof(*d_pzbuffer);

	VID_highhunkmark = Hunk_HighMark ();

	d_pzbuffer = Hunk_HighAllocName (bsize+tsize+zsize, "video");

	vid_surfcache = ((byte *)d_pzbuffer) + zsize;

	vid.conbuffer = vid.buffer = (pixel_t *)(((byte *)d_pzbuffer) + zsize + tsize);

	D_InitCaches (vid_surfcache, tsize);

	if (!context)
		if (context = aa_autoinit(&aa_defparams)) {
			if (buffer = malloc(64000)) {
				params = aa_getrenderparams();
				params->bright = 100;
				params->contrast = 100;
				memset(aa_image(context), 0,
				aa_imgwidth(context) * aa_imgheight(context));
				fprintf(stderr,"%d,%d\n",
					aa_imgwidth(context),
					aa_imgheight(context));
				if (aa_imgwidth(context)>=320)
					aaversion=10;
				else
					aaversion=20;
				if (aa_imgheight(context)>=200)
					aaversion+=1;
				else if (aa_imgheight(context)>=100)
					aaversion+=2;
				else if (aa_imgheight(context)>=66)
					aaversion+=3;
				else
					aaversion+=4
			} else
				exit((perror("malloc"),-1));
		} else {
			fprintf(stderr, "set video failed!\n", x);
			exit(-1);
			}

	VID_SetPalette(palette);

	aa_setpage();

	/* Force a surface cache flush */
	vid.recalc_refdef = 1;

	return 1;
}


void
VID_Init(unsigned char *palette)
{
	int w, h, d;
	int err;

#if 0
	Cmd_AddCommand ("gamma", VID_Gamma_f);
#endif

	if (UseDisplay) {

		vid_mode = Cvar_Get ("vid_mode","5",0,"None");
		vid_redrawfull = Cvar_Get ("vid_redrawfull","0",0,"None");
		vid_waitforrefresh = Cvar_Get ("vid_waitforrefresh","0",
						CVAR_ARCHIVE,"None");

		Cmd_AddCommand("vid_nummodes", VID_NumModes_f);
		Cmd_AddCommand("vid_describemode", VID_DescribeMode_f);
		Cmd_AddCommand("vid_describemodes", VID_DescribeModes_f);
		Cmd_AddCommand("vid_debug", VID_Debug_f);

		/* Interpret command-line params */
		w = h = d = 0;
		if (getenv("GSVGAMODE")) {
			current_mode = get_mode(getenv("AAMODE"), w, h, d);
		} else if (COM_CheckParm("-mode")) {
			current_mode = get_mode(com_argv[COM_CheckParm("-mode")+1], w, h, d);
		} else if (COM_CheckParm("-w") || COM_CheckParm("-h")
			   || COM_CheckParm("-d"))  {
			if (COM_CheckParm("-w")) {
				w = Q_atoi(com_argv[COM_CheckParm("-w")+1]);
			}
			if (COM_CheckParm("-h")) {
				h = Q_atoi(com_argv[COM_CheckParm("-h")+1]);
			}
			if (COM_CheckParm("-d")) {
				d = Q_atoi(com_argv[COM_CheckParm("-d")+1]);
			}
			current_mode = get_mode(0, w, h, d);
		} else {
			current_mode = 5;
		}

		/* Set vid parameters */
		VID_SetMode(current_mode, palette);

		VID_SetPalette(palette);
	}
}


void
VID_Update(vrect_t *rects)
{
		int ycount;
		int offset;
		int vidpage=0;

		vga_setpage();

		while (rects) {
			ycount = rects->height;
			offset = rects->y * vid.rowbytes + rects->x;
			while (ycount--) {
				register int i = offset % 0x10000;

				if ((offset / 0x10000) != vidpage) {
					vidpage=offset / 0x10000;
					aa_setpage();
				}
				if (rects->width + i > 0x10000) {
					memcpy(framebuffer_ptr + i,
							vid.buffer + offset,
							0x10000 - i);
					aa_setpage();
					memcpy(framebuffer_ptr,
							vid.buffer + offset + 0x10000 - i,
							rects->width - 0x10000 + i);
				} else {
					memcpy(framebuffer_ptr + i,
					       vid.buffer + offset,
					       rects->width);
				}
				offset += vid.rowbytes;
			}
			rects = rects->pnext;
		}
	}

	if (vid_mode->value != current_mode) {
		VID_SetMode ((int)vid_mode->value, vid_current_palette);
	}
}


static int dither = 0;

void
VID_DitherOn(void)
{
	if (dither == 0) {
#if 0
		R_ViewChanged (&vrect, sb_lines, vid.aspect);
#endif
		dither = 1;
	}
}


void
VID_DitherOff(void)
{
	if (dither) {
#if 0
		R_ViewChanged (&vrect, sb_lines, vid.aspect);
#endif
		dither = 0;
	}
}


/*
================
VID_ModeInfo
================
*/
char *
VID_ModeInfo (int modenum)
{
/*	static char *badmodestr = "Bad mode number";
	static char  modestr[40];

	if (modenum == 0) {
		snprintf(modestr, sizeof(modestr), "%d x %d, %d bpp",
			 vid.width, vid.height, modes[current_mode].bytesperpixel*8);
		return (modestr);
	} else {
		return (badmodestr);
	}
*/
}


void
VID_ExtraOptionDraw(unsigned int options_draw_cursor)
{
	/* No extra option menu items yet */
}


void
VID_ExtraOptionCmd(int option_cursor)
{
#if 0
        switch(option_cursor) {
        case 1:  // Always start with 1
		break;
        }
#endif
}

void VID_InitCvars ()
{
	// It may not look it, but this is important
}
