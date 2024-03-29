/*
	keys.c

	(description)

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

#include <quakedef.h>
#include <qtypes.h>
#include <keys.h>
#include <cmd.h>
#include <cvar.h>
#include <client.h>
#include <lib_replace.h>
#include <console.h>
#include <menu.h>
#include <screen.h>
#include <sys.h>

#ifdef _WIN32
#include <windows.h>
#endif

/*

key up events are sent even if in console mode

*/

cvar_t	*cl_constyle;

#define		MAXCMDLINE	256
char	key_lines[32][MAXCMDLINE];
int		key_linepos;
int		key_lastpress;

int		edit_line=0;
int		history_line=0;

keydest_t	key_dest;

int		key_count;			// incremented every key event

char	*keybindings[256];
qboolean	consolekeys[256];	// if true, can't be rebound while in console
qboolean	menubound[256];	// if true, can't be rebound while in menu
int		keyshift[256];		// key to map to if shift held down in console
int		key_repeats[256];	// if > 1, it is autorepeating
qboolean	keydown[256];

typedef struct
{
	char	*name;
	int		keynum;
} keyname_t;

keyname_t keynames[] =
{
	{"TAB", K_TAB},
	{"ENTER", K_ENTER},
	{"ESCAPE", K_ESCAPE},
	{"SPACE", K_SPACE},
	{"BACKSPACE", K_BACKSPACE},

	{"CAPSLOCK",K_CAPSLOCK},
	{"PRINTSCR", K_PRNTSCR},
	{"SCRLCK", K_SCRLCK},
	{"PAUSE", K_PAUSE},

	{"UPARROW", K_UPARROW},
	{"DOWNARROW", K_DOWNARROW},
	{"LEFTARROW", K_LEFTARROW},
	{"RIGHTARROW", K_RIGHTARROW},

	{"ALT", K_ALT},
	{"CTRL", K_CTRL},
	{"SHIFT", K_SHIFT},

	// Keypad stuff..
	// These are duplicated, Tonik DIES if he breaks configs again.
	{"NUMLOCK", KP_NUMLCK},
	{"KP_SLASH", KP_DIVIDE},
	{"KP_STAR", KP_MULTIPLY},

	{"KP_NUMLCK", KP_NUMLCK},
	{"KP_NUMLOCK", KP_NUMLCK},
	{"KP_DIVIDE", KP_DIVIDE},
	{"KP_MULTIPLY", KP_MULTIPLY},

	{"KP_MINUS", KP_MINUS},

	{"KP_HOME", KP_HOME},
	{"KP_UPARROW", KP_UPARROW},
	{"KP_PGUP", KP_PGUP},
	{"KP_PLUS", KP_PLUS},

	{"KP_LEFTARROW", KP_LEFTARROW},
	{"KP_5", KP_5},
	{"KP_RIGHTARROW", KP_RIGHTARROW},

	{"KP_END", KP_END},
	{"KP_DOWNARROW", KP_DOWNARROW},
	{"KP_PGDN", KP_PGDN},

	{"KP_INS", KP_INS},
	{"KP_DEL", KP_DEL},
	{"KP_ENTER", KP_ENTER},


	{"F1", K_F1},
	{"F2", K_F2},
	{"F3", K_F3},
	{"F4", K_F4},
	{"F5", K_F5},
	{"F6", K_F6},
	{"F7", K_F7},
	{"F8", K_F8},
	{"F9", K_F9},
	{"F10", K_F10},
	{"F11", K_F11},
	{"F12", K_F12},

	{"INS", K_INS},
	{"DEL", K_DEL},
	{"PGDN", K_PGDN},
	{"PGUP", K_PGUP},
	{"HOME", K_HOME},
	{"END", K_END},

	{"MOUSE1", K_MOUSE1},
	{"MOUSE2", K_MOUSE2},
	{"MOUSE3", K_MOUSE3},

	{"JOY1", K_JOY1},
	{"JOY2", K_JOY2},
	{"JOY3", K_JOY3},
	{"JOY4", K_JOY4},

	{"AUX1", K_AUX1},
	{"AUX2", K_AUX2},
	{"AUX3", K_AUX3},
	{"AUX4", K_AUX4},
	{"AUX5", K_AUX5},
	{"AUX6", K_AUX6},
	{"AUX7", K_AUX7},
	{"AUX8", K_AUX8},
	{"AUX9", K_AUX9},
	{"AUX10", K_AUX10},
	{"AUX11", K_AUX11},
	{"AUX12", K_AUX12},
	{"AUX13", K_AUX13},
	{"AUX14", K_AUX14},
	{"AUX15", K_AUX15},
	{"AUX16", K_AUX16},
	{"AUX17", K_AUX17},
	{"AUX18", K_AUX18},
	{"AUX19", K_AUX19},
	{"AUX20", K_AUX20},
	{"AUX21", K_AUX21},
	{"AUX22", K_AUX22},
	{"AUX23", K_AUX23},
	{"AUX24", K_AUX24},
	{"AUX25", K_AUX25},
	{"AUX26", K_AUX26},
	{"AUX27", K_AUX27},
	{"AUX28", K_AUX28},
	{"AUX29", K_AUX29},
	{"AUX30", K_AUX30},
	{"AUX31", K_AUX31},
	{"AUX32", K_AUX32},

	{"MWHEELUP", K_MWHEELUP},
	{"MWHEELDOWN", K_MWHEELDOWN},

	{"SEMICOLON", ';'},	// because a raw semicolon seperates commands

	{NULL,0}
};

/*
==============================================================================

			LINE TYPING INTO THE CONSOLE

==============================================================================
*/

qboolean
CheckForCommand ( void )
{
	char	command[128];
	char	*cmd, *s;
	int		i;

	s = key_lines[edit_line]+1;

	for (i=0 ; i<127 ; i++)
		if (s[i] <= ' ')
			break;
		else
			command[i] = s[i];
	command[i] = 0;

	cmd = Cmd_CompleteCommand (command);
	if (!cmd || strcmp (cmd, command))
		cmd = Cvar_CompleteVariable (command);
	if (!cmd || strcmp (cmd, command) )
		return false;		// just a chat message
	return true;
}

void
CompleteCommand ( void )
{
	char	*cmd, *s;

	s = key_lines[edit_line]+1;
	if (*s == '\\' || *s == '/')
		s++;

	cmd = Cmd_CompleteCommand (s);
	if (!cmd)
		cmd = Cvar_CompleteVariable (s);
	if (cmd)
	{
		key_lines[edit_line][1] = '/';
		Q_strcpy (key_lines[edit_line]+2, cmd);
		key_linepos = Q_strlen(cmd)+2;
		key_lines[edit_line][key_linepos] = ' ';
		key_linepos++;
		key_lines[edit_line][key_linepos] = 0;
		return;
	}
}

/*
====================
Key_Console

Interactive line editing and console scrollback
====================
*/
void
Key_Console ( int key )
{
	int		i;
#ifdef _WIN32
	char	*cmd, *s;
	HANDLE	th;
	char	*clipText, *textCopied;
#endif

	switch (key) {
		case KP_ENTER:
		case K_ENTER:
			// backslash text are commands
			if (key_lines[edit_line][1] == '/' && key_lines[edit_line][2] == '/')
				goto no_lf;
			else if (key_lines[edit_line][1] == '\\' || key_lines[edit_line][1] == '/')
				Cbuf_AddText (key_lines[edit_line]+2);	// skip the ]/
			else if (cl_constyle->value != 2 && CheckForCommand())
				Cbuf_AddText (key_lines[edit_line]+1);	// valid command
			else if ((cls.state >= ca_connected && cl_constyle->value == 1) || cl_constyle->value == 2)
			{
				if (cls.state < ca_connected)	// can happen if cl_constyle == 2
					goto no_lf;					// drop the whole line

				// convert to a chat message
				Cbuf_AddText ("say ");
				Cbuf_AddText (key_lines[edit_line]+1);
			}
			else
				Cbuf_AddText (key_lines[edit_line]+1);	// skip the ]

			Cbuf_AddText ("\n");
no_lf:
			Con_Printf ("%s\n",key_lines[edit_line]);
			edit_line = (edit_line + 1) & 31;
			history_line = edit_line;
			key_lines[edit_line][0] = ']';
			key_lines[edit_line][1] = 0;
			key_linepos = 1;
			if (cls.state == ca_disconnected)
				SCR_UpdateScreen ();	// force an update, because the command
										// may take some time
			return;

		case K_TAB:
			// command completion
			CompleteCommand ();
			return;

		case K_BACKSPACE:
			if (key_linepos > 1)
			{
				strcpy(key_lines[edit_line] + key_linepos - 1, key_lines[edit_line] + key_linepos);
				key_linepos--;
			}
			return;

		case KP_DEL:
		case K_DEL:
			if (key_linepos < strlen(key_lines[edit_line]))
				strcpy(key_lines[edit_line] + key_linepos, key_lines[edit_line] + key_linepos + 1);
			return;

		case KP_RIGHTARROW:
		case K_RIGHTARROW:
			if (key_linepos < strlen(key_lines[edit_line]))
				key_linepos++;
			return;

		case KP_LEFTARROW:
		case K_LEFTARROW:
			if (key_linepos > 1)
				key_linepos--;
			return;

		case KP_UPARROW:
		case K_UPARROW:
			do {
				history_line = (history_line - 1) & 31;
			} while (history_line != edit_line
					&& !key_lines[history_line][1]);
			if (history_line == edit_line)
				history_line = (edit_line+1)&31;
			Q_strcpy(key_lines[edit_line], key_lines[history_line]);
			key_linepos = Q_strlen(key_lines[edit_line]);
			return;

		case KP_DOWNARROW:
		case K_DOWNARROW:
			if (history_line == edit_line) return;
			do {
				history_line = (history_line + 1) & 31;
			} while (history_line != edit_line
				&& !key_lines[history_line][1]);

			if (history_line == edit_line) {
				key_lines[edit_line][0] = ']';
				key_lines[edit_line][1] = 0;
				key_linepos = 1;
			} else {
				Q_strcpy(key_lines[edit_line], key_lines[history_line]);
				key_linepos = Q_strlen(key_lines[edit_line]);
			}
			return;

		case K_MWHEELUP:
		case KP_PGUP:
		case K_PGUP:
			if (con->display - con->current + con->numlines > 2)
				con->display -= 2;
			return;

		case K_MWHEELDOWN:
		case KP_PGDN:
		case K_PGDN:
			con->display += 2;
			if (con->display > con->current)
				con->display = con->current;
			return;

		case KP_HOME:
		case K_HOME:
			if (keydown[K_CTRL])
			{
				if (con->numlines > 10)
					con->display = con->current - con->numlines + 10;
			}
			else
				key_linepos = 1;
			return;

		case KP_END:
		case K_END:
			if (keydown[K_CTRL])
				con->display = con->current;
			else
				key_linepos = strlen(key_lines[edit_line]);
			return;
	}
#ifdef _WIN32
	if ((key=='V' || key=='v') && GetKeyState(VK_CONTROL)<0) {
		if (OpenClipboard(NULL)) {
			th = GetClipboardData(CF_TEXT);
			if (th) {
				clipText = GlobalLock(th);
				if (clipText) {
					textCopied = malloc(GlobalSize(th)+1);
					strcpy(textCopied, clipText);
	/* Substitutes a NULL for every token */strtok(textCopied, "\n\r\b");
					i = strlen(textCopied);
					if (i + strlen(key_lines[edit_line]) >= MAXCMDLINE-1)
						i = MAXCMDLINE-1 - strlen(key_lines[edit_line]);
					if (i > 0)
					{	// insert the string
						memmove (key_lines[edit_line] + key_linepos + i,
							key_lines[edit_line] + key_linepos, strlen(key_lines[edit_line]) - key_linepos + 1);
						memcpy (key_lines[edit_line] + key_linepos, textCopied, i);
						key_linepos += i;
					}
					free(textCopied);
				}
				GlobalUnlock(th);
			}
			CloseClipboard();
		return;
		}
	}
#endif

	if (key < 32 || key > 127)
		return;	// non printable

	i = strlen(key_lines[edit_line]);
	if (i >= MAXCMDLINE-1)
		return;

	// This also moves the ending \0
	memmove (key_lines[edit_line]+key_linepos+1, key_lines[edit_line]+key_linepos, i-key_linepos+1);
	key_lines[edit_line][key_linepos] = key;
	key_linepos++;
}

//============================================================================

qboolean	chat_team;
char		chat_buffer[MAXCMDLINE];
int			chat_bufferlen = 0;

void
Key_Message ( int key )
{

	if (key == K_ENTER || key == KP_ENTER)
	{
		if (chat_team)
			Cbuf_AddText ("say_team \"");
		else
			Cbuf_AddText ("say \"");
		Cbuf_AddText(chat_buffer);
		Cbuf_AddText("\"\n");

		key_dest = key_game;
		chat_bufferlen = 0;
		chat_buffer[0] = 0;
		return;
	}

	if (key == K_ESCAPE)
	{
		key_dest = key_game;
		chat_bufferlen = 0;
		chat_buffer[0] = 0;
		return;
	}

	if (key < 32 || key > 127)
		return;	// non printable

	if (key == K_BACKSPACE)
	{
		if (chat_bufferlen)
		{
			chat_bufferlen--;
			chat_buffer[chat_bufferlen] = 0;
		}
		return;
	}

	if (chat_bufferlen == sizeof(chat_buffer)-1)
		return;		// all full

	chat_buffer[chat_bufferlen++] = key;
	chat_buffer[chat_bufferlen] = 0;
}

//============================================================================


/*
===================
Key_StringToKeynum

Returns a key number to be used to index keybindings[] by looking at
the given string.  Single ascii characters return themselves, while
the K_* names are matched up.
===================
*/
int
Key_StringToKeynum ( char *str )
{
	keyname_t	*kn;

	if (!str || !str[0])
		return -1;
	if (!str[1])
		return str[0];

	for (kn=keynames ; kn->name ; kn++)
	{
		if (!Q_strcasecmp(str,kn->name))
			return kn->keynum;
	}
	return -1;
}

/*
===================
Key_KeynumToString

Returns a string (either a single ascii char, or a K_* name) for the
given keynum.
FIXME: handle quote special (general escape sequence?)
===================
*/
char *
Key_KeynumToString ( int keynum )
{
	keyname_t	*kn;
	static	char	tinystr[2];

	if (keynum == -1)
		return "<KEY NOT FOUND>";
	if (keynum > 32 && keynum < 127)
	{	// printable ascii
		tinystr[0] = keynum;
		tinystr[1] = 0;
		return tinystr;
	}

	for (kn=keynames ; kn->name ; kn++)
		if (keynum == kn->keynum)
			return kn->name;

	return "<UNKNOWN KEYNUM>";
}


/*
===================
Key_SetBinding
===================
*/
void
Key_SetBinding ( int keynum, char *binding )
{
	char	*new;
	int		l;

	if (keynum == -1)
		return;

// free old bindings
	if (keybindings[keynum])
	{
		if (!Q_strncmp (keybindings[keynum], "toggleconsole", 13)) {
				consolekeys[keynum] = true;
		}
		Z_Free (keybindings[keynum]);
		keybindings[keynum] = NULL;
	}

// allocate memory for new binding
	l = Q_strlen (binding);
	new = Z_Malloc (l+1);
	Q_strcpy (new, binding);
	new[l] = 0;
	keybindings[keynum] = new;

	if (!Q_strncmp (new, "toggleconsole", 13)) {
			consolekeys[keynum] = false;
	}
}

/*
===================
Key_Unbind_f
===================
*/
void
Key_Unbind_f ( void )
{
	int		b;

	if (Cmd_Argc() != 2)
	{
		Con_Printf ("unbind <key> : remove commands from a key\n");
		return;
	}

	b = Key_StringToKeynum (Cmd_Argv(1));
	if (b==-1)
	{
		Con_Printf ("\"%s\" isn't a valid key\n", Cmd_Argv(1));
		return;
	}

	Key_SetBinding (b, "");
}

void
Key_Unbindall_f ( void )
{
	int		i;

	for (i=0 ; i<256 ; i++)
		if (keybindings[i])
			Key_SetBinding (i, "");
}


/*
===================
Key_Bind_f
===================
*/
void
Key_Bind_f ( void )
{
	int			i, c, b;
	char		cmd[1024];

	c = Cmd_Argc();

	if (c != 2 && c != 3)
	{
		Con_Printf ("bind <key> [command] : attach a command to a key\n");
		return;
	}
	b = Key_StringToKeynum (Cmd_Argv(1));
	if (b==-1)
	{
		Con_Printf ("\"%s\" isn't a valid key\n", Cmd_Argv(1));
		return;
	}

	if (c == 2)
	{
		if (keybindings[b])
			Con_Printf ("\"%s\" = \"%s\"\n", Cmd_Argv(1), keybindings[b] );
		else
			Con_Printf ("\"%s\" is not bound\n", Cmd_Argv(1) );
		return;
	}

// copy the rest of the command line
	cmd[0] = 0;		// start out with a null string
	for (i=2 ; i< c ; i++)
	{
		strcat (cmd, Cmd_Argv(i));
		if (i != (c-1))
			strcat (cmd, " ");
	}

	Key_SetBinding (b, cmd);
}

/*
============
Key_WriteBindings

Writes lines containing "bind key value"
============
*/
void
Key_WriteBindings ( QFile *f )
{
	int		i;

	for (i=0 ; i<256 ; i++)
		if (keybindings[i])
			Qprintf (f, "bind \"%s\" \"%s\"\n", Key_KeynumToString(i), keybindings[i]);	// 1999-12-26 bound keys not saved in quotes fix by Maddes
}


/*
===================
Key_Init
===================
*/
void
Key_Init ( void )
{
	int		i;

	for (i=0 ; i<32 ; i++)
	{
		key_lines[i][0] = ']';
		key_lines[i][1] = 0;
	}
	key_linepos = 1;

//
// init ascii characters in console mode
//
	for (i=32 ; i<128 ; i++)
		consolekeys[i] = true;
	consolekeys[KP_ENTER] = true;
	consolekeys[KP_LEFTARROW] = true;
	consolekeys[KP_RIGHTARROW] = true;
	consolekeys[KP_UPARROW] = true;
	consolekeys[KP_DOWNARROW] = true;
	consolekeys[KP_HOME] = true;
	consolekeys[KP_END] = true;
	consolekeys[KP_PGUP] = true;
	consolekeys[KP_PGDN] = true;
	consolekeys[KP_DEL] = true;

	consolekeys[K_ENTER] = true;
	consolekeys[K_TAB] = true;
	consolekeys[K_LEFTARROW] = true;
	consolekeys[K_RIGHTARROW] = true;
	consolekeys[K_UPARROW] = true;
	consolekeys[K_DOWNARROW] = true;
	consolekeys[K_BACKSPACE] = true;
	consolekeys[K_DEL] = true;
	consolekeys[K_HOME] = true;
	consolekeys[K_END] = true;
	consolekeys[K_PGUP] = true;
	consolekeys[K_PGDN] = true;
	consolekeys[K_CTRL] = true;
	consolekeys[K_SHIFT] = true;
	consolekeys[K_MWHEELUP] = true;
	consolekeys[K_MWHEELDOWN] = true;
	consolekeys['`'] = false;
	consolekeys['~'] = false;

	for (i=0 ; i<256 ; i++)
		keyshift[i] = i;
	for (i='a' ; i<='z' ; i++)
		keyshift[i] = i - 'a' + 'A';
	keyshift['1'] = '!';
	keyshift['2'] = '@';
	keyshift['3'] = '#';
	keyshift['4'] = '$';
	keyshift['5'] = '%';
	keyshift['6'] = '^';
	keyshift['7'] = '&';
	keyshift['8'] = '*';
	keyshift['9'] = '(';
	keyshift['0'] = ')';
	keyshift['-'] = '_';
	keyshift['='] = '+';
	keyshift[','] = '<';
	keyshift['.'] = '>';
	keyshift['/'] = '?';
	keyshift[';'] = ':';
	keyshift['\''] = '"';
	keyshift['['] = '{';
	keyshift[']'] = '}';
	keyshift['`'] = '~';
	keyshift['\\'] = '|';

	menubound[K_ESCAPE] = true;
	for (i=0 ; i<12 ; i++)
		menubound[K_F1+i] = true;

//
// register our functions
//
	Cmd_AddCommand ("bind",Key_Bind_f);
	Cmd_AddCommand ("unbind",Key_Unbind_f);
	Cmd_AddCommand ("unbindall",Key_Unbindall_f);

	cl_constyle = Cvar_Get ("cl_constyle", "1", 0,
		"0 - NQ, 1 - QW, 2 - Q3");
}

/*
===================
Key_Event

Called by the system between frames for both key up and key down events
Should NOT be called during an interrupt!
===================
*/
void
Key_Event ( int key, qboolean down )
{
	char	*kb;
	char	cmd[1024];

//	Con_Printf ("%i : %i\n", key, down);	//@@@

	keydown[key] = down;

	if (!down)
		key_repeats[key] = 0;

	key_lastpress = key;
	key_count++;
	if (key_count <= 0)
	{
		return;		// just catching keys for Con_NotifyBox
	}

// update auto-repeat status
	if (down)
	{
		key_repeats[key]++;
		if (key != K_BACKSPACE && key != K_DEL
			&& key != K_LEFTARROW && key != K_RIGHTARROW
			&& key != K_PGUP && key != K_PGDN
			&& key_repeats[key] > 1)
			return;	// ignore most autorepeats

		if (key >= 200 && !keybindings[key])
			Con_Printf ("%s is unbound, hit F4 to set.\n", Key_KeynumToString (key) );
	}

	if (key_dest == key_message && cls.state != ca_active)
		key_dest = key_console;

//
// handle escape specialy, so the user can never unbind it
//
	if (key == K_ESCAPE)
	{
		if (!down)
			return;
		switch (key_dest)
		{
			case key_message:
				Key_Message (key);
				break;
			case key_menu:
				M_Keydown (key);
				break;
			case key_game:
			case key_console:
				M_ToggleMenu_f ();
				break;
			default:
				Sys_Error ("Bad key_dest");
		}
		return;
	}

//
// key up events only generate commands if the game key binding is
// a button command (leading + sign).  These will occur even in console mode,
// to keep the character from continuing an action started before a console
// switch.  Button commands include the kenum as a parameter, so multiple
// downs can be matched with ups
//
	if (!down)
	{
		kb = keybindings[key];
		if (kb && kb[0] == '+')
		{
			snprintf(cmd, sizeof(cmd), "-%s %i\n", kb+1, key);
			Cbuf_AddText (cmd);
		}
		if (keyshift[key] != key)
		{
			kb = keybindings[keyshift[key]];
			if (kb && kb[0] == '+')
			{
				snprintf(cmd, sizeof(cmd), "-%s %i\n", kb+1, key);
				Cbuf_AddText (cmd);
			}
		}
		return;
	}

//
// during demo playback, most keys bring up the main menu
//
	if (cls.demoplayback && down && consolekeys[key] && key_dest == key_game
	&& key != K_CTRL && key != K_DEL && key != K_HOME && key != K_END && key != K_TAB)
	{
		M_ToggleMenu_f ();
		return;
	}

//
// if not a consolekey, send to the interpreter no matter what mode is
//
	if ( (key_dest == key_menu && menubound[key])
	|| (key_dest == key_console && !consolekeys[key])
	|| (key_dest == key_game && (
		(cls.state == ca_active)
		|| !consolekeys[key] ) ) )
	{
		kb = keybindings[key];
		if (kb)
		{
			if (kb[0] == '+')
			{	// button commands add keynum as a parm
				snprintf(cmd, sizeof(cmd), "%s %i\n", kb, key);
				Cbuf_AddText (cmd);
			}
			else
			{
				Cbuf_AddText (kb);
				Cbuf_AddText ("\n");
			}
		}
		return;
	}

	if (!down)
		return;		// other systems only care about key down events

	if (keydown[K_SHIFT])
		key = keyshift[key];

	switch (key_dest)
	{
	case key_message:
		Key_Message (key);
		break;
	case key_menu:
		M_Keydown (key);
		break;

	case key_game:
	case key_console:
		Key_Console (key);
		break;
	default:
		Sys_Error ("Bad key_dest");
	}
}

/*
===================
Key_ClearStates
===================
*/
void
Key_ClearStates ( void )
{
	int		i;

	for (i=0 ; i<256 ; i++)
	{
		keydown[i] = false;
		key_repeats[i] = false;
	}
}
