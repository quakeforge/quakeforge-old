/*
Copyright (C) 1996-1997  Id Software, Inc.
Copyright (C) 1999-2000  contributors of the QuakeForge project
Copyright (C) 1999-2000  Nelson Rush.

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

#include "qtypes.h"
#include "quakedef.h"
#include "keys.h"
#include "client.h"
#include "sys.h"
#include "console.h"
#include "cvar.h"
#include <cmd.h>

#include <stdio.h>
#include <stdlib.h>
#include <aalib.h>
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif

void (*kbd_handler) (int scancode, int press) = NULL;
static int scantokey[128];
unsigned char scanpressed[128]={0,};

static unsigned char scantokey[128];

static void IN_init_kb();

#if 0
static void
vtswitch(int newconsole)
{
	int fd;
	struct vt_stat x;

	/* switch consoles and wait until reactivated */
	fd = open("/dev/console", O_RDONLY);
	ioctl(fd, VT_GETSTATE, &x);
	ioctl(fd, VT_ACTIVATE, newconsole);
	ioctl(fd, VT_WAITACTIVE, x.v_active);
	close(fd);
}
#endif


static void
keyhandler(int scancode, int state)
{
	int sc;

	sc = scancode & 0x7f;
#if 0
	Con_Printf("scancode=%x (%d%s)\n", scancode, sc, scancode&0x80?"+128":"");
#endif
	Key_Event(scantokey[sc], state == KEY_EVENTPRESS);
}

void
Force_CenterView_f(void)
{
	cl.viewangles[PITCH] = 0;
}


void IN_Init(void)
{
	IN_init_kb();
}

static void IN_init_kb()
{
	int i;

	if (!context)
		{
		fprintf(stderr, "Video mode not set?\n");
		Sys_Error("keyboard_init() failed");
		}
		
	for (i = 0; i < 128; i++)
		scantokey[i] = ' ';

	scantokey[59] = AA_UNKNOWN;		/* F1 */ // row 0

	scantokey[60] = AA_UNKNOWN;
	scantokey[61] = AA_UNKNOWN;
	scantokey[62] = AA_UNKNOWN;
	scantokey[63] = AA_UNKNOWN;
	scantokey[64] = AA_UNKNOWN;
	scantokey[65] = AA_UNKNOWN;
	scantokey[66] = AA_UNKNOWN;
	scantokey[67] = AA_UNKNOWN;
	scantokey[68] = AA_UNKNOWN;
	scantokey[87] = AA_UNKNOWN;
	scantokey[88] = AA_UNKNOWN;		/* F12 */

	scantokey[1] = AA_UNKNOWN; /* escape */ 	// row 1

	scantokey[2] = '1';
	scantokey[3] = '2';
	scantokey[4] = '3';
	scantokey[5] = '4';
	scantokey[6] = '5';
	scantokey[7] = '6';
	scantokey[8] = '7';
	scantokey[9] = '8';
	scantokey[10] = '9';
	scantokey[11] = '0';
	scantokey[12] = '-';
	scantokey[13] = '=';
	scantokey[14] = AA_BACKSPACE;

	scantokey[15] = '\t';	// row 2

	scantokey[16] = 'q';
	scantokey[17] = 'w';
	scantokey[18] = 'e';
	scantokey[19] = 'r';
	scantokey[20] = 't';
	scantokey[21] = 'y';
	scantokey[22] = 'u';
	scantokey[23] = 'i';
	scantokey[24] = 'o';
	scantokey[25] = 'p';
	scantokey[26] = '[';
	scantokey[27] = ']';
	scantokey[28] = '\r';		/* ENTER */

	scantokey[30] = 'a';	// row 3

	scantokey[31] = 's';
	scantokey[32] = 'd';
	scantokey[33] = 'f';
	scantokey[34] = 'g';
	scantokey[35] = 'h';
	scantokey[36] = 'j';
	scantokey[37] = 'k';
	scantokey[38] = 'l';
	scantokey[39] = ';';
	scantokey[40] = '\'';
	scantokey[41] = '`';

	scantokey[42] = AA_UNKNOWN;	/* shift */ // row 4

	scantokey[43] = '\\';
	scantokey[44] = 'z';
	scantokey[45] = 'x';
	scantokey[46] = 'c';
	scantokey[47] = 'v';
	scantokey[48] = 'b';
	scantokey[49] = 'n';
	scantokey[50] = 'm';
	scantokey[51] = ',';
	scantokey[52] = '.';
	scantokey[53] = '/';
	scantokey[54] = AA_UNKNOWN;	/* shift */

	scantokey[29] = AA_UNKNOWN;	// row 5 /* ctrl */

	scantokey[56] = AA_UNKNOWN;	/* alt */
	scantokey[57] = ' ';
	scantokey[100] = AA_UNKNOWN;	/* alt */
	scantokey[97] = AA_UNKNOWN;	/* ctrl */


	scantokey[98] = '/';
	scantokey[55] = '8';
	scantokey[74] = '-';
	scantokey[71] = AA_UNKNOWN;	/* home */
	scantokey[72] = AA_UP;
	scantokey[73] = AA_UNKNOWN;	/* pgup */
	scantokey[75] = AA_LEFT;
	scantokey[76] = '5';
	scantokey[77] = AA_RIGHT;
	scantokey[78] = '+';
	scantokey[79] = AA_UNKNOWN;	/* end */
	scantokey[80] = AA_DOWN;
	scantokey[81] = AA_UNKNOWN;	/* pgdn */
	scantokey[82] = AA_UNKNOWN;	/* ins */
	scantokey[83] = AA_UNKNOWN;	/* del */
	scantokey[96] = '\n';

	scantokey[103] = AA_UP;		// arrow pad

	scantokey[108] = AA_DOWN;
	scantokey[105] = AA_LEFT;
	scantokey[106] = AA_RIGHT;

	scantokey[119] = AA_UNKNOWN; /* pause */	// misc

	if (!aa_autoinitkbd(context, AA_SENDRELEASE))
		{ 
		fprintf(stderr, "Error in aa_autoinitkbd!\n");
		Sys_Error("keyboard_init() failed");
		}

	kbd_handler = keyhandler;

}


void
IN_Shutdown(void)
{
	aa_uninitkbd(context);
}


int keyboard_update()
{
	int i,stat=1,scan;
	if ((i=aa_getevent(context,0))==AA_NONE)
		return 0;
	if (i>=AA_UNKNOWN && i<AA_RELEASE) {
		fprintf(stderr,"key: %d pressed\n");
		return 1;
	}
	if (i>=AA_RELEASE) {
		stat=0;
		i&=~AA_RELEASE;
	}
	for (scan=0;scan<128 && scantokey[scan]!=i;scan++) ;
	if (i==' ') scan=57;
/*	if (scan==128) {
		fprintf(stderr,"key: %d ('%c') %s.%d\n",i,i,(stat?"pressed":"released"),scan);
		return 1;
	} */
	kbd_handler(scan,stat);
	for (i=0;i<128;i++)
		if (scanpressed[i]) {
			kbd_handler(i,0);
			scanpressed[i]=0;
		}
	scanpressed[scan]=1;
	return 1;
}


void
Sys_SendKeyEvents(void)
{
	while (keyboard_update());
}


