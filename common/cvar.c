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
cvar_alias_t *calias_vars;

// 2000-06-22 General cvar display by Maddes  start
/*
============
Cvar_Display

same cvar display for all cvar commands
============
*/
void
Cvar_Display ( cvar_t *var )
{
	Con_Printf ("%c%c%c ",
			(var->flags & CVAR_ARCHIVE) ? 'A' : ' ',	// archived
			(var->flags & CVAR_ROM) ? 'R' : ' ',		// read-only
			(var->flags & CVAR_USER_CREATED) ? 'U' : ' ');	// user-created

	Con_Printf ("\"%s\" is \"%s\"", var->name, var->string);

// 2000-06-22 Range check for cvars by Maddes  start
	if (var->rangecheck)
	{
		if (var->rangecheck == 3)	// boolean
		{
			Con_Printf (" (bool: 0/1)");
		}
		else if (var->rangecheck == 2)	// integer
		{
			Con_Printf (" (int: %i-%i)", (int)var->minvalue, (int)var->maxvalue);
		}
		else
		{
			char	val[32];
			int	i;

			if (var->minvalue == (int)var->minvalue)
			{
				sprintf (val, "%i", (int)var->minvalue);
			}
			else
			{
				sprintf (val, "%1f", var->minvalue);
				for (i=Q_strlen(val)-1 ; i>0 && val[i]=='0' && val[i-1]!='.' ; i--)
				{
					val[i] = 0;
				}
			}
			Con_Printf (" (float: %s-", val);

			if (var->maxvalue == (int)var->maxvalue)
			{
				sprintf (val, "%i", (int)var->maxvalue);
			}
			else
			{
				sprintf (val, "%1f", var->maxvalue);
				for (i=Q_strlen(val)-1 ; i>0 && val[i]=='0' && val[i-1]!='.' ; i--)
				{
					val[i] = 0;
				}
			}
			Con_Printf ("%s)", val);
		}
	}
// 2000-06-22 Range check for cvars by Maddes  end

	Con_Printf ("\n");
}
// 2000-06-22 General cvar display by Maddes  end

/*
============
Cvar_FindVar
============
*/
cvar_t *
Cvar_FindVar ( char *var_name )
{
	cvar_t	*var;

	for (var=cvar_vars ; var ; var=var->next)
		if (!Q_strcmp (var_name, var->name))
			return var;

	return NULL;
}

cvar_t *
Cvar_FindAlias ( char *alias_name )
{
	cvar_alias_t	*alias;

	for (alias = calias_vars ; alias ; alias=alias->next)
		if (!Q_strcmp (alias_name, alias->name))
			return alias->cvar;
	return NULL;
}

void
Cvar_Alias_Get ( char *name, cvar_t *cvar )
{
	cvar_alias_t	*alias;
	cvar_t		*var;

	if (Cmd_Exists (name))
	{
		Con_Printf ("CAlias_Get: %s is a command\n", name);
		return;
	}
	if (Cvar_FindVar(name))
	{
		Con_Printf ("CAlias_Get: tried to alias used cvar name %s\n",name);
		return;
	}
	var = Cvar_FindAlias(name);
	if (!var)
	{
		alias = (cvar_alias_t *) malloc(sizeof(cvar_alias_t));
		alias->next = calias_vars;
		calias_vars = alias;
		alias->name = strdup(name);
		alias->cvar = cvar;
	}
}

/*
============
Cvar_VariableValue
============
*/
float
Cvar_VariableValue ( char *var_name )
{
	cvar_t	*var;

	var = Cvar_FindVar (var_name);
	if (!var)
		var = Cvar_FindAlias(var_name);
	if (!var)
		return 0;
	return Q_atof (var->string);
}


/*
============
Cvar_VariableString
============
*/
char *
Cvar_VariableString ( char *var_name )
{
	cvar_t *var;

	var = Cvar_FindVar (var_name);
	if (!var)
		var = Cvar_FindAlias(var_name);
	if (!var)
		return cvar_null_string;
	return var->string;
}


/*
============
Cvar_CompleteVariable
============
*/
char *
Cvar_CompleteVariable ( char *partial )
{
	cvar_t		*cvar;
	cvar_alias_t	*alias;
	int		len;

	len = Q_strlen(partial);

	if (!len)
		return NULL;

	// check exact match
	for (cvar=cvar_vars ; cvar ; cvar=cvar->next)
		if (!strcmp (partial,cvar->name))
			return cvar->name;

	// check aliases too :)
	for (alias=calias_vars ; alias ; alias=alias->next)
		if (!strcmp (partial, alias->name))
			return alias->name;

	// check partial match
	for (cvar=cvar_vars ; cvar ; cvar=cvar->next)
		if (!Q_strncmp (partial,cvar->name, len))
			return cvar->name;

	// check aliases too :)
	for (alias=calias_vars ; alias ; alias=alias->next)
		if (!Q_strncmp (partial, alias->name, len))
			return alias->name;

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
void
Cvar_Set ( cvar_t *var, char *value )
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
void
Cvar_Set ( cvar_t *var, char *value )
{
	qboolean changed;
// 2000-06-22 Range check for cvars by Maddes  start
	char	val[32];
	float	newvalue;
	int	i;
// 2000-06-22 Range check for cvars by Maddes  end

	if (!var)
		return;

	// Don't change if this is a CVAR_ROM
	if(var->flags&CVAR_ROM) return;

// 2000-06-22 Range check for cvars by Maddes  start
	if (var->rangecheck)
	{
		newvalue = Q_atof (value);

		if (var->rangecheck == 3)	// boolean
		{
			if (newvalue)
			{
				newvalue = 1;
			}
			else
			{
				newvalue = 0;
			}
		}
		else
		{
			if (var->rangecheck == 2)	// integer
			{
				newvalue = (int)newvalue;
			}

			// check limits of newvalue
			if (newvalue < var->minvalue)
			{
				newvalue = var->minvalue;
			}
			if (newvalue > var->maxvalue)
			{
				newvalue = var->maxvalue;
			}
		}

		if (newvalue == (int)newvalue)
		{
			sprintf (val, "%i", (int)newvalue);
		}
		else
		{
			sprintf (val, "%1f", newvalue);
			for (i=Q_strlen(val)-1 ; i>0 && val[i]=='0' && val[i-1]!='.' ; i--)
			{
				val[i] = 0;
			}
		}

		changed = Q_strcmp(var->string, val);
	}
	else
	{
// 2000-06-22 Range check for cvars by Maddes  end
		changed = Q_strcmp(var->string, value);
	}	// 2000-06-22 Range check for cvars by Maddes

// 2000-06-22 Fix for unnecessary cvar changing/zone usage by Maddes  start
	if (!changed)	// nothing changed, nothing to do
	{
		return;
	}
// 2000-06-22 Fix for unnecessary cvar changing/zone usage by Maddes  end

	Z_Free (var->string);	// free the old value string

// 2000-06-22 Range check for cvars by Maddes  start
	if (var->rangecheck)
	{
		var->string = Z_Malloc (Q_strlen(val)+1);
		Q_strcpy (var->string, val);
	}
	else
	{
// 2000-06-22 Range check for cvars by Maddes  end
		var->string = Z_Malloc (Q_strlen(value)+1);
		Q_strcpy (var->string, value);
	}	// 2000-06-22 Range check for cvars by Maddes
	var->value = Q_atof (var->string);
	if (var->flags&CVAR_USERINFO && changed)
	{
		if (sv.active)
			SV_BroadcastPrintf ("\"%s\" changed to \"%s\"\n", var->name, var->string);
	}

// 1999-09-06 deathmatch/coop not at the same time fix by Maddes  start
	if ( (var == deathmatch) && (var->value != 0) )
	{
		Cvar_Set (coop, "0");
	}

	if ( (var == coop) && (var->value != 0) )
	{
		Cvar_Set (deathmatch, "0");
	}
// 1999-09-06 deathmatch/coop not at the same time fix by Maddes  end
}
#endif


/*
============
Cvar_Command

Handles variable inspection and changing from the console
============
*/
qboolean
Cvar_Command ( void )
{
	cvar_t			*v;

// check variables
	v = Cvar_FindVar (Cmd_Argv(0));
	if (!v)
		v = Cvar_FindAlias (Cmd_Argv(0));
	if (!v)
		return false;

// perform a variable print or set
	if (Cmd_Argc() == 1)
	{
		Cvar_Display (v);	// 2000-06-22 General cvar display by Maddes
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
void
Cvar_WriteVariables ( QFile *f )
{
	cvar_t	*var;

	for (var = cvar_vars ; var ; var = var->next)
		if (var->flags&CVAR_ARCHIVE)
			Qprintf (f, "%s \"%s\"\n", var->name, var->string);
}

void
Cvar_Set_f ( void )
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
	if (!var)
		var = Cvar_FindAlias (var_name);
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

void
Cvar_Toggle_f ( void )
{
	cvar_t *var;

	if (Cmd_Argc() != 2)
	{
		Con_Printf ("toggle <cvar> : toggle a cvar on/off\n");
		return;
	}

	var = Cvar_FindVar (Cmd_Argv(1));
	if (!var)
		var = Cvar_FindAlias(Cmd_Argv(1));
	if (!var)
	{
		Con_Printf ("Unknown variable \"%s\"\n", Cmd_Argv(1));
		return;
	}

	Cvar_Set (var, var->value ? "0" : "1");
}

void
Cvar_Help_f ( void )
{
	char	*var_name;
	cvar_t	*var;

	if (Cmd_Argc() != 2)
	{
		Con_Printf ("usage: help <cvar>\n");
		return;
	}

	var_name = Cmd_Argv (1);
	var = Cvar_FindVar (var_name);
	if (!var)
		var = Cvar_FindAlias (var_name);
	if (var)
	{
		Con_Printf ("%s\n",var->description);
		return;
	}
	Con_Printf ("variable not found\n");
}

void
Cvar_CvarList_f ( void )
{
	cvar_t	*var;
	int i;
// 2000-06-22 Partial selection for CvarList command by Maddes  start
	char 		*partial;
	int		len;
	int		count;

	if (Cmd_Argc() > 1)
	{
		partial = Cmd_Argv (1);
		len = Q_strlen(partial);
	}
	else
	{
		partial = NULL;
		len = 0;
	}

	count=0;
// 2000-06-22 Partial selection for CvarList command by Maddes  end

	for (var=cvar_vars, i=0 ; var ; var=var->next, i++)
	{
// 2000-06-22 Partial selection for CvarList command by Maddes  start
		if (partial && Q_strncmp (partial, var->name, len))
		{
			continue;
		}
		count++;
// 2000-06-22 Partial selection for CvarList command by Maddes  end
		Cvar_Display (var);	// 2000-06-22 General cvar display by Maddes
	}

// 2000-06-22 Partial selection for CvarList command by Maddes  start
	Con_Printf ("------------\n");
	if (partial)
	{
		Con_Printf ("%i beginning with \"%s\" out of ", count, partial);
	}
	Con_Printf ("%i variables\n", i);
// 2000-06-22 Partial selection for CvarList command by Maddes  end
}

void
Cvar_Init ( )
{
	developer = Cvar_Get ("developer","0",0,"None");

	Cmd_AddCommand ("set", Cvar_Set_f);
	Cmd_AddCommand ("toggle", Cvar_Toggle_f);
	Cmd_AddCommand ("help",Cvar_Help_f);
	Cmd_AddCommand ("cvarlist",Cvar_CvarList_f);
}

void
Cvar_Shutdown ( void )
{
	cvar_t	*var,*next;
	cvar_alias_t	*alias,*nextalias;

	// Free cvars
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
	// Free aliases
	alias = calias_vars;
	while(alias)
	{
		nextalias = alias->next;
		free(alias->name);
		free(alias);
		alias = nextalias;
	}
}


cvar_t *
Cvar_Get ( char *name, char *string, int cvarflags, char *description )
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
// 2000-06-22 Range check for cvars by Maddes  start
		v->rangecheck = 0;
		v->minvalue = 0;
		v->maxvalue = 0;
// 2000-06-22 Range check for cvars by Maddes  end
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
