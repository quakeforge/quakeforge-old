/*
	nozip.h

	(description)

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

#ifndef _NOZIP_H
#define _NOZIP_H
#include <config.h>

#ifndef HAS_ZLIB
#define gzFile  FILE
#define gzdopen fdopen
#define gzread(F, BUF, LEN)  fread(BUF, 1, LEN, F)
#define gzwrite(F, BUF, LEN) fwrite(BUF, 1, LEN, F)
/* #define gzwrite(F, BUF, SIZE,LEN) fwrite( */
#define gzflush(FILE, VAL)   fflush(FILE)
#define gzprintf fprintf
#define gzgetc fgetc
#define gzseek fseek
#define gztell ftell
#define gzopen fopen
#define gzclose fclose
#define gzgets(FILE,BUF,SIZE) fgets(BUF,SIZE,FILE)
#endif
#endif // _NOZIP_H
