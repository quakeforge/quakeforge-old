/* 
   Unix SMB/Netbios implementation.
   Version 1.9.
   a implementation of MD4 designed for use in the SMB authentication protocol
   Copyright (C) Andrew Tridgell 1997-1998.
   
   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.
   
   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.
   
   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*/

#ifndef _UINT32_H
#include	<uint32.h>
#endif

#ifndef _MDFOUR_H
#define _MDFOUR_H

struct mdfour {
	uint32 A, B, C, D;
	uint32 totalN;
};

void mdfour_begin(struct mdfour *md); // old: MD4Init
void mdfour_update(struct mdfour *md, unsigned char *in, int n); //old: MD4Update
void mdfour_result(struct mdfour *md, unsigned char *out); // old: MD4Final
void mdfour(unsigned char *out, unsigned char *in, int n);

#endif	// _MDFOUR_H
