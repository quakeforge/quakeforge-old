/*
	pcx.c

	PCX loading/saving functions

	Copyright (C) 1996-1997  Id Software, Inc.
	Copyright (C) 1999,2000  contributors of the QuakeForge project
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
		Boston, MA  02111-1307, USA

	$Id$
*/

#include <string.h>
#include <stdlib.h>
#include <qtypes.h>
#include <quakeio.h>
#include <console.h>
#include <image.h>
#include <zone.h>
#include <quakefs.h>
#include <sys.h>

#ifdef HAVE_STRING_H
#include <string.h>
#endif

/*
	LoadPCX
*/
byte *LoadPCX (char *file, cache_user_t *cache, int buf_x, int buf_y) {

	pcx_t	*pcx;
	byte	*pix;
	byte	*raw, *out;
	int	x, y;
	int	dataByte, runLength;

/*
	Parse PCX file
*/
	out = Cache_Check (cache);
	if (out)
	{
		Con_Printf("Using cached version of %s\n", file);
		return out;
	}

	raw = COM_LoadTempFile (file);
	if (!raw) {
		Con_Printf("Can not open %s\n", file);
		return NULL;
	}

	pcx = (pcx_t *)raw;
	raw = &pcx->data;

	if (pcx->manufacturer != 0x0a
			|| pcx->version != 5
			|| pcx->encoding != 1
			|| pcx->bits_per_pixel != 8
			|| pcx->xmax >= 320
			|| pcx->ymax >= 200)
	{
		Con_Printf ("Bad PCX file\n");
		return NULL;
	}

	if (!buf_x)
		buf_x = pcx->ymax;
	if (!buf_y)
		buf_y = pcx->ymax;

	Con_DPrintf("PCX file %s %dx%d\n", file, buf_x, buf_y);
	out = Cache_Alloc (cache, buf_x * buf_y, file);
	if (!out)
		Sys_Error("LoadPCX: couldn't allocate.");

	pix = out;
	memset(out, 0, buf_x * buf_y);

	for (y=0 ; y<=pcx->ymax ; y++, pix += buf_x) {
		for (x=0 ; x<=pcx->xmax ; ) {
			if (raw - (byte*)pcx > com_filesize) {
				Cache_Free(cache);
				Con_Printf("PCX file %s was malformed.  You should delete it.\n", file);
				return NULL;
			}
			dataByte = *raw++;

			if ((dataByte & 0xC0) == 0xC0) {
				runLength = dataByte & 0x3F;
				if (raw - (byte*)pcx > com_filesize) {
					Cache_Free(cache);
					Con_Printf("PCX file %s was malformed.  You should delete it.\n", file);
					return NULL;
				}
				dataByte = *raw++;
			} else
				runLength = 1;

			if (runLength + x > pcx->xmax + 2) {
				Cache_Free(cache);
				Con_Printf("PCX file %s was malformed.  You should delete it.\n", file);
				return NULL;
			}

			while(runLength-- > 0)
				pix[x++] = dataByte;
		}
	}
	if (raw - (byte*)pcx > com_filesize) {
		Cache_Free(cache);
		Con_Printf("PCX file %s was malformed.  You should delete it.\n", file);
		return NULL;
	}

	return out;
}

