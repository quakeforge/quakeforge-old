/*
	pcx.c
	
	PCX loading/saving functions
	
	Copyright (C) 1996-1997  Id Software, Inc.
	Copyright (C) 1999,2000  contributors of the QuakeForge project
	Please see the file "AUTHORS" for a list of contributors
	Portions Copyright (C) 1999,2000  Nelson Rush.

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
*/

#include <stdlib.h>
#include <qtypes.h>
#include <quakeio.h>
#include <console.h>
#include <image.h>

/*
	LoadPCX
*/
void
LoadPCX (QFile *f, byte **pcx_rgb) {

	pcx_t	*pcx, pcxbuf;
	byte	palette[768];
	byte	*pix;
	int	x, y;
	int	dataByte, runLength;
	int	count;

/*
	Parse PCX file
*/
	Qread (f, &pcxbuf, sizeof(pcxbuf));

	pcx = &pcxbuf;

	if (pcx->manufacturer != 0x0a || pcx->version != 5 || pcx->encoding != 1
			|| pcx->bits_per_pixel != 8 || pcx->xmax >= 320
			|| pcx->ymax >= 256) {
		Con_Printf ("Bad PCX file\n");
		return;
	}

	// seek to palette
	Qseek (f, -768, SEEK_END);
	Qread (f, palette, 768);

	Qseek (f, sizeof(pcxbuf) - 4, SEEK_SET);

	count = (pcx->xmax+1) * (pcx->ymax+1);
	*pcx_rgb = malloc( count); // * 3);

	for (y=0 ; y<=pcx->ymax ; y++)
	{
		pix = *pcx_rgb + /*3**/y*(pcx->xmax+1);
		for (x=0 ; x<=pcx->ymax ; )
		{
			dataByte = Qgetc(f);

			if ((dataByte & 0xC0) == 0xC0)
			{
				runLength = dataByte & 0x3F;
				dataByte = Qgetc(f);
			}
			else
				runLength = 1;

			while(runLength-- > 0) {
				pix[x++] = dataByte;
//				pix[0] = palette[dataByte*3];
//				pix[1] = palette[dataByte*3+1];
//				pix[2] = palette[dataByte*3+2];
//				pix[3] = 255;
//				pix += 4;
//				x++;
			}
		}
	}
}

