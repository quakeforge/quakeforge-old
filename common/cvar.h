/*
	cvar.h

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

#ifndef _CVAR_H
#define _CVAR_H

#include <qtypes.h>
#include <quakeio.h>

typedef struct cvar_s
{
	char    *name;
	char    *string;
	int	flags;
	char 	*description;	// for "help" command
	float	value;
	struct cvar_s *next;
} cvar_t;

typedef struct cvar_alias_s
{
	char	*name;
	cvar_t	*cvar;
	struct cvar_alias_s	*next;
} cvar_alias_t;

#define CVAR_NONE		0
#define	CVAR_ARCHIVE		1	// set to cause it to be saved to vars.rc
								// used for system variables, not for player
								// specific configurations
#define	CVAR_USERINFO		2	// sent to server on connect or change
#define	CVAR_SERVERINFO		4	// sent in response to front end requests
#define	CVAR_SYSTEMINFO		8	// these cvars will be duplicated on all clients
#define	CVAR_INIT			16	// don't allow change from console at all,
								// but can be set from the command line
#define	CVAR_NOTIFY			32	// Will notify players when changed.

#define	CVAR_ROM			64	// display only, cannot be set by user at all
#define	CVAR_USER_CREATED	128	// created by a set command
#define	CVAR_HEAP			256	// allocated off the heap, safe to free
#define CVAR_CHEAT			512	// can not be changed if cheats are disabled
#define CVAR_NORESTART		1024	// do not clear when a cvar_restart is issued
#define CVAR_LATCH		2048	// will only change when C code next does
					// a Cvar_Get(), so it can't be changed
#define CVAR_TEMP		4906	// can be set even when cheats are
					// disabled, but is not archived

// Zoid| A good CVAR_ROM example is basepath.  The code should read "cvar_t
// *fs_basepath = CvarGet("fs_basepath", ".", CVAR_ROM);  The user can
// override that with +set fs_basepath <blah> since the command line +set gets
// created _before_ the C code for fs_basepath setup is called.  The code goes
// "look, the user made fs_basepath already", uses the users value, but sets
// CVAR_ROM as per the call.

cvar_t	*Cvar_Get (char *name, char *value, int cvarflags, char *description);

void	Cvar_Alias_Get (char *name, cvar_t *cvar);

void 	Cvar_Set (cvar_t *var, char *value);
// equivelant to "<name> <variable>" typed at the console

float	Cvar_VariableValue (char *var_name);
// returns 0 if not defined or non numeric

char	*Cvar_VariableString (char *var_name);
// returns an empty string if not defined

char 	*Cvar_CompleteVariable (char *partial);
// attempts to match a partial variable name for command line completion
// returns NULL if nothing fits

qboolean Cvar_Command (void);
// called by Cmd_ExecuteString when Cmd_Argv(0) doesn't match a known
// command.  Returns true if the command was a variable reference that
// was handled. (print or change)

void 	Cvar_WriteVariables (QFile *f);
// Writes lines containing "set variable value" for all variables
// with the archive flag set to true.

cvar_t *Cvar_FindVar (char *var_name);

void Cvar_Init();

void Cvar_Shutdown();
extern cvar_t	*cvar_vars;
#endif // _CVAR_H
