/*
in_x11.c - general x11 input driver
Copyright (C) 1996-1997	Id Software, Inc.
Copyright (C) 1999-2000	contributors of the QuakeForge project
Copyright (C) 2000	Marcus Sundberg [mackan@stacken.kth.se]
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

#define _BSD
#include <config.h>

#include <ctype.h>
#include <sys/time.h>
#include <sys/types.h>
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif
#include <signal.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xatom.h>
#include <X11/keysym.h>
#include <X11/extensions/XShm.h>
#include <errno.h>

#include <quakedef.h>
#include <d_local.h>
#include <sound.h>
#include <keys.h>
#include <cvar.h>
#include <sys.h>
#include <cmd.h>
#include <lib_replace.h>
#include <draw.h>
#include <console.h>
#include <client.h>
#include <plugin.h>

#ifdef HAVE_STRINGS_H
#include <strings.h>
#endif

cvar_t		_windowed_mouse = {"_windowed_mouse","0", CVAR_ARCHIVE};
cvar_t		m_filter = {"m_filter","0", CVAR_ARCHIVE};

static qboolean	mouse_avail;
static float	mouse_x, mouse_y;
static float	old_mouse_x, old_mouse_y;
static int	p_mouse_x, p_mouse_y;
static qboolean	mouse_in_window = false;
static float	old_windowed_mouse;

static Display		*x_disp = NULL;
static Window		x_win;
static Atom		aWMDelete = 0;


int XShmQueryExtension(Display *);
int XShmGetEventBase(Display *);

static qboolean		doShm;
static int		x_shmeventtype;

static qboolean		oktodraw = false;

static int verbose = 0;

int	VID_options_items = 1;

#define STD_EVENT_MASK \
	(KeyPressMask | KeyReleaseMask | ButtonPressMask | ButtonReleaseMask \
	 | PointerMotionMask | EnterWindowMask | LeaveWindowMask \
	 | VisibilityChangeMask | ExposureMask | StructureNotifyMask)


// ========================================================================
// makes a null cursor
// ========================================================================

static Cursor CreateNullCursor(Display *display, Window root)
{
    Pixmap cursormask; 
    XGCValues xgc;
    GC gc;
    XColor dummycolour;
    Cursor cursor;

    cursormask = XCreatePixmap(display, root, 1, 1, 1/*depth*/);
    xgc.function = GXclear;
    gc =  XCreateGC(display, cursormask, GCFunction, &xgc);
    XFillRectangle(display, cursormask, gc, 0, 0, 1, 1);
    dummycolour.pixel = 0;
    dummycolour.red = 0;
    dummycolour.flags = 04;
    cursor = XCreatePixmapCursor(display, cursormask, cursormask,
          &dummycolour,&dummycolour, 0,0);
    XFreePixmap(display,cursormask);
    XFreeGC(display,gc);
    return cursor;
}

// ========================================================================
// Tragic death handler
// ========================================================================

static void TragicDeath(int signal_num)
{
	XAutoRepeatOn(x_disp);
	XCloseDisplay(x_disp);
	Sys_Error("This death brought to you by the number %d\n", signal_num);
}


static int X_IN_Init (unsigned char *palette)
{
	int template_mask;
	int screen;

	srandom(getpid());

	verbose=COM_CheckParm("-verbose");

// open the display
	x_disp = XOpenDisplay(NULL);
	if (!x_disp)
	{
		if (getenv("DISPLAY"))
			Sys_Error("IN: Could not open display [%s]\n",
				getenv("DISPLAY"));
		else
			Sys_Error("IN: Could not open local display\n");
	}

// catch signals so i can turn on auto-repeat

	{
		struct sigaction sa;
		sigaction(SIGINT, 0, &sa);
		sa.sa_handler = TragicDeath;
		sigaction(SIGINT, &sa, 0);
		sigaction(SIGTERM, &sa, 0);
	}

	XAutoRepeatOff(x_disp);

// for debugging only
	XSynchronize(x_disp, True);


	template_mask = 0;

	screen = XDefaultScreen(x_disp);

	/* Setup attributes for main window */
	{
		int attribmask = CWEventMask | CWBorderPixel;
		XSetWindowAttributes attribs;

		attribs.event_mask = STD_EVENT_MASK;
		attribs.border_pixel = 0;

		/* Create the main window */
		x_win = XCreateWindow(x_disp,
			      XRootWindow(x_disp, screen),
			      0, 0, 300, 200,
			      0, /* borderwidth	*/
			      0, InputOutput, CopyFromParent,
			      attribmask, &attribs);

		/* Give it a title */
		XStoreName(x_disp, x_win, "QF X11 Input");

		/* Make window respond to Delete events */
		aWMDelete = XInternAtom(x_disp, "WM_DELETE_WINDOW", False);
		XSetWMProtocols(x_disp, x_win, &aWMDelete, 1);
	}

	XDefineCursor(x_disp, x_win, CreateNullCursor(x_disp, x_win));

// map the window
	XMapWindow(x_disp, x_win);

//	XSynchronize(x_disp, False);

	Cvar_RegisterVariable(&_windowed_mouse);
	Cvar_RegisterVariable(&m_filter);
	if (COM_CheckParm("-nomouse")) return 1;
	mouse_x = mouse_y = 0.0;
	mouse_avail = 1;

	return 1;
}

/*
  Called at shutdown
*/
static void X_IN_Shutdown(void)
{
	Con_Printf("IN_Shutdown\n");
	mouse_avail = 0;
	if (x_disp) {
		XAutoRepeatOn(x_disp);
		XCloseDisplay(x_disp);
	}
}


static int XLateKey(XKeyEvent *ev)
{
	int key = 0;
	char buf[64];
	KeySym keysym;

	XLookupString(ev, buf, sizeof(buf), &keysym, 0);

	switch(keysym) {
		case XK_KP_Page_Up:	key = KP_PGUP; break;
		case XK_Page_Up:	key = K_PGUP; break;

		case XK_KP_Page_Down:	key = KP_PGDN; break;
		case XK_Page_Down:	key = K_PGDN; break;

		case XK_KP_Home:	key = KP_HOME; break;
		case XK_Home:		key = K_HOME; break;

		case XK_KP_End:		key = KP_END; break;
		case XK_End:		key = K_END; break;

		case XK_KP_Left:	key = KP_LEFTARROW; break;
		case XK_Left:		key = K_LEFTARROW; break;

		case XK_KP_Right:	key = KP_RIGHTARROW; break;
		case XK_Right:		key = K_RIGHTARROW; break;

		case XK_KP_Down:	key = KP_DOWNARROW; break;
		case XK_Down:		key = K_DOWNARROW; break;

		case XK_KP_Up:		key = KP_UPARROW; break;
		case XK_Up:		key = K_UPARROW; break;

		case XK_Escape:		key = K_ESCAPE; break;

		case XK_KP_Enter:	key = KP_ENTER; break;
		case XK_Return:		key = K_ENTER; break;

		case XK_Tab:		key = K_TAB; break;

		case XK_F1:		key = K_F1; break;
		case XK_F2:		key = K_F2; break;
		case XK_F3:		key = K_F3; break;
		case XK_F4:		key = K_F4; break;
		case XK_F5:		key = K_F5; break;
		case XK_F6:		key = K_F6; break;
		case XK_F7:		key = K_F7; break;
		case XK_F8:		key = K_F8; break;
		case XK_F9:		key = K_F9; break;
		case XK_F10:		key = K_F10; break;
		case XK_F11:		key = K_F11; break;
		case XK_F12:		key = K_F12; break;

		case XK_BackSpace:	key = K_BACKSPACE; break;

		case XK_KP_Delete:	key = KP_DEL; break;
		case XK_Delete:		key = K_DEL; break;

		case XK_Pause:		key = K_PAUSE; break;

		case XK_Shift_L:
		case XK_Shift_R:	key = K_SHIFT; break;

		case XK_Execute: 
		case XK_Control_L: 
		case XK_Control_R:	key = K_CTRL; break;

		case XK_Mode_switch:
		case XK_Alt_L:	
		case XK_Meta_L: 
		case XK_Alt_R:	
		case XK_Meta_R:		key = K_ALT; break;

		case XK_KP_Begin:	key = K_AUX30; break;

		case XK_Insert:		key = K_INS; break;
		case XK_KP_Insert:	key = KP_INS; break;

		case XK_KP_Multiply:	key = KP_MULTIPLY; break;
		case XK_KP_Add:		key = KP_PLUS; break;
		case XK_KP_Subtract:	key = KP_MINUS; break;
		case XK_KP_Divide:	key = KP_DIVIDE; break;

		/* For Sun keyboards */
		case XK_F27:		key = K_HOME; break;
		case XK_F29:		key = K_PGUP; break;
		case XK_F33:		key = K_END; break;
		case XK_F35:		key = K_PGDN; break;

#if 0
		case 0x021: key = '1';break;/* [!] */
		case 0x040: key = '2';break;/* [@] */
		case 0x023: key = '3';break;/* [#] */
		case 0x024: key = '4';break;/* [$] */
		case 0x025: key = '5';break;/* [%] */
		case 0x05e: key = '6';break;/* [^] */
		case 0x026: key = '7';break;/* [&] */
		case 0x02a: key = '8';break;/* [*] */
		case 0x028: key = '9';;break;/* [(] */
		case 0x029: key = '0';break;/* [)] */
		case 0x05f: key = '-';break;/* [_] */
		case 0x02b: key = '=';break;/* [+] */
		case 0x07c: key = '\'';break;/* [|] */
		case 0x07d: key = '[';break;/* [}] */
		case 0x07b: key = ']';break;/* [{] */
		case 0x022: key = '\'';break;/* ["] */
		case 0x03a: key = ';';break;/* [:] */
		case 0x03f: key = '/';break;/* [?] */
		case 0x03e: key = '.';break;/* [>] */
		case 0x03c: key = ',';break;/* [<] */
#endif
		default:
			key = *(unsigned char*)buf;
			if (key >= 'A' && key <= 'Z') {
				key = key + ('a' - 'A');
			}
			break;
	} 

	return key;
}

static void GetEvent(void)
{
	XEvent x_event;
	int but;

	XNextEvent(x_disp, &x_event);
	switch(x_event.type) {
	case KeyPress:
		Key_Event(XLateKey(&x_event.xkey), true);
		break;

	case KeyRelease:
		Key_Event(XLateKey(&x_event.xkey), false);
		break;

	case ButtonPress:
		but = x_event.xbutton.button;
		if (but == 2) but = 3;
		else if (but == 3) but = 2;
		switch(but) {
		case 1:
		case 2:
		case 3:
			Key_Event(K_MOUSE1 + but - 1, true);
		}
		break;

	case ButtonRelease:
		but = x_event.xbutton.button;
		if (but == 2) but = 3;
		else if (but == 3) but = 2;
		switch(but) {
		case 1:
		case 2:
		case 3:
			Key_Event(K_MOUSE1 + but - 1, false);
		}
		break;

	case MotionNotify:
		if (_windowed_mouse.value) {
			mouse_x = (float) ((int)x_event.xmotion.x - (int)(vid.width/2));
			mouse_y = (float) ((int)x_event.xmotion.y - (int)(vid.height/2));
//printf("m: x=%d,y=%d, mx=%3.2f,my=%3.2f\n", 
//	x_event.xmotion.x, x_event.xmotion.y, mouse_x, mouse_y);

			/* move the mouse to the window center again */
			XSelectInput(x_disp,x_win,StructureNotifyMask|KeyPressMask
				|KeyReleaseMask|ExposureMask
				|ButtonPressMask
				|ButtonReleaseMask);
			XWarpPointer(x_disp,None,x_win,0,0,0,0, 
				(vid.width/2),(vid.height/2));
			XSelectInput(x_disp,x_win,StructureNotifyMask|KeyPressMask
				|KeyReleaseMask|ExposureMask
				|PointerMotionMask|ButtonPressMask
				|ButtonReleaseMask);
		} else {
			mouse_x = (float) (x_event.xmotion.x-p_mouse_x);
			mouse_y = (float) (x_event.xmotion.y-p_mouse_y);
			p_mouse_x=x_event.xmotion.x;
			p_mouse_y=x_event.xmotion.y;
		}
		break;

	case EnterNotify:
		mouse_in_window = true;
		break;
	case LeaveNotify:
		mouse_in_window = false;
		break;
/* Host_Quit_f only available in uquake */
#ifdef UQUAKE
	case ClientMessage:
		if (x_event.xclient.data.l[0] == aWMDelete) Host_Quit_f();
		break;
#endif

	default:
		if (doShm && x_event.type == x_shmeventtype)
			oktodraw = true;
	}
   
	if (old_windowed_mouse != _windowed_mouse.value) {
		old_windowed_mouse = _windowed_mouse.value;

		if (!_windowed_mouse.value) {
			/* ungrab the pointer */
			XUngrabPointer(x_disp,CurrentTime);
		} else {
			/* grab the pointer */
			XGrabPointer(x_disp,x_win,True,0,GrabModeAsync,
				GrabModeAsync,x_win,None,CurrentTime);
		}
	}
}

static void X_IN_SendKeyEvents(void)
{
	/* Get events from X server. */
	if (x_disp) {
		while (XPending(x_disp)) {
			GetEvent();
		}
	}
}

static void X_IN_Commands(void)
{
	/* Nothing to do here */
}


static void X_IN_Move(usercmd_t *cmd)
{
	if (!mouse_avail)
		return;
   
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

/*
static void X_IN_ExtraOptionDraw(unsigned int options_draw_cursor)
{
	// Windowed Mouse
	M_Print(16, options_draw_cursor+=8, "             Use Mouse");
	M_DrawCheckbox(220, options_draw_cursor, _windowed_mouse.value);
}

static void X_IN_ExtraOptionCmd(int option_cursor)
{
	switch (option_cursor) {
	case 1:	// _windowed_mouse
		Cvar_SetValue ("_windowed_mouse", !_windowed_mouse.value);
		break;
	}
}
*/

static input_pi x11_ip =
{
	NULL,
	NULL,
	"X11 input module",
	X_IN_Init,
	X_IN_Shutdown,
	X_IN_Commands,
	X_IN_SendKeyEvents,
	X_IN_Move,
};

input_pi *get_input_plugin_info()
{
	return &x11_ip;
}

