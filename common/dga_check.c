/*
	dga_check.h
	
	Definitions for DGA support for XFree86
	
	Copyright (C) 2000 contributors of the QuakeForge project
	Please see the file "AUTHORS" for a list of contributors

	Author: Jeff Teunissen
	Date:	09 Jan 2000
	
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
	Boston, MA  02111-1307, USA.
*/

#include <config.h>

#ifdef HAS_DGA
#include "dga_check.h"

#include <X11/extensions/xf86dga.h>
#include <X11/extensions/xf86vmode.h>

/*
	VID_CheckDGA
	
	Check for the presence of the XFree86-DGA X server extension
*/
int
VID_CheckDGA ( Display *dpy )
{
	int *event_base, *error_base;
	
	event_base = error_base = NULL;
	XF86DGAQueryExtension ( dpy, event_base, error_base );
	if ( event_base )
		return true;
	return false;
}

/*
	VID_CheckVMode
	
	Check for the presence of the XFree86-VMode X server extension
*/
int
VID_CheckVMode ( Display *dpy )
{
	int *event_base, *error_base;
	
	event_base = error_base = NULL;
	XF86VidModeQueryExtension ( dpy, event_base, error_base );
	if ( event_base )
		return true;
	return false;
}

#endif // HAS_DGA