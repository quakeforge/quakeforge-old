/*
Copyright (C) 1996-1997  Id Software, Inc.
Copyright (C) 1999-2000  contributors of the QuakeForge project
Copyright (C) 1999-2000  Nelson Rush.
Copyright (C) 2000       Marcus Sundberg [mackan@stacken.kth.se]
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

#include <qtypes.h>
#include <quakedef.h>
#include <glquake.h>
#include <cvar.h>
#include <console.h>
#include <sound.h>
#include <keys.h>
#include <menu.h>
#include <sys.h>
#include <lib_replace.h>
#include <draw.h>

#ifndef _EXPERIMENTAL_
# undef HAS_DGA
#endif

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <signal.h>

#ifdef HAVE_DLFCN_H
# include <dlfcn.h>
#endif
#ifndef RTLD_LAZY
# ifdef DL_LAZY
#  define RTLD_LAZY	DL_LAZY
# else
#  define RTLD_LAZY	0
# endif
#endif

#include <GL/glx.h>

#include <X11/keysym.h>
#include <X11/cursorfont.h>

#ifdef HAS_DGA
# include <X11/extensions/xf86dga.h>
# include <X11/extensions/xf86vmode.h>
#endif
#include <dga_check.h>

#ifdef XMESA
# include <GL/xmesa.h>
#endif

#define WARP_WIDTH              320
#define WARP_HEIGHT             200

Display			*x_disp = NULL;
static int		screen;
Window			x_win;
static GLXContext	ctx = NULL;

#define X_MASK (VisibilityChangeMask | StructureNotifyMask)

unsigned short	d_8to16table[256];
unsigned	d_8to24table[256];
unsigned char	d_15to8table[65536];

cvar_t	vid_mode = {"vid_mode", "0", CVAR_NONE};
cvar_t  vid_glx_fullscreen = {"vid_glx_fullscreen", "0", CVAR_NONE};

#ifdef HAS_DGA
static int	nummodes;
static XF86VidModeModeInfo **vidmodes;
static int	hasdgavideo = 0, hasvidmode = 0;
#endif

#ifdef HAVE_DLOPEN
static void	*dlhand = NULL;
#endif
static int	hasdga = 0;
static GLboolean (*QF_XMesaSetFXmode)(GLint mode) = NULL;


static int scr_width, scr_height;

#if defined(XMESA) || defined(HAS_DGA)
int VID_options_items = 2;
#else
int VID_options_items = 1;
#endif

/*-----------------------------------------------------------------------*/

//int		texture_mode = GL_NEAREST;
//int		texture_mode = GL_NEAREST_MIPMAP_NEAREST;
//int		texture_mode = GL_NEAREST_MIPMAP_LINEAR;
int		texture_mode = GL_LINEAR;
//int		texture_mode = GL_LINEAR_MIPMAP_NEAREST;
//int		texture_mode = GL_LINEAR_MIPMAP_LINEAR;

int		texture_extension_number = 1;

float		gldepthmin, gldepthmax;

cvar_t	gl_ztrick = {"gl_ztrick", "0", CVAR_ARCHIVE};

const char *gl_vendor;
const char *gl_renderer;
const char *gl_version;
const char *gl_extensions;

qboolean is8bit = false;
qboolean gl_mtexable = false;

/*-----------------------------------------------------------------------*/
void D_BeginDirectRect (int x, int y, byte *pbitmap, int width, int height)
{
}

void D_EndDirectRect (int x, int y, int width, int height)
{
}

void
VID_Shutdown(void)
{
	Con_Printf("VID_Shutdown\n");

	if (x_disp && ctx) {
		glXDestroyContext(x_disp, ctx);
		ctx = NULL;
	}
#ifdef HAS_DGA
	if (hasvidmode) {
		int i;

		for (i = 0; i < nummodes; i++) {
			if (vidmodes[i]->private) XFree(vidmodes[i]->private);
		}
		XFree(vidmodes);
	}
#endif
#ifdef HAVE_DLOPEN
	if (dlhand) {
		dlclose(dlhand);
		dlhand = NULL;
	}
#endif

	if (x_disp) {
		XCloseDisplay(x_disp);
		x_disp = NULL;
	}
}


static void
signal_handler(int sig)
{
	printf("Received signal %d, exiting...\n", sig);
	Sys_Quit();
	exit(sig);
}

static void
InitSig(void)
{
	signal(SIGHUP, signal_handler);
	signal(SIGINT, signal_handler);
	signal(SIGQUIT, signal_handler);
	signal(SIGILL, signal_handler);
	signal(SIGTRAP, signal_handler);
	signal(SIGIOT, signal_handler);
	signal(SIGBUS, signal_handler);
/*	signal(SIGFPE, signal_handler); */
	signal(SIGSEGV, signal_handler);
	signal(SIGTERM, signal_handler);
}

void VID_ShiftPalette(unsigned char *p)
{
#ifdef _EXPERIMENTAL_
	VID_SetPalette(p);
#endif
}

void	VID_SetPalette (unsigned char *palette)
{
	byte	*pal;
	unsigned r,g,b;
	unsigned v;
	int     r1,g1,b1;
	int		k;
	unsigned short i;
	unsigned	*table;
	QFile *f;
	char s[255];
	float dist, bestdist;
	static qboolean palflag = false;

//
// 8 8 8 encoding
//
//	Con_Printf("Converting 8to24\n");

	pal = palette;
	table = d_8to24table;
	for (i=0 ; i<256 ; i++)
	{
		r = pal[0];
		g = pal[1];
		b = pal[2];
		pal += 3;
		
//		v = (255<<24) + (r<<16) + (g<<8) + (b<<0);
//		v = (255<<0) + (r<<8) + (g<<16) + (b<<24);
		v = (255<<24) + (r<<0) + (g<<8) + (b<<16);
		*table++ = v;
	}
	d_8to24table[255] &= 0xffffff;	// 255 is transparent

	// JACK: 3D distance calcs - k is last closest, l is the distance.
	// FIXME: Precalculate this and cache to disk.
	if (palflag)
		return;
	palflag = true;

	COM_FOpenFile("glquake/15to8.pal", &f);
	if (f) {
		Qread(f, d_15to8table, 1<<15);
		Qclose(f);
	} else {
		for (i=0; i < (1<<15); i++) {
			/* Maps
 			000000000000000
 			000000000011111 = Red  = 0x1F
 			000001111100000 = Blue = 0x03E0
 			111110000000000 = Grn  = 0x7C00
 			*/
 			r = ((i & 0x1F) << 3)+4;
 			g = ((i & 0x03E0) >> 2)+4;
 			b = ((i & 0x7C00) >> 7)+4;
			pal = (unsigned char *)d_8to24table;
			for (v=0,k=0,bestdist=10000.0; v<256; v++,pal+=4) {
 				r1 = (int)r - (int)pal[0];
 				g1 = (int)g - (int)pal[1];
 				b1 = (int)b - (int)pal[2];
				dist = sqrt(((r1*r1)+(g1*g1)+(b1*b1)));
				if (dist < bestdist) {
					k=v;
					bestdist = dist;
				}
			}
			d_15to8table[i]=k;
		}
		snprintf(s, sizeof(s), "%s/glquake", com_gamedir);
 		Sys_mkdir (s);
		snprintf(s, sizeof(s), "%s/glquake/15to8.pal", com_gamedir);
		if ((f = Qopen(s, "wb")) != NULL) {
			Qwrite(f, d_15to8table, 1<<15);
			Qclose(f);
		}
	}
}


/*
===============
GL_Init
===============
*/
void GL_Init (void)
{
	gl_vendor = glGetString (GL_VENDOR);
	Con_Printf ("GL_VENDOR: %s\n", gl_vendor);
	gl_renderer = glGetString (GL_RENDERER);
	Con_Printf ("GL_RENDERER: %s\n", gl_renderer);

	gl_version = glGetString (GL_VERSION);
	Con_Printf ("GL_VERSION: %s\n", gl_version);
	gl_extensions = glGetString (GL_EXTENSIONS);
	Con_Printf ("GL_EXTENSIONS: %s\n", gl_extensions);

//	Con_Printf ("%s %s\n", gl_renderer, gl_version);

	glClearColor (0,0,0,0);
	glCullFace(GL_FRONT);
	glEnable(GL_TEXTURE_2D);

	glEnable(GL_ALPHA_TEST);
	glAlphaFunc(GL_GREATER, 0.666);

	glPolygonMode (GL_FRONT_AND_BACK, GL_FILL);

/* glShadeMode(GL_SMOOTH) should look better then GL_FLAT but
   I don't know if it looks any better, sure is slower
	glShadeModel (GL_SMOOTH);
*/
	glShadeModel (GL_FLAT);

	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

//	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
}

/*
=================
GL_BeginRendering

=================
*/
void GL_BeginRendering (int *x, int *y, int *width, int *height)
{
	*x = *y = 0;
	*width = scr_width;
	*height = scr_height;

//    if (!wglMakeCurrent( maindc, baseRC ))
//		Sys_Error ("wglMakeCurrent failed");

//	glViewport (*x, *y, *width, *height);
}


void GL_EndRendering (void)
{
	glFlush();
	glXSwapBuffers(x_disp, x_win);
}

qboolean VID_Is8bit(void)
{
	return is8bit;
}

#ifdef GL_EXT_SHARED
void VID_Init8bitPalette() 
{
	// Check for 8bit Extensions and initialize them.
	int i;
	char thePalette[256*3];
	char *oldPalette, *newPalette;

	if (strstr(gl_extensions, "GL_EXT_shared_texture_palette") == NULL)
		return;

	Con_SafePrintf("8-bit GL extensions enabled.\n");
	glEnable( GL_SHARED_TEXTURE_PALETTE_EXT );
	oldPalette = (char *) d_8to24table; //d_8to24table3dfx;
	newPalette = thePalette;
	for (i=0;i<256;i++) {
		*newPalette++ = *oldPalette++;
		*newPalette++ = *oldPalette++;
		*newPalette++ = *oldPalette++;
		oldPalette++;
	}
	glColorTableEXT(GL_SHARED_TEXTURE_PALETTE_EXT, GL_RGB, 256, GL_RGB, GL_UNSIGNED_BYTE, (void *) thePalette);
	is8bit = true;
}

#else

void VID_Init8bitPalette(void) 
{
}

#endif

void VID_Init(unsigned char *palette)
{
	int i;
	int attrib[] = {
		GLX_RGBA,
		GLX_RED_SIZE, 1,
		GLX_GREEN_SIZE, 1,
		GLX_BLUE_SIZE, 1,
		GLX_DOUBLEBUFFER,
		GLX_DEPTH_SIZE, 1,
		None
	};
	char	gldir[MAX_OSPATH];
	int width = 640, height = 480;
	XSetWindowAttributes attr;
	unsigned long mask;
	Window root;
	XVisualInfo *visinfo;

	S_Init();

	Cvar_RegisterVariable(&vid_mode);
	Cvar_RegisterVariable(&gl_ztrick); 
        Cvar_RegisterVariable(&vid_glx_fullscreen);

	vid.maxwarpwidth = WARP_WIDTH;
	vid.maxwarpheight = WARP_HEIGHT;
	vid.colormap = host_colormap;
	vid.fullbright = 256 - LittleLong (*((int *)vid.colormap + 2048));

	/* Interpret command-line params
	 */

	/* Set vid parameters */
	if ((i = COM_CheckParm("-width")) != 0)
		width = atoi(com_argv[i+1]);
	if ((i = COM_CheckParm("-height")) != 0)
		height = atoi(com_argv[i+1]);

	if ((i = COM_CheckParm("-conwidth")) != 0)
		vid.conwidth = Q_atoi(com_argv[i+1]);
	else
		vid.conwidth = width;

	vid.conwidth &= 0xfff8; // make it a multiple of eight
	if (vid.conwidth < 320)
		vid.conwidth = 320;

	// pick a conheight that matches with correct aspect
	vid.conheight = vid.conwidth * 3 / 4;

	i = COM_CheckParm ("-conheight");
	if ( i != 0 )	// Set console height, but no smaller than 200 px
		vid.conheight = Q_atoi(com_argv[i+1]);
	if (vid.conheight < 200)
		vid.conheight = 200;

	x_disp = XOpenDisplay(NULL);
	if ( !x_disp ) {
		fprintf(stderr, "Error couldn't open the X display\n");
		exit(1);
	}

	screen = DefaultScreen(x_disp);
	root = RootWindow(x_disp, screen);

	visinfo = glXChooseVisual(x_disp, screen, attrib);
	if (!visinfo) {
		fprintf(stderr, "Error couldn't get an RGB, Double-buffered, Depth visual\n");
		exit(1);
	}

#ifdef HAS_DGA
	{
		int maj_ver;

		hasdga = VID_CheckDGA(x_disp, &maj_ver, NULL, &hasdgavideo);
		if (!hasdga || maj_ver < 1) {
			hasdga = hasdgavideo = 0;
		}
	}
	hasvidmode = VID_CheckVMode(x_disp, NULL, NULL);
	if (hasvidmode) {
		if (! XF86VidModeGetAllModeLines(x_disp, DefaultScreen(x_disp),
						 &nummodes, &vidmodes)
		    || nummodes <= 0) {
			hasvidmode = 0;
		}
	}
#endif
#ifdef HAVE_DLOPEN
	dlhand = dlopen(NULL, RTLD_LAZY);
	if (dlhand) {
		QF_XMesaSetFXmode = dlsym(dlhand, "XMesaSetFXmode");
		if (!QF_XMesaSetFXmode) {
			QF_XMesaSetFXmode = dlsym(dlhand, "_XMesaSetFXmode");
		}
	} else {
		QF_XMesaSetFXmode = NULL;
	}
#else
#ifdef XMESA
	QF_XMesaSetFXmode = XMesaSetFXmode;
#endif
#endif
	if (QF_XMesaSetFXmode) {
#ifdef XMESA
		const char *str = getenv("MESA_GLX_FX");
		if (str != NULL && *str != 'f') {
			if (tolower(*str) == 'w') {
				Cvar_SetValue("vid_glx_fullscreen", 0);
			} else {
				Cvar_SetValue("vid_glx_fullscreen", 1);
			}
		}
#endif
		/* Glide uses DGA internally, so we don't want to
		   mess with it. */
		hasdga = 0;
	}

	/* window attributes */
	attr.background_pixel = 0;
	attr.border_pixel = 0;
	attr.colormap = XCreateColormap(x_disp, root, visinfo->visual, AllocNone);
	attr.event_mask = X_MASK;
	mask = CWBackPixel | CWBorderPixel | CWColormap | CWEventMask;

	x_win = XCreateWindow(x_disp, root, 0, 0, width, height,
						0, visinfo->depth, InputOutput,
						visinfo->visual, mask, &attr);
	XMapWindow(x_disp, x_win);

	XSync(x_disp, 0);

	ctx = glXCreateContext(x_disp, visinfo, NULL, True);

	glXMakeCurrent(x_disp, x_win, ctx);

	scr_width = width;
	scr_height = height;

	if (vid.conheight > height)
		vid.conheight = height;
	if (vid.conwidth > width)
		vid.conwidth = width;
	vid.width = vid.conwidth;
	vid.height = vid.conheight;

	vid.aspect = ((float)vid.height / (float)vid.width) * (320.0 / 240.0);
	vid.numpages = 2;

	InitSig(); // trap evil signals

	GL_Init();

	snprintf(gldir, sizeof(gldir), "%s/glquake", com_gamedir);
	Sys_mkdir (gldir);

	VID_SetPalette(palette);

	// Check for 3DFX Extensions and initialize them.
	VID_Init8bitPalette();

	Con_SafePrintf ("Video mode %dx%d initialized.\n",
			width, height);

	vid.recalc_refdef = 1;		// force a surface cache flush
}
