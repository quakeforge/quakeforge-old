/*
context_x11.c - general x11 context layer.
Copyright (C) 1996-1997	Id Software, Inc.
Copyright (C) 1999-2000	contributors of the QuakeForge project
Copyright (C) 2000	Zephaniah E. Hull <warp@whitestar.soark.net>
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

#include <context_x11.h>
#include <qtypes.h>

static void (*event_handlers[LASTEvent])(XEvent *);
qboolean oktodraw = false;
int x_shmeventtype;

qboolean x11_add_event(int event, void (*event_handler)(XEvent *))
{
	if (event >= LASTEvent) {
		printf("event: %d, LASTEvent: %d\n", event, LASTEvent);
		return false;
	}
	if (event_handlers[event] != NULL)
		return false;

	event_handlers[event] = event_handler;
	return true;
}

qboolean x11_del_event(int event, void (*event_handler)(XEvent *))
{
	if (event >= LASTEvent)
		return false;
	if (event_handlers[event] != event_handler)
		return false;

	event_handlers[event] = NULL;
	return true;
}

void x11_process_event(void)
{
	XEvent x_event;

	XNextEvent(x_disp, &x_event);
	if (x_event.type >= LASTEvent) {
		// FIXME: KLUGE!!!!!!
		if (x_event.type == x_shmeventtype)
			oktodraw = 1;
		return;
	}
	if (event_handlers[x_event.type])
		event_handlers[x_event.type](&x_event);
}

void x11_process_events(void)
{
	/* Get events from X server. */
	while (XPending(x_disp)) {
		x11_process_event();
	}
}
