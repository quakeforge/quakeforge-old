/*
	lib_replace.c

	(description)

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

#include <ctype.h>
#include <stdlib.h>

/*
============================================================================

					LIBRARY REPLACEMENT FUNCTIONS

============================================================================
*/

int Q_atoi (char *str)
{
	int		val;
	int		sign;
	int		c;

	if (*str == '-')
	{
		sign = -1;
		str++;
	}
	else
		sign = 1;

	val = 0;

//
// check for hex
//
	if (str[0] == '0' && (str[1] == 'x' || str[1] == 'X') )
	{
		str += 2;
		while (1)
		{
			c = *str++;
			if (c >= '0' && c <= '9')
				val = (val<<4) + c - '0';
			else if (c >= 'a' && c <= 'f')
				val = (val<<4) + c - 'a' + 10;
			else if (c >= 'A' && c <= 'F')
				val = (val<<4) + c - 'A' + 10;
			else
				return val*sign;
		}
	}

//
// check for character
//
	if (str[0] == '\'')
	{
		return sign * str[1];
	}

//
// assume decimal
//
	while (1)
	{
		c = *str++;
		if (c <'0' || c > '9')
			return val*sign;
		val = val*10 + c - '0';
	}

	return 0;
}


float Q_atof (char *str)
{
	double	val;
	int		sign;
	int		c;
	int		decimal, total;

	if (*str == '-')
	{
		sign = -1;
		str++;
	}
	else
		sign = 1;

	val = 0;

//
// check for hex
//
	if (str[0] == '0' && (str[1] == 'x' || str[1] == 'X') )
	{
		str += 2;
		while (1)
		{
			c = *str++;
			if (c >= '0' && c <= '9')
				val = (val*16) + c - '0';
			else if (c >= 'a' && c <= 'f')
				val = (val*16) + c - 'a' + 10;
			else if (c >= 'A' && c <= 'F')
				val = (val*16) + c - 'A' + 10;
			else
				return val*sign;
		}
	}

//
// check for character
//
	if (str[0] == '\'')
	{
		return sign * str[1];
	}

//
// assume decimal
//
	decimal = -1;
	total = 0;
	while (1)
	{
		c = *str++;
		if (c == '.')
		{
			decimal = total;
			continue;
		}
		if (c <'0' || c > '9')
			break;
		val = val*10 + c - '0';
		total++;
	}

	if (decimal == -1)
		return val*sign;
	while (total > decimal)
	{
		val /= 10;
		total--;
	}

	return val*sign;
}

// Note, this is /NOT/ a work-alike strcmp, this groups numbers sanely.
int Q_qstrcmp(const char *val, const char *ref)
{
	int vc, rc;
	long vl, rl;
	const char *vp, *rp;

	if (!val) val= "";
	if (!ref) ref= "";
	for (;;) {
		vp= val;  while (*vp && !isdigit(*vp)) vp++;
		rp= ref;  while (*rp && !isdigit(*rp)) rp++;
		for (;;) {
			vc= val == vp ? 0 : *val++;
			rc= ref == rp ? 0 : *ref++;
			if (!rc && !vc) break;
			if (vc && !isalpha(vc)) vc += 256; /* assumes ASCII character set */
			if (rc && !isalpha(rc)) rc += 256;
			if (vc != rc) return vc - rc;
		}
		val= vp;
		ref= rp;
		vl=0;  if (isdigit(*vp)) vl= strtol(val,(char**)&val,10);
		rl=0;  if (isdigit(*rp)) rl= strtol(ref,(char**)&ref,10);
		if (vl != rl) return vl - rl;
		if (!*val && !*ref) return 0;
		if (!*val) return -1;
		if (!*ref) return +1;
	}
}

