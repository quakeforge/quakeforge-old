/*
	cvar.c

	dynamic variable tracking

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

#include <zone.h>
#include <qtypes.h>
#include <qstructs.h>
#include <lib_replace.h>
#include <console.h>
#include <cmd.h>
#include <cvar.h>
#include <cvars.h>
#include <client.h>
#include <stdlib.h>
#if defined(UQUAKE) || defined(SERVERONLY)
#include <server.h>
#endif
#include <string.h>

#if !defined(HAVE_SNPRINTF) && defined(HAVE__SNPRINTF)
# define snprintf _snprintf
#endif

cvar_t	*cvar_vars;
char	*cvar_null_string = "";
cvar_t	*developer;

/*
============
Cvar_FindVar
============
*/
cvar_t *Cvar_FindVar (char *var_name)
{
	cvar_t	*var;

	for (var=cvar_vars ; var ; var=var->next)
		if (!Q_strcmp (var_name, var->name))
			return var;

	return NULL;
}

/*
============
Cvar_VariableValue
============
*/
float	Cvar_VariableValue (char *var_name)
{
	cvar_t	*var;

	var = Cvar_FindVar (var_name);
	if (!var)
		return 0;
	return Q_atof (var->string);
}


/*
============
Cvar_VariableString
============
*/
char *Cvar_VariableString (char *var_name)
{
	cvar_t *var;

	var = Cvar_FindVar (var_name);
	if (!var)
		return cvar_null_string;
	return var->string;
}


/*
============
Cvar_CompleteVariable
============
*/
char *Cvar_CompleteVariable (char *partial)
{
	cvar_t		*cvar;
	int			len;

	len = Q_strlen(partial);

	if (!len)
		return NULL;

	// check exact match
	for (cvar=cvar_vars ; cvar ; cvar=cvar->next)
		if (!strcmp (partial,cvar->name))
			return cvar->name;

	// check partial match
	for (cvar=cvar_vars ; cvar ; cvar=cvar->next)
		if (!Q_strncmp (partial,cvar->name, len))
			return cvar->name;

	return NULL;
}


#ifdef SERVERONLY
void SV_SendServerInfoChange(char *key, char *value);
#endif

/*
============
Cvar_Set
============
*/
#if defined(QUAKEWORLD)
void Cvar_Set (cvar_t *var, char *value)
{
	if (!var)
		return;

	if(var->flags&CVAR_ROM) return;
#ifdef SERVERONLY
	if (var->flags&CVAR_SERVERINFO)
	{
		Info_SetValueForKey (svs.info, var->name, value, MAX_SERVERINFO_STRING);
		SV_SendServerInfoChange(var->name, value);
//		SV_BroadcastCommand ("fullserverinfo \"%s\"\n", svs.info);
	}
#else
	if (var->flags&CVAR_USERINFO)
	{
		Info_SetValueForKey (cls.userinfo, var->name, value, MAX_INFO_STRING);
		if (cls.state >= ca_connected)
		{
			MSG_WriteByte (&cls.netchan.message, clc_stringcmd);
			SZ_Print (&cls.netchan.message, va("setinfo \"%s\" \"%s\"\n", var->name, value));
		}
	}
#endif

	Z_Free (var->string);	// free the old value string

	var->string = Z_Malloc (Q_strlen(value)+1);
	Q_strcpy (var->string, value);
	var->value = Q_atof (var->string);
}
#elif defined(UQUAKE)
void Cvar_Set (cvar_t *var, char *value)
{
	qboolean changed;

	if (!var)
		return;

	// Don't change if this is a CVAR_ROM
	if(var->flags&CVAR_ROM) return;

	changed = Q_strcmp(var->string, value);

	Z_Free (var->string);	// free the old value string

	var->string = Z_Malloc (Q_strlen(value)+1);
	Q_strcpy (var->string, value);
	var->value = Q_atof (var->string);
	if (var->flags&CVAR_USERINFO && changed)
	{
		if (sv.active)
			SV_BroadcastPrintf ("\"%s\" changed to \"%s\"\n", var->name, var->string);
	}
}
#endif


/*
============
Cvar_Command

Handles variable inspection and changing from the console
============
*/
qboolean	Cvar_Command (void)
{
	cvar_t			*v;

// check variables
	v = Cvar_FindVar (Cmd_Argv(0));
	if (!v)
		return false;

// perform a variable print or set
	if (Cmd_Argc() == 1)
	{
		Con_Printf ("\"%s\" is \"%s\"\n", v->name, v->string);
		return true;
	}

	Cvar_Set (v, Cmd_Argv(1));
	return true;
}


/*
============
Cvar_WriteVariables

Writes lines containing "set variable value" for all variables
with the archive flag set to true.
============
*/
void Cvar_WriteVariables (QFile *f)
{
	cvar_t	*var;

	for (var = cvar_vars ; var ; var = var->next)
		if (var->flags&CVAR_ARCHIVE)
			Qprintf (f, "%s \"%s\"\n", var->name, var->string);
}

void Cvar_Set_f(void)
{
	cvar_t *var;
	char *value;
	char *var_name;

	if (Cmd_Argc() != 3)
	{
		Con_Printf ("usage: set <cvar> <value>\n");
		return;
	}
	var_name = Cmd_Argv (1);
	value = Cmd_Argv (2);
	var = Cvar_FindVar (var_name);
	if (var)
	{
		Cvar_Set (var, value);
	}
	else
	{
		var = Cvar_Get (var_name, value, CVAR_USER_CREATED|CVAR_HEAP,
				"User created cvar");
	}
}

void Cvar_Toggle_f (void)
{
	cvar_t *var;

	if (Cmd_Argc() != 2)
	{
		Con_Printf ("toggle <cvar> : toggle a cvar on/off\n");
		return;
	}

	var = Cvar_FindVar (Cmd_Argv(1));
	if (!var)
	{
		Con_Printf ("Unknown variable \"%s\"\n", Cmd_Argv(1));
		return;
	}

	Cvar_Set (var, var->value ? "0" : "1");
}

void Cvar_Help_f (void)
{
	char *cvar_name;
	cvar_t	*var;

	if (Cmd_Argc() != 2)
	{
		Con_Printf ("usage: help <cvar>\n");
		return;
	}

	cvar_name = Cmd_Argv (1);
	if((var = Cvar_FindVar(cvar_name)) != NULL)
	{
		Con_Printf ("description: %s\nvalue: %s\n",var->description,
			    var->string);
		return;
	}
	Con_Printf ("variable not found\n");
}

void Cvar_CvarList_f (void)
{
	cvar_t	*var;
	int i;

	for (var=cvar_vars, i=0 ; var ; var=var->next, i++)
	{
		Con_Printf("%s\n",var->name);
	}
	Con_Printf ("------------\n%d variables\n", i);
}

void Cvar_Init()
{
	developer = Cvar_Get ("developer","0",0,"None");

	Cmd_AddCommand ("set", Cvar_Set_f);
	Cmd_AddCommand ("toggle", Cvar_Toggle_f);
	Cmd_AddCommand ("help",Cvar_Help_f);
	Cmd_AddCommand ("cvarlist",Cvar_CvarList_f);
}

void Cvar_Shutdown (void)
{
	cvar_t	*var,*next;

	var = cvar_vars;
	while(var)
	{
		next = var->next;
		free(var->description);
		Z_Free(var->string);
		free(var->name);
		free(var);
		var = next;
	}
}


cvar_t *Cvar_Get(char *name, char *string, int cvarflags, char *description)
{

	cvar_t		*v;

	if (Cmd_Exists (name))
	{
		Con_Printf ("Cvar_Get: %s is a command\n",name);
		return NULL;
	}
	v = Cvar_FindVar(name);
	if (!v)
	{
		v = (cvar_t *) malloc(sizeof(cvar_t));
		// Cvar doesn't exist, so we create it
		v->next = cvar_vars;
		cvar_vars = v;
		v->name = strdup(name);
		v->string = Z_Malloc (Q_strlen(string)+1);
		Q_strcpy (v->string, string);
		v->flags = cvarflags;
		v->description = strdup(description);
		v->value = Q_atof (v->string);
		return v;
	}
	// Cvar does exist, so we update the flags and return.
	v->flags ^= CVAR_USER_CREATED;
	v->flags ^= CVAR_HEAP;
	v->flags |= cvarflags;
	if (!Q_strcmp (v->description,"User created cvar"))
	{	
		// Set with the real description
		free(v->description);
		v->description = strdup (description);
	}
	return v;
}
