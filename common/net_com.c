/*
	net_com.c
	
	MDfour-based checksum utility functions
	
	Copyright (C) 1999, 2000 The QuakeForge Project
	
	Author: Jeff Teunissen	<d2deek@pmail.net>
	Date: 01 Jan 2000
	
	This file is part of the QuakeForge utility library.

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

#include <mdfour.h>

unsigned Com_BlockChecksum (void *buffer, int length)
{
	int				digest[4];
	unsigned 		val;
	struct mdfour	md;

	mdfour_begin (&md);
	mdfour_update (&md, (unsigned char *) buffer, length);
	mdfour_result (&md, (unsigned char *) digest);
//	mdfour ( (unsigned char *) digest, (unsigned char *) buffer, length );
	
	val = digest[0] ^ digest[1] ^ digest[2] ^ digest[3];

	return val;
}

void Com_BlockFullChecksum (void *buffer, int len, unsigned char *outbuf)
{
	struct mdfour	md;
	
	mdfour_begin (&md);
	mdfour_update (&md, (unsigned char *) buffer, len);
	mdfour_result (&md, outbuf);
//	mdfour ( outbuf, (unsigned char *) buffer, len );
}
