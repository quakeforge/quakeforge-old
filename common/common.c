/*
	common.c

	misc functions used in client and server

	Copyright (C) 1996-1997  Id Software, Inc.
	Copyright (C) 1999,2000  Nelson Rush.
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

#include <config.h>
#include <quakedef.h>

#ifdef HAVE_STRING_H
#include <string.h>
#endif
#ifdef HAVE_STRINGS_H
#include <strings.h>
#endif
#include <stdarg.h>
#include <ctype.h>
#include <common.h>
#include <protocol.h>
#include <zone.h>
#include <sys.h>
#include <crc.h>
#include <console.h>
#include <cmd.h>

int				com_no_escapes = 1;
char			com_token[1024];
qboolean		standard_quake = true, rogue, hipnotic;
qboolean		msg_suppress_1 = 0;

// ClearLink is used for new headnodes
void
ClearLink ( link_t *l )
{
	l->prev = l->next = l;
}

void
RemoveLink ( link_t *l )
{
	l->next->prev = l->prev;
	l->prev->next = l->next;
}

void
InsertLinkBefore ( link_t *l, link_t *before )
{
	l->next = before;
	l->prev = before->prev;
	l->prev->next = l;
	l->next->prev = l;
}
void
InsertLinkAfter ( link_t *l, link_t *after )
{
	l->next = after->next;
	l->prev = after;
	l->prev->next = l;
	l->next->prev = l;
}

/*
==============================================================================

			MESSAGE IO FUNCTIONS

Handles byte ordering and avoids alignment errors
==============================================================================
*/

//
// writing functions
//

void
MSG_WriteChar ( sizebuf_t *sb, int c )
{
	byte	*buf;

#ifdef PARANOID
	if (c < -128 || c > 127)
		Sys_Error ("MSG_WriteChar: range error");
#endif

	buf = SZ_GetSpace (sb, 1);
	buf[0] = c;
}

void
MSG_WriteByte ( sizebuf_t *sb, int c )
{
	byte	*buf;

#ifdef PARANOID
	if (c < 0 || c > 255)
		Sys_Error ("MSG_WriteByte: range error");
#endif

	buf = SZ_GetSpace (sb, 1);
	buf[0] = c;
}

void
MSG_WriteShort ( sizebuf_t *sb, int c )
{
	byte	*buf;

#ifdef PARANOID
	if (c < ((short)0x8000) || c > (short)0x7fff)
		Sys_Error ("MSG_WriteShort: range error");
#endif

	buf = SZ_GetSpace (sb, 2);
	buf[0] = c&0xff;
	buf[1] = c>>8;
}

void
MSG_WriteLong ( sizebuf_t *sb, int c )
{
	byte	*buf;

	buf = SZ_GetSpace (sb, 4);
	buf[0] = c&0xff;
	buf[1] = (c>>8)&0xff;
	buf[2] = (c>>16)&0xff;
	buf[3] = c>>24;
}

void
MSG_WriteFloat ( sizebuf_t *sb, float f )
{
	union
	{
		float	f;
		int	l;
	} dat;


	dat.f = f;
	dat.l = LittleLong (dat.l);

	SZ_Write (sb, &dat.l, 4);
}

void
MSG_WriteString ( sizebuf_t *sb, char *s )
{
	if (!s)
		SZ_Write (sb, "", 1);
	else
		SZ_Write (sb, s, Q_strlen(s)+1);
}

void
MSG_WriteCoord ( sizebuf_t *sb, float f )
{
	MSG_WriteShort (sb, (int)(f*8));
}

void
MSG_WriteAngle ( sizebuf_t *sb, float f )
{
	MSG_WriteByte (sb, (int)(f*256/360) & 255);
}

void
MSG_WriteAngle16 ( sizebuf_t *sb, float f )
{
	MSG_WriteShort (sb, (int)(f*65536/360) & 65535);
}

void
MSG_WriteDeltaUsercmd ( sizebuf_t *buf, usercmd_t *from, usercmd_t *cmd )
{
	int		bits;

//
// send the movement message
//
	bits = 0;
	if (cmd->angles[0] != from->angles[0])
		bits |= CM_ANGLE1;
	if (cmd->angles[1] != from->angles[1])
		bits |= CM_ANGLE2;
	if (cmd->angles[2] != from->angles[2])
		bits |= CM_ANGLE3;
	if (cmd->forwardmove != from->forwardmove)
		bits |= CM_FORWARD;
	if (cmd->sidemove != from->sidemove)
		bits |= CM_SIDE;
	if (cmd->upmove != from->upmove)
		bits |= CM_UP;
	if (cmd->buttons != from->buttons)
		bits |= CM_BUTTONS;
	if (cmd->impulse != from->impulse)
		bits |= CM_IMPULSE;

	MSG_WriteByte (buf, bits);

	if (bits & CM_ANGLE1)
		MSG_WriteAngle16 (buf, cmd->angles[0]);
	if (bits & CM_ANGLE2)
		MSG_WriteAngle16 (buf, cmd->angles[1]);
	if (bits & CM_ANGLE3)
		MSG_WriteAngle16 (buf, cmd->angles[2]);

	if (bits & CM_FORWARD)
		MSG_WriteShort (buf, cmd->forwardmove);
	if (bits & CM_SIDE)
		MSG_WriteShort (buf, cmd->sidemove);
	if (bits & CM_UP)
		MSG_WriteShort (buf, cmd->upmove);

 	if (bits & CM_BUTTONS)
		MSG_WriteByte (buf, cmd->buttons);
 	if (bits & CM_IMPULSE)
		MSG_WriteByte (buf, cmd->impulse);
	MSG_WriteByte (buf, cmd->msec);
}
//
// reading functions
//
int			msg_readcount;
qboolean	msg_badread;

void
MSG_BeginReading ( void )
{
	msg_readcount = 0;
	msg_badread = false;
}

int
MSG_GetReadCount ( void )
{
	return msg_readcount;
}

// returns -1 and sets msg_badread if no more characters are available
int
MSG_ReadChar ( void )
{
	int	c;

	if (msg_readcount+1 > net_message.cursize)
	{
		msg_badread = true;
		return -1;
	}

	c = (signed char)net_message.data[msg_readcount];
	msg_readcount++;

	return c;
}

int
MSG_ReadByte ( void )
{
	int	c;

	if (msg_readcount+1 > net_message.cursize)
	{
		msg_badread = true;
		return -1;
	}

	c = (unsigned char)net_message.data[msg_readcount];
	msg_readcount++;

	return c;
}

int
MSG_ReadShort ( void )
{
	int	c;

	if (msg_readcount+2 > net_message.cursize)
	{
		msg_badread = true;
		return -1;
	}

	c = (short)(net_message.data[msg_readcount]
	+ (net_message.data[msg_readcount+1]<<8));

	msg_readcount += 2;

	return c;
}

int
MSG_ReadLong ( void )
{
	int	c;

	if (msg_readcount+4 > net_message.cursize)
	{
		msg_badread = true;
		return -1;
	}

	c = net_message.data[msg_readcount]
	+ (net_message.data[msg_readcount+1]<<8)
	+ (net_message.data[msg_readcount+2]<<16)
	+ (net_message.data[msg_readcount+3]<<24);

	msg_readcount += 4;

	return c;
}

float
MSG_ReadFloat ( void )
{
	union
	{
		byte	b[4];
		float	f;
		int	l;
	} dat;

	dat.b[0] =	net_message.data[msg_readcount];
	dat.b[1] =	net_message.data[msg_readcount+1];
	dat.b[2] =	net_message.data[msg_readcount+2];
	dat.b[3] =	net_message.data[msg_readcount+3];
	msg_readcount += 4;

	dat.l = LittleLong (dat.l);

	return dat.f;
}

char *
MSG_ReadString ( void )
{
	static char	string[2048];
	int		l,c;

	l = 0;
	do
	{
		c = MSG_ReadChar ();
		if (c == -1 || c == 0)
			break;
		string[l] = c;
		l++;
	} while (l < sizeof(string)-1);

	string[l] = 0;

	return string;
}

char *
MSG_ReadStringLine ( void )
{
	static char	string[2048];
	int		l,c;

	l = 0;
	do
	{
		c = MSG_ReadChar ();
		if (c == -1 || c == 0 || c == '\n')
			break;
		string[l] = c;
		l++;
	} while (l < sizeof(string)-1);

	string[l] = 0;

	return string;
}

float
MSG_ReadCoord ( void )
{
	return MSG_ReadShort() * (1.0/8);
}

float
MSG_ReadAngle ( void )
{
	return MSG_ReadChar() * (360.0/256);
}

float
MSG_ReadAngle16 ( void )
{
	return MSG_ReadShort() * (360.0/65536);
}

void
MSG_ReadDeltaUsercmd ( usercmd_t *from, usercmd_t *move )
{
	int bits;

	memcpy (move, from, sizeof(*move));

	bits = MSG_ReadByte ();

// read current angles
	if (bits & CM_ANGLE1)
		move->angles[0] = MSG_ReadAngle16 ();
	if (bits & CM_ANGLE2)
		move->angles[1] = MSG_ReadAngle16 ();
	if (bits & CM_ANGLE3)
		move->angles[2] = MSG_ReadAngle16 ();

// read movement
	if (bits & CM_FORWARD)
		move->forwardmove = MSG_ReadShort ();
	if (bits & CM_SIDE)
		move->sidemove = MSG_ReadShort ();
	if (bits & CM_UP)
		move->upmove = MSG_ReadShort ();

// read buttons
	if (bits & CM_BUTTONS)
		move->buttons = MSG_ReadByte ();

	if (bits & CM_IMPULSE)
		move->impulse = MSG_ReadByte ();

// read time to run command
	move->msec = MSG_ReadByte ();
}

//===========================================================================

void
SZ_Alloc ( sizebuf_t *buf, int startsize )
{
	if (startsize < 256)
		startsize = 256;
	buf->data = Hunk_AllocName (startsize, "sizebuf");
	buf->maxsize = startsize;
	buf->cursize = 0;
}


void
SZ_Free ( sizebuf_t *buf )
{
//	Z_Free (buf->data);
//	buf->data = NULL;
//	buf->maxsize = 0;
	buf->cursize = 0;
}

void
SZ_Clear ( sizebuf_t *buf )
{
	buf->cursize = 0;
	buf->overflowed = false;
}

void *
SZ_GetSpace ( sizebuf_t *buf, int length )
{
	void	*data;

	if (buf->cursize + length > buf->maxsize)
	{
		if (!buf->allowoverflow)
			Sys_Error ("SZ_GetSpace: overflow without allowoverflow set (%d)", buf->maxsize);

		if (length > buf->maxsize)
			Sys_Error ("SZ_GetSpace: %i is > full buffer size", length);

		Sys_Printf ("SZ_GetSpace: overflow\n");	// because Con_Printf may be redirected
		SZ_Clear (buf);
		buf->overflowed = true;
	}

	data = buf->data + buf->cursize;
	buf->cursize += length;

	return data;
}

void
SZ_Write ( sizebuf_t *buf, void *data, int length )
{
	Q_memcpy (SZ_GetSpace(buf,length),data,length);
}

void
SZ_Print ( sizebuf_t *buf, char *data )
{
	int		len;

	len = Q_strlen(data)+1;

	if (!buf->cursize || buf->data[buf->cursize-1])
		Q_memcpy ((byte *)SZ_GetSpace(buf, len),data,len);	// no trailing 0
	else
		Q_memcpy ((byte *)SZ_GetSpace(buf, len-1)-1,data,len);	// write over trailing 0
}
//============================================================================

/*
============
COM_SkipPath
============
*/
char *
COM_SkipPath ( char *pathname )
{
	char	*last;

	last = pathname;
	while (*pathname)
	{
		if (*pathname=='/')
			last = pathname+1;
		pathname++;
	}
	return last;
}

/*
============
COM_StripExtension
============
*/
void
COM_StripExtension ( char *in, char *out )
{
	while (*in && *in != '.')
		*out++ = *in++;
	*out = 0;
}

/*
============
COM_FileExtension
============
*/
char *
COM_FileExtension ( char *in )
{
	static char	exten[8];
	int			i;

	while (*in && *in != '.')
		in++;
	if (!*in)
		return "";
	in++;
	for (i=0 ; i<7 && *in ; i++,in++)
		exten[i] = *in;
	exten[i] = 0;
	return exten;
}

/*
============
COM_FileBase
============
*/
void
COM_FileBase ( char *in, char *out )
{
	char *s, *s2;

	s = in + strlen(in) - 1;

	while (s != in && *s != '.')
		s--;

	for (s2 = s ; *s2 && *s2 != '/' ; s2--)
	;

	if (s-s2 < 2)
		strcpy (out,"?model?");
	else
	{
		s--;
		strncpy (out,s2+1, s-s2);
		out[s-s2] = 0;
	}
}


/*
==================
COM_DefaultExtension
==================
*/
void
COM_DefaultExtension ( char *path, char *extension )
{
	char	*src;
//
// if path doesn't have a .EXT, append extension
// (extension should include the .)
//
	src = path + strlen(path) - 1;

	while (*src != '/' && src != path)
	{
		if (*src == '.')
			return;			// it has an extension
		src--;
	}

	strcat (path, extension);
}
/*
==============
COM_Parse

Parse a token out of a string
==============
*/
char *
COM_Parse ( char *data )
{
	int		c;
	int		len;

	len = 0;
	com_token[0] = 0;

	if (!data)
		return NULL;

// skip whitespace
skipwhite:
	while ( (c = *data) <= ' ')
	{
		if (c == 0)
			return NULL;			// end of file;
		data++;
	}

// skip comments
	if (c=='/' && data[1] == '/')
	{
		while (*data && *data != '\n')
			data++;
		goto skipwhite;
	}


// handle quoted strings specially
	if (c == '\"')
	{
		data++;
		while (*data && *data!='\"')
		{
			c = *data++;
			if (!com_no_escapes && c=='\\') {
				int base=8;
				char buf[4];
				char *str,*string=buf;

				c = *data++;
				switch (c) {
				case 'a': c='\a'; break;
				case 'b': c='\b'; break;
				case 'e': c=27; break;
				case 'f': c='\f'; break;
				case 'n': c='\n'; break;
				case 'r': c='\r'; break;
				case 'x':
					base+=8;
					string=data;
					goto parse_char_number;
//Tonik		case '0' ... '7':	-- MSVC 6.0 doesn't understand this
				case '0': case '1': case '2': case '3':
				case '4': case '5': case '6': case '7':
					strncpy(buf,data,3);
				parse_char_number:
					c=strtol(string,&str,base);
					if (str==string)
						c='x';
					data+=str-string;
					break;
				}
			}
			com_token[len] = c;
			len++;
		}
		if (*data)
			data++;
		com_token[len] = 0;
		return data;
	}

// parse single characters
	if (c=='{' || c=='}'|| c==')'|| c=='(' || c=='\'')
	{
		com_token[len] = c;
		len++;
		com_token[len] = 0;
		return data+1;
	}

// parse a regular word
	do
	{
		com_token[len] = c;
		data++;
		len++;
		c = *data;
	if (c=='{' || c=='}'|| c==')'|| c=='(' || c=='\'')
			break;
	} while (c>32);

	com_token[len] = 0;
	return data;
}

char *
COM_EscapeEscapes ( char *str )
{
	static char buf[4096];
	int i;

	for (i=0; *str && i<sizeof(buf)-1; i++, str++) {
		if (*str=='\\')
			buf[i++]='\\';
		buf[i]=*str;
	}
	return buf;
}

/*
============
va

does a varargs printf into a temp buffer, so I don't need to have
varargs versions of all text functions.
FIXME: make this buffer size safe someday
============
*/
char *
va ( char *format, ... )
{
	va_list		argptr;
	static char	string[1024];

	va_start (argptr, format);
	vsnprintf (string, sizeof(string), format, argptr);
	va_end (argptr);

	return string;
}

/// just for debugging
int
memsearch ( byte *start, int count, int search )
{
	int	i;

	for (i=0 ; i<count ; i++)
		if (start[i] == search)
			return i;
	return -1;
}

/*
================
COM_Init
================
*/
void
COM_Init ( void )
{
#ifdef WORDS_BIGENDIAN
	BigShort = ShortNoSwap;
	LittleShort = ShortSwap;
	BigLong = LongNoSwap;
	LittleLong = LongSwap;
	BigFloat = FloatNoSwap;
	LittleFloat = FloatSwap;
#else
	BigShort = ShortSwap;
	LittleShort = ShortNoSwap;
	BigLong = LongSwap;
	LittleLong = LongNoSwap;
	BigFloat = FloatSwap;
	LittleFloat = FloatNoSwap;
#endif

//	Cvar_RegisterVariable (&cmdline);
	cmdline = Cvar_Get ("cmdline","0",CVAR_USERINFO|CVAR_SERVERINFO,"None");
	Cmd_AddCommand ("path", COM_Path_f);

	COM_InitFilesystem ();
	register_check ();
}

/*
=====================================================================

  INFO STRINGS

=====================================================================
*/

/*
===============
Info_ValueForKey

Searches the string for the given
key and returns the associated value, or an empty string.
===============
*/
char *
Info_ValueForKey ( char *s, char *key )
{
	char	pkey[512];
	static	char value[4][512];	// use two buffers so compares
								// work without stomping on each other
	static	int	valueindex;
	char	*o;

	valueindex = (valueindex + 1) % 4;
	if (*s == '\\')
		s++;
	while (1)
	{
		o = pkey;
		while (*s != '\\')
		{
			if (!*s)
				return "";
			*o++ = *s++;
		}
		*o = 0;
		s++;

		o = value[valueindex];

		while (*s != '\\' && *s)
		{
			if (!*s)
				return "";
			*o++ = *s++;
		}
		*o = 0;

		if (!strcmp (key, pkey) )
			return value[valueindex];

		if (!*s)
			return "";
		s++;
	}
}

void
Info_RemoveKey ( char *s, char *key )
{
	char	*start;
	char	pkey[512];
	char	value[512];
	char	*o;

	if (strstr (key, "\\"))
	{
		Con_Printf ("Can't use a key with a \\\n");
		return;
	}

	while (1)
	{
		start = s;
		if (*s == '\\')
			s++;
		o = pkey;
		while (*s != '\\')
		{
			if (!*s)
				return;
			*o++ = *s++;
		}
		*o = 0;
		s++;

		o = value;
		while (*s != '\\' && *s)
		{
			if (!*s)
				return;
			*o++ = *s++;
		}
		*o = 0;

		if (!strcmp (key, pkey) )
		{
			strcpy (start, s);	// remove this part
			return;
		}

		if (!*s)
			return;
	}

}

void
Info_RemovePrefixedKeys ( char *start, char prefix )
{
	char	*s;
	char	pkey[512];
	char	value[512];
	char	*o;

	s = start;

	while (1)
	{
		if (*s == '\\')
			s++;
		o = pkey;
		while (*s != '\\')
		{
			if (!*s)
				return;
			*o++ = *s++;
		}
		*o = 0;
		s++;

		o = value;
		while (*s != '\\' && *s)
		{
			if (!*s)
				return;
			*o++ = *s++;
		}
		*o = 0;

		if (pkey[0] == prefix)
		{
			Info_RemoveKey (start, pkey);
			s = start;
		}

		if (!*s)
			return;
	}

}


void
Info_SetValueForStarKey ( char *s, char *key, char *value, int maxsize )
{
	char	new[1024], *v;
	int		c;
#ifdef SERVERONLY
	extern cvar_t *sv_highchars;
#endif

	if (strstr (key, "\\") || strstr (value, "\\") )
	{
		Con_Printf ("Can't use keys or values with a \\\n");
		return;
	}

	if (strstr (key, "\"") || strstr (value, "\"") )
	{
		Con_Printf ("Can't use keys or values with a \"\n");
		return;
	}

	if (strlen(key) > 63 || strlen(value) > 63)
	{
		Con_Printf ("Keys and values must be < 64 characters.\n");
		return;
	}

	// this next line is kinda trippy
	if (*(v = Info_ValueForKey(s, key))) {
		// key exists, make sure we have enough room for new value,
		// if we don't, don't change it!
		if (strlen(value) - strlen(v) + strlen(s) > maxsize) {
			Con_Printf ("Info 1 string length exceeded\n");
			return;
		}
	}
	Info_RemoveKey (s, key);
	if (!value || !strlen(value))
		return;

	snprintf(new, sizeof(new), "\\%s\\%s", key, value);

	if ((int)(strlen(new) + strlen(s)) > maxsize)
	{
		Con_Printf ("Info 2 string length exceeded\n");
		return;
	}

	// only copy ascii values
	s += strlen(s);
	v = new;
	while (*v)
	{
		c = (unsigned char)*v++;
#ifndef SERVERONLY
		// client only allows highbits on name
		if (stricmp(key, "name") != 0) {
			c &= 127;
			if (c < 32 || c > 127)
				continue;
			// auto lowercase team
			if (stricmp(key, "team") == 0)
				c = tolower(c);
		}
#else
		if (!sv_highchars->value) {
			c &= 127;
			if (c < 32 || c > 127)
				continue;
		}
#endif
//		c &= 127;		// strip high bits
		if (c > 13)		// && c < 127)
			*s++ = c;
	}
	*s = 0;
}

void
Info_SetValueForKey ( char *s, char *key, char *value, int maxsize )
{
	if (key[0] == '*')
	{
		Con_Printf ("Can't set * keys\n");
		return;
	}

	Info_SetValueForStarKey (s, key, value, maxsize);
}

void
Info_Print ( char *s )
{
	char	key[512];
	char	value[512];
	char	*o;
	int		l;

	if (*s == '\\')
		s++;
	while (*s)
	{
		o = key;
		while (*s && *s != '\\')
			*o++ = *s++;

		l = o - key;
		if (l < 20)
		{
			memset (o, ' ', 20-l);
			key[20] = 0;
		}
		else
			*o = 0;
		Con_Printf ("%s", key);

		if (!*s)
		{
			Con_Printf ("MISSING VALUE\n");
			return;
		}

		o = value;
		s++;
		while (*s && *s != '\\')
			*o++ = *s++;
		*o = 0;

		if (*s)
			s++;
		Con_Printf ("%s\n", value);
	}
}

static byte chktbl[1024 + 4] = {
0x78,0xd2,0x94,0xe3,0x41,0xec,0xd6,0xd5,0xcb,0xfc,0xdb,0x8a,0x4b,0xcc,0x85,0x01,
0x23,0xd2,0xe5,0xf2,0x29,0xa7,0x45,0x94,0x4a,0x62,0xe3,0xa5,0x6f,0x3f,0xe1,0x7a,
0x64,0xed,0x5c,0x99,0x29,0x87,0xa8,0x78,0x59,0x0d,0xaa,0x0f,0x25,0x0a,0x5c,0x58,
0xfb,0x00,0xa7,0xa8,0x8a,0x1d,0x86,0x80,0xc5,0x1f,0xd2,0x28,0x69,0x71,0x58,0xc3,
0x51,0x90,0xe1,0xf8,0x6a,0xf3,0x8f,0xb0,0x68,0xdf,0x95,0x40,0x5c,0xe4,0x24,0x6b,
0x29,0x19,0x71,0x3f,0x42,0x63,0x6c,0x48,0xe7,0xad,0xa8,0x4b,0x91,0x8f,0x42,0x36,
0x34,0xe7,0x32,0x55,0x59,0x2d,0x36,0x38,0x38,0x59,0x9b,0x08,0x16,0x4d,0x8d,0xf8,
0x0a,0xa4,0x52,0x01,0xbb,0x52,0xa9,0xfd,0x40,0x18,0x97,0x37,0xff,0xc9,0x82,0x27,
0xb2,0x64,0x60,0xce,0x00,0xd9,0x04,0xf0,0x9e,0x99,0xbd,0xce,0x8f,0x90,0x4a,0xdd,
0xe1,0xec,0x19,0x14,0xb1,0xfb,0xca,0x1e,0x98,0x0f,0xd4,0xcb,0x80,0xd6,0x05,0x63,
0xfd,0xa0,0x74,0xa6,0x86,0xf6,0x19,0x98,0x76,0x27,0x68,0xf7,0xe9,0x09,0x9a,0xf2,
0x2e,0x42,0xe1,0xbe,0x64,0x48,0x2a,0x74,0x30,0xbb,0x07,0xcc,0x1f,0xd4,0x91,0x9d,
0xac,0x55,0x53,0x25,0xb9,0x64,0xf7,0x58,0x4c,0x34,0x16,0xbc,0xf6,0x12,0x2b,0x65,
0x68,0x25,0x2e,0x29,0x1f,0xbb,0xb9,0xee,0x6d,0x0c,0x8e,0xbb,0xd2,0x5f,0x1d,0x8f,
0xc1,0x39,0xf9,0x8d,0xc0,0x39,0x75,0xcf,0x25,0x17,0xbe,0x96,0xaf,0x98,0x9f,0x5f,
0x65,0x15,0xc4,0x62,0xf8,0x55,0xfc,0xab,0x54,0xcf,0xdc,0x14,0x06,0xc8,0xfc,0x42,
0xd3,0xf0,0xad,0x10,0x08,0xcd,0xd4,0x11,0xbb,0xca,0x67,0xc6,0x48,0x5f,0x9d,0x59,
0xe3,0xe8,0x53,0x67,0x27,0x2d,0x34,0x9e,0x9e,0x24,0x29,0xdb,0x69,0x99,0x86,0xf9,
0x20,0xb5,0xbb,0x5b,0xb0,0xf9,0xc3,0x67,0xad,0x1c,0x9c,0xf7,0xcc,0xef,0xce,0x69,
0xe0,0x26,0x8f,0x79,0xbd,0xca,0x10,0x17,0xda,0xa9,0x88,0x57,0x9b,0x15,0x24,0xba,
0x84,0xd0,0xeb,0x4d,0x14,0xf5,0xfc,0xe6,0x51,0x6c,0x6f,0x64,0x6b,0x73,0xec,0x85,
0xf1,0x6f,0xe1,0x67,0x25,0x10,0x77,0x32,0x9e,0x85,0x6e,0x69,0xb1,0x83,0x00,0xe4,
0x13,0xa4,0x45,0x34,0x3b,0x40,0xff,0x41,0x82,0x89,0x79,0x57,0xfd,0xd2,0x8e,0xe8,
0xfc,0x1d,0x19,0x21,0x12,0x00,0xd7,0x66,0xe5,0xc7,0x10,0x1d,0xcb,0x75,0xe8,0xfa,
0xb6,0xee,0x7b,0x2f,0x1a,0x25,0x24,0xb9,0x9f,0x1d,0x78,0xfb,0x84,0xd0,0x17,0x05,
0x71,0xb3,0xc8,0x18,0xff,0x62,0xee,0xed,0x53,0xab,0x78,0xd3,0x65,0x2d,0xbb,0xc7,
0xc1,0xe7,0x70,0xa2,0x43,0x2c,0x7c,0xc7,0x16,0x04,0xd2,0x45,0xd5,0x6b,0x6c,0x7a,
0x5e,0xa1,0x50,0x2e,0x31,0x5b,0xcc,0xe8,0x65,0x8b,0x16,0x85,0xbf,0x82,0x83,0xfb,
0xde,0x9f,0x36,0x48,0x32,0x79,0xd6,0x9b,0xfb,0x52,0x45,0xbf,0x43,0xf7,0x0b,0x0b,
0x19,0x19,0x31,0xc3,0x85,0xec,0x1d,0x8c,0x20,0xf0,0x3a,0xfa,0x80,0x4d,0x2c,0x7d,
0xac,0x60,0x09,0xc0,0x40,0xee,0xb9,0xeb,0x13,0x5b,0xe8,0x2b,0xb1,0x20,0xf0,0xce,
0x4c,0xbd,0xc6,0x04,0x86,0x70,0xc6,0x33,0xc3,0x15,0x0f,0x65,0x19,0xfd,0xc2,0xd3,

// map checksum goes here
0x00,0x00,0x00,0x00
};

#if 0
static byte chkbuf[16 + 60 + 4];

static unsigned last_mapchecksum = 0;

/*
====================
COM_BlockSequenceCheckByte

For proxy protecting
====================
*/
byte
COM_BlockSequenceCheckByte ( byte *base, int length, int sequence, unsigned mapchecksum )
{
	int		checksum;
	byte	*p;

	if (last_mapchecksum != mapchecksum) {
		last_mapchecksum = mapchecksum;
		chktbl[1024] = (mapchecksum & 0xff000000) >> 24;
		chktbl[1025] = (mapchecksum & 0x00ff0000) >> 16;
		chktbl[1026] = (mapchecksum & 0x0000ff00) >> 8;
		chktbl[1027] = (mapchecksum & 0x000000ff);

		Com_BlockFullChecksum (chktbl, sizeof(chktbl), chkbuf);
	}

	p = chktbl + (sequence % (sizeof(chktbl) - 8));

	if (length > 60)
		length = 60;
	memcpy (chkbuf + 16, base, length);

	length += 16;

	chkbuf[length] = (sequence & 0xff) ^ p[0];
	chkbuf[length+1] = p[1];
	chkbuf[length+2] = ((sequence>>8) & 0xff) ^ p[2];
	chkbuf[length+3] = p[3];

	length += 4;

	checksum = LittleLong(Com_BlockChecksum (chkbuf, length));

	checksum &= 0xff;

	return checksum;
}
#endif

/*
====================
COM_BlockSequenceCRCByte

For proxy protecting
====================
*/
byte
COM_BlockSequenceCRCByte ( byte *base, int length, int sequence )
{
	unsigned short crc;
	byte	*p;
	byte chkb[60 + 4];

	p = chktbl + (sequence % (sizeof(chktbl) - 8));

	if (length > 60)
		length = 60;
	memcpy (chkb, base, length);

	chkb[length] = (sequence & 0xff) ^ p[0];
	chkb[length+1] = p[1];
	chkb[length+2] = ((sequence>>8) & 0xff) ^ p[2];
	chkb[length+3] = p[3];

	length += 4;

	crc = CRC_Block(chkb, length);

	crc &= 0xff;

	return crc;
}
