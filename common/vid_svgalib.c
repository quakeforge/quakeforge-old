/*
vid_svgalib.c - Linux SVGALib video routines
Copyright (C) 1996-1997  Id Software, Inc.
Copyright (C) 1999-2000  contributors of the QuakeForge project
Please see the file "AUTHORS" for a list of contributors
Copyright (C) 1999-2000  Nelson Rush.
Copyright (C) 1999-2000  Marcus Sundberg [mackan@stacken.kth.se]
Copyright (C) 1999-2000  XoXus [xoxus@usa.net]

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
#include <common.h>
#include <d_local.h>
#include <sound.h>
#include <cvar.h>
#include <lib_replace.h>
#include <cmd.h>
#include <sys.h>
#include <console.h>
#include <input.h>

#include <stdio.h>
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif
#if defined(HAVE_SYS_IO_H)
# include <sys/io.h>
#elif defined(HAVE_ASM_IO_H)
# include <asm/io.h>
#endif

#include <vga.h>


void VGA_UpdatePlanarScreen (void *srcbuffer);


unsigned short	d_8to16table[256];
static byte	*vid_surfcache;
static int	VID_highhunkmark;

static int		num_modes, current_mode;
static vga_modeinfo	*modes;

static byte vid_current_palette[768];

static int	svgalib_inited=0;
static int	UseDisplay = 1;

//static cvar_t	vid_mode = {"vid_mode","5",false};
static cvar_t	*vid_mode;
// static cvar_t	vid_redrawfull = {"vid_redrawfull","0",false};
static cvar_t	*vid_redrawfull;
//static cvar_t	vid_waitforrefresh = {"vid_waitforrefresh","0",true};
static cvar_t	*vid_waitforrefresh;
 
static char	*framebuffer_ptr;


static byte     backingbuf[48*24];

int	VGA_width, VGA_height, VGA_rowbytes, VGA_bufferrowbytes, VGA_planar;
byte	*VGA_pagebase;

int	VID_options_items = 0;


void
D_BeginDirectRect (int x, int y, byte *pbitmap, int width, int height)
{
	int i, j, k, plane, reps, repshift, offset, vidpage, off;

	if (!svgalib_inited || !vid.direct || !vga_oktowrite()) return;

	if (vid.aspect > 1.5) {
		reps = 2;
		repshift = 1;
	} else {
		reps = 1;
		repshift = 0;
	}

	vidpage = 0;
	vga_setpage(0);

	if (VGA_planar) {
		for (plane=0 ; plane<4 ; plane++) {
			/* Select the correct plane for reading and writing */
			outb(0x02, 0x3C4);
			outb(1 << plane, 0x3C5);
			outb(4, 0x3CE);
			outb(plane, 0x3CF);

			for (i=0 ; i<(height << repshift) ; i += reps) {
				for (k=0 ; k<reps ; k++) {
					for (j=0 ; j<(width >> 2) ; j++) {
						backingbuf[(i + k) * 24 + (j << 2) + plane] =
							vid.direct[(y + i + k) * VGA_rowbytes +
								(x >> 2) + j];
						vid.direct[(y + i + k) * VGA_rowbytes + (x>>2) + j] =
							pbitmap[(i >> repshift) * 24 +
							       (j << 2) + plane];
					}
				}
			}
		}
	} else {
		for (i=0 ; i<(height << repshift) ; i += reps) {
			for (j=0 ; j<reps ; j++) {
				offset = x + ((y << repshift) + i + j)
					* vid.rowbytes;
				off = offset % 0x10000;
				if ((offset / 0x10000) != vidpage) {
					vidpage=offset / 0x10000;
					vga_setpage(vidpage);
				}
				memcpy (&backingbuf[(i + j) * 24],
					vid.direct + off, width);
				memcpy (vid.direct + off,
					&pbitmap[(i >> repshift)*width],
					width);
			}
		}
	}
}


void
D_EndDirectRect (int x, int y, int width, int height)
{
	int i, j, k, plane, reps, repshift, offset, vidpage, off;

	if (!svgalib_inited || !vid.direct || !vga_oktowrite()) return;

	if (vid.aspect > 1.5) {
		reps = 2;
		repshift = 1;
	} else {
		reps = 1;
		repshift = 0;
	}

	vidpage = 0;
	vga_setpage(0);

	if (VGA_planar) {
		for (plane=0 ; plane<4 ; plane++) {
			/* Select the correct plane for writing */
			outb(2, 0x3C4);
			outb(1 << plane, 0x3C5);
			outb(4, 0x3CE);
			outb(plane, 0x3CF);

			for (i=0 ; i<(height << repshift) ; i += reps) {
				for (k=0 ; k<reps ; k++) {
					for (j=0 ; j<(width >> 2) ; j++) {
						vid.direct[(y + i + k) * VGA_rowbytes + (x>>2) + j] =
							backingbuf[(i + k) * 24 + (j << 2) + plane];
					}
				}
			}
		}
	} else {
		for (i=0 ; i<(height << repshift) ; i += reps) {
			for (j=0 ; j<reps ; j++) {
				offset = x + ((y << repshift) + i + j)
					* vid.rowbytes;
				off = offset % 0x10000;
				if ((offset / 0x10000) != vidpage) {
					vidpage=offset / 0x10000;
					vga_setpage(vidpage);
				}
				memcpy (vid.direct + off, 
					&backingbuf[(i +j)*24],	width);
			}
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
	int modenum;
	
	modenum = Q_atoi (Cmd_Argv(1));
	if ((modenum >= num_modes) || (modenum < 0 ) ||
	    !modes[modenum].width) {
		Con_Printf("Invalid video mode: %d!\n", modenum);
	}
	Con_Printf("%d: %d x %d - ", modenum,
		   modes[modenum].width, modes[modenum].height);
	if (modes[modenum].bytesperpixel == 0) {
		Con_Printf("ModeX\n");
	} else {
		Con_Printf("%d bpp\n", modes[modenum].bytesperpixel<<3);
	}
}


static void
VID_DescribeModes_f(void)
{
	int i;
	
	for (i=0;i<num_modes;i++) {
		if (modes[i].width) {
			Con_Printf("%d: %d x %d - ", i, modes[i].width,modes[i].height);
			if (modes[i].bytesperpixel == 0)
				Con_Printf("ModeX\n");
			else
				Con_Printf("%d bpp\n", modes[i].bytesperpixel<<3);
		}
	}
}


/*
================
VID_NumModes
================
*/
static int
VID_NumModes(void)
{
	int i,i1=0;
	
	for (i=0; i < num_modes; i++) {
		i1 += modes[i].width ? 1 : 0;
	}
	return (i1);
}


static void
VID_NumModes_f(void)
{
	Con_Printf("%d modes\n", VID_NumModes());
}


static void
VID_Debug_f (void)
{
	Con_Printf("mode: %d\n",current_mode);
	Con_Printf("height x width: %d x %d\n",vid.height,vid.width);
	Con_Printf("bpp: %d\n",modes[current_mode].bytesperpixel*8);
	Con_Printf("vid.aspect: %f\n",vid.aspect);
}


static void
VID_InitModes(void)
{
	int i;

	/* Get complete information on all modes */
	num_modes = vga_lastmodenumber()+1;
	modes = Z_Malloc(num_modes * sizeof(vga_modeinfo));
	for (i=0 ; i<num_modes ; i++) {
		if (vga_hasmode(i)) {
			Q_memcpy(&modes[i], vga_getmodeinfo(i),
				 sizeof (vga_modeinfo));
		} else {
			modes[i].width = 0; // means not available
		}
	}

	/* Filter for modes we don't support */
	for (i=0 ; i<num_modes ; i++) {
		if (modes[i].bytesperpixel != 1 && modes[i].colors != 256) {
			modes[i].width = 0;
		}
	}
}


static int
get_mode(char *name, int width, int height, int depth)
{
	int i, ok, match;

	match = (!!width) + (!!height)*2 + (!!depth)*4;

	if (name) {
		i = vga_getmodenumber(name);
		if (!modes[i].width) {
			Sys_Printf("Mode [%s] not supported\n", name);
			i = G320x200x256;
		}
	} else {
		for (i=0 ; i<num_modes ; i++) {
			if (modes[i].width) {
				ok = (modes[i].width == width)
					+ (modes[i].height == height)*2
					+ (modes[i].bytesperpixel == depth/8)*4;
				if ((ok & match) == ok)
					break;
			}
		}
		if (i==num_modes) {
			Sys_Printf("Mode %dx%d (%d bits) not supported\n",
				   width, height, depth);
			i = G320x200x256;
		}
	}

	return i;
}


void
VID_Shutdown(void)
{

	if (!svgalib_inited) return;

	if (UseDisplay) {
		vga_setmode(TEXT);
	}
	svgalib_inited = 0;
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

	if (!svgalib_inited) return;

	memcpy(vid_current_palette, palette, sizeof(vid_current_palette));

	if (vga_getcolors() == 256) {
		tp = tmppal;
		for (i=256*3 ; i ; i--) {
			*(tp++) = *(palette++) >> 2;
		}

		if (UseDisplay && vga_oktowrite()) {
			vga_setpalvec(0, 256, tmppal);
		}
	}
}


int
VID_SetMode(int modenum, unsigned char *palette)
{
	int bsize, zsize, tsize;
	int err;

	if ((modenum >= num_modes) || (modenum < 0) || !modes[modenum].width){
		vid_mode->value = (float)current_mode;

		Con_Printf("No such video mode: %d\n",modenum);

		return 0;
	}

	vid_mode->value = (float)modenum;

	current_mode=modenum;

	vid.width = modes[current_mode].width;
	vid.height = modes[current_mode].height;

	VGA_width = modes[current_mode].width;
	VGA_height = modes[current_mode].height;
	VGA_planar = modes[current_mode].bytesperpixel == 0;
	VGA_rowbytes = modes[current_mode].linewidth;
	vid.rowbytes = modes[current_mode].linewidth;
	if (VGA_planar) {
		VGA_bufferrowbytes = modes[current_mode].linewidth * 4;
		vid.rowbytes = modes[current_mode].linewidth*4;
	}

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

	/* get goin' */
	/* XoXus: need to check return values! */
	err = vga_setmode(current_mode);
	if (err) {
		Sys_Error("Video mode failed: %d\n",modenum);
	}
	VID_SetPalette(palette);

	VGA_pagebase = vid.direct = framebuffer_ptr = (char *) vga_getgraphmem();
#if 0
	if (vga_setlinearaddressing() > 0) {
		framebuffer_ptr = (char *) vga_getgraphmem();
	}
#endif
	if (!framebuffer_ptr) {
		Sys_Error("This mode isn't hapnin'\n");
	}

	vga_setpage(0);

	svgalib_inited = 1;

	/* Force a surface cache flush */
	vid.recalc_refdef = 1;

	return 1;
}


void
VID_Init(unsigned char *palette)
{
	int w, h, d;
	int err;

	//plugin_load("in_svgalib.so");
	/* Sound gets initialized here */
	S_Init();
	
	if (svgalib_inited) return;

#if 0
	Cmd_AddCommand ("gamma", VID_Gamma_f);
#endif

	if (UseDisplay) {
		/* XoXus: return values need to be checked! */
		err = vga_init();
		if (err)
			Sys_Error("SVGALib failed to allocate a new VC\n");

		VID_InitModes();

//		Cvar_RegisterVariable (&vid_mode);
		vid_mode = Cvar_Get ("vid_mode","5",0,"None");
//		Cvar_RegisterVariable (&vid_redrawfull);
		vid_redrawfull = Cvar_Get ("vid_redrawfull","0",0,"None");
//		Cvar_RegisterVariable (&vid_waitforrefresh);
		vid_waitforrefresh = Cvar_Get ("vid_waitforrefresh","0",
						CVAR_ARCHIVE,"None");
		
		Cmd_AddCommand("vid_nummodes", VID_NumModes_f);
		Cmd_AddCommand("vid_describemode", VID_DescribeMode_f);
		Cmd_AddCommand("vid_describemodes", VID_DescribeModes_f);
		Cmd_AddCommand("vid_debug", VID_Debug_f);

		/* Interpret command-line params */
		w = h = d = 0;
		if (getenv("GSVGAMODE")) {
			current_mode = get_mode(getenv("GSVGAMODE"), w, h, d);
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
			current_mode = G320x200x256;
		}

		/* Set vid parameters */
		VID_SetMode(current_mode, palette);

		VID_SetPalette(palette);

		/* We do want to run in the background when switched away */
		/* XoXus: Running in background is just plain bad... */
		vga_runinbackground(0);
	}
	IN_Init();
}


void 
VID_Update(vrect_t *rects)
{
	if (!svgalib_inited) return;

	if (!vga_oktowrite()) {
		/* Can't update screen if it's not active */
		return;
	}

	if (vid_waitforrefresh->value) {
		vga_waitretrace();
	}

	if (VGA_planar) {
		VGA_UpdatePlanarScreen(vid.buffer);
	} else if (vid_redrawfull->value) {
		int total = vid.rowbytes * vid.height;
		int offset;

		for (offset=0;offset<total;offset+=0x10000) {
			vga_setpage(offset/0x10000);
			memcpy(framebuffer_ptr, vid.buffer + offset,
			       ((total-offset>0x10000)
				? 0x10000 : (total-offset)));
		}
	} else {
		int ycount;
		int offset;
		int vidpage=0;

		vga_setpage(0);

		while (rects) {
			ycount = rects->height;
			offset = rects->y * vid.rowbytes + rects->x;
			while (ycount--) {
				register int i = offset % 0x10000;
	
				if ((offset / 0x10000) != vidpage) {
					vidpage=offset / 0x10000;
					vga_setpage(vidpage);
				}
				if (rects->width + i > 0x10000) {
					memcpy(framebuffer_ptr + i, 
							vid.buffer + offset, 
							0x10000 - i);
					vga_setpage(++vidpage);
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
	static char *badmodestr = "Bad mode number";
	static char  modestr[40];

	if (modenum == 0) {
		snprintf(modestr, sizeof(modestr), "%d x %d, %d bpp",
			 vid.width, vid.height, modes[current_mode].bytesperpixel*8);
		return (modestr);
	} else {
		return (badmodestr);
	}
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
	// It may not look like it, but this is important
}
