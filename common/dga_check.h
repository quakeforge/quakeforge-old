/*
	dga_check.h

	Definitions for XFree86 DGA and VidMode support

	Copyright (C) 2000 Contributors of the QuakeForge Project
	Copyright (C) 2000 Marcus Sundberg [mackan@stacken.kth.se]
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
	Boston, MA  02111-1307, USA.
*/

#ifndef DGA_CHECK_H
#define DGA_CHECK_H

#include <X11/Xlib.h>

/*
  VID_CheckDGA

  Check for the presence of the XFree86-DGA support in the X server
*/
int VID_CheckDGA(Display *dpy, int *maj_ver, int *min_ver, int *hasvideo);


/*
  VID_CheckVMode

  Check for the presence of the XFree86-VMode X server extension
*/
int VID_CheckVMode(Display *dpy, int *maj_ver, int *min_ver);

#endif /* DGA_CHECK_H */
