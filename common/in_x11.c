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

#ifdef HAS_DGA
#include <X11/extensions/XShm.h>
#include <X11/extensions/xf86dga.h>
#endif

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
#include <context_x11.h>

#ifdef HAVE_STRINGS_H
#include <strings.h>
#endif

//cvar_t		_windowed_mouse = {"_windowed_mouse","0", CVAR_ARCHIVE};
cvar_t	*_windowed_mouse;
//cvar_t		m_filter = {"m_filter","0", CVAR_ARCHIVE};
cvar_t	*m_filter;
#ifdef HAS_DGA
qboolean		dgamouse = 0;
//static cvar_t	vid_dga_mouseaccel = {"vid_dga_mouseaccel", "1", CVAR_ARCHIVE};
cvar_t	*vid_dga_mouseaccel;
#endif

static qboolean	mouse_avail;
static float	mouse_x, mouse_y;
static float	old_mouse_x, old_mouse_y;
static int	p_mouse_x, p_mouse_y;
static float	old_windowed_mouse;

#define KEY_MASK (KeyPressMask | KeyReleaseMask)
#define MOUSE_MASK (ButtonPressMask | ButtonReleaseMask | PointerMotionMask)
#define INPUT_MASK (KEY_MASK | MOUSE_MASK)


/*
  Called at shutdown
*/
void IN_Shutdown(void)
{
	Con_Printf("IN_Shutdown\n");
	mouse_avail = 0;
	XAutoRepeatOn(x_disp);
	x11_close_display();
}


int XLateKey(XKeyEvent *ev)
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

		case XK_Caps_Lock:	key = K_CAPSLOCK; break;
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

static void event_key(XEvent *event)
{
	Key_Event(XLateKey(&event->xkey), event->type == KeyPress);
}

static void event_button(XEvent *event)
{
	int but;

	but = event->xbutton.button;
	if (but == 2) but = 3;
	else if (but == 3) but = 2;
	switch(but) {
		case 1:
		case 2:
		case 3:
			Key_Event(K_MOUSE1 + but - 1, event->type == ButtonPress);
			break;
		case 4:
			Key_Event(K_MWHEELUP, 1);
			Key_Event(K_MWHEELUP, 0);
			break;
		case 5:
			Key_Event(K_MWHEELDOWN, 1);
			Key_Event(K_MWHEELDOWN, 0);
			break;
	}
}

static void event_motion(XEvent *event)
{
#ifdef HAS_DGA
	if (dgamouse) {
		mouse_x += event->xmotion.x_root * vid_dga_mouseaccel->value;
		mouse_y += event->xmotion.y_root * vid_dga_mouseaccel->value;
	} else
#endif
		//printf("_windowed_mouse: %f\n", _windowed_mouse->value);
		//printf("CurrentTime: %ld\n", CurrentTime);
		if (_windowed_mouse->value) {
			mouse_x = (float) ((int) event->xmotion.x - ((int) vid.width / 2));
			mouse_y = (float) ((int) event->xmotion.y - ((int) vid.height / 2));

			/* move the mouse to the window center again */
			XGrabPointer(x_disp, x_win, True, MOUSE_MASK & ~PointerMotionMask,
						GrabModeAsync, GrabModeAsync, x_win, None, CurrentTime);
			XSelectInput(x_disp, x_win, INPUT_MASK & ~PointerMotionMask);
			XWarpPointer(x_disp, None, x_win, 0, 0, 0, 0,
					(vid.width / 2), (vid.height / 2));
			XSelectInput(x_disp, x_win, INPUT_MASK);
			XGrabPointer(x_disp, x_win, True, MOUSE_MASK, GrabModeAsync,
						 GrabModeAsync, x_win, None, CurrentTime);
		} else {
			mouse_x = (event->xmotion.x - p_mouse_x);
			mouse_y = (event->xmotion.y - p_mouse_y);
			p_mouse_x = event->xmotion.x;
			p_mouse_y = event->xmotion.y;
		}
}


void
IN_Frame(void)
{
	if (old_windowed_mouse != _windowed_mouse->value) {
		old_windowed_mouse = _windowed_mouse->value;

		if (!_windowed_mouse->value) {
			/* ungrab the pointer */
			XUngrabPointer(x_disp,CurrentTime);
		} else {
			/* grab the pointer */
			XGrabPointer(x_disp, x_win, True, MOUSE_MASK, GrabModeAsync,
						 GrabModeAsync, x_win, None, CurrentTime);
			//XGrabPointer(x_disp,x_win,True,0,GrabModeAsync,
			//		GrabModeAsync,x_win,None,CurrentTime);
		}
	}
}


void
IN_SendKeyEvents(void)
{
	/* Get events from X server. */
	x11_process_events();
}


void IN_Move(usercmd_t *cmd)
{
	if (!mouse_avail)
		return;

	if (m_filter->value) {
		mouse_x = (mouse_x + old_mouse_x) * 0.5;
		mouse_y = (mouse_y + old_mouse_y) * 0.5;
	}

	old_mouse_x = mouse_x;
	old_mouse_y = mouse_y;

	mouse_x *= sensitivity->value;
	mouse_y *= sensitivity->value;

	if ( (in_strafe.state & 1) || (lookstrafe->value && (in_mlook.state & 1) ))
		cmd->sidemove += m_side->value * mouse_x;
	else
		cl.viewangles[YAW] -= m_yaw->value * mouse_x;
	if (in_mlook.state & 1)
		V_StopPitchDrift ();

	if ( (in_mlook.state & 1) && !(in_strafe.state & 1)) {
		cl.viewangles[PITCH] += m_pitch->value * mouse_y;
		if (cl.viewangles[PITCH] > 80)
			cl.viewangles[PITCH] = 80;
		if (cl.viewangles[PITCH] < -70)
			cl.viewangles[PITCH] = -70;
	} else {
		if ((in_strafe.state & 1) && noclip_anglehack)
			cmd->upmove -= m_forward->value * mouse_y;
		else
			cmd->forwardmove -= m_forward->value * mouse_y;
	}
	mouse_x = mouse_y = 0.0;
}

/*
static void IN_ExtraOptionDraw(unsigned int options_draw_cursor)
{
	// Windowed Mouse
	M_Print(16, options_draw_cursor+=8, "             Use Mouse");
	M_DrawCheckbox(220, options_draw_cursor, _windowed_mouse->value);
}

static void IN_ExtraOptionCmd(int option_cursor)
{
	switch (option_cursor) {
	case 1:	// _windowed_mouse
		_windowed_mouse->value = !_windowed_mouse->value;
		break;
	}
}
*/

int IN_Init ()
{
// open the display
	if (!x_disp)
		Sys_Error("IN: No display!!\n");
	if (!x_win)
		Sys_Error("IN: No window!!\n");

	x11_open_display();	// call to increment the reference counter
	{
		int attribmask = CWEventMask;
		XWindowAttributes attribs_1;
		XSetWindowAttributes attribs_2;

		XGetWindowAttributes(x_disp, x_win, &attribs_1);

		attribs_2.event_mask = attribs_1.your_event_mask | INPUT_MASK;

		XChangeWindowAttributes(x_disp, x_win, attribmask, &attribs_2);
	}

	_windowed_mouse = Cvar_Get ("_windowed_mouse","0",CVAR_ARCHIVE,"None");
	m_filter = Cvar_Get ("m_filter","0",CVAR_ARCHIVE,"None");
#ifdef HAS_DGA
	vid_dga_mouseaccel = Cvar_Get ("vid_dga_mouseaccel","1",CVAR_ARCHIVE,
					"None");
#if 0
	XF86DGASetViewPort(x_disp, x_win, 0, 0);
	XF86DGADirectVideo(x_disp, x_win, XF86DGADirectGraphics|XF86DGADirectMouse|XF86DGADirectKeyb);
	XF86DGASetVidPage(x_disp, x_win, 0);

	XGrabKeyboard(x_disp, x_win, True, GrabModeAsync, GrabModeAsync, CurrentTime);
	XGrabPointer(x_disp, x_win, True, MOUSE_MASK, GrabModeAsync, GrabModeAsync,
				 x_win, None, CurrentTime);
#endif
#endif
	if (COM_CheckParm("-nomouse")) return 1;
	mouse_x = mouse_y = 0.0;
	mouse_avail = 1;

	x11_add_event(KeyPress, &event_key);
	x11_add_event(KeyRelease, &event_key);
	x11_add_event(ButtonPress, &event_button);
	x11_add_event(ButtonRelease, &event_button);
	x11_add_event(MotionNotify, &event_motion);

	return 1;
}
