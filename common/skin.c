/*
skin.c - player skins
Copyright (C) 1996-1997  Id Software, Inc.
Copyright (C) 1999,2000  contributors of the QuakeForge project
Portions Copyright (C) 1999,2000  Nelson Rush.
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
#include "cvar.h"
#include "client.h"
#include "console.h"
#include "client.h"

//cvar_t		baseskin = {"baseskin", "base"};
cvar_t	*baseskin;
//cvar_t		noskins = {"noskins", "0"};
cvar_t	*noskins;

char		allskins[128];
#define	MAX_CACHED_SKINS		128
skin_t		skins[MAX_CACHED_SKINS];
int			numskins;

/*
================
Skin_Find

  Determines the best skin for the given scoreboard
  slot, and sets scoreboard->skin

================
*/
void Skin_Find (player_info_t *sc)
{
	skin_t		*skin;
	int			i;
	char		name[128], *s;

	if (allskins[0])
		strcpy (name, allskins);
	else
	{
		s = Info_ValueForKey (sc->userinfo, "skin");
		if (s && s[0])
			strcpy (name, s);
		else
			strcpy (name, baseskin->string);
	}

	if (strstr (name, "..") || *name == '.')
		strcpy (name, "base");

	COM_StripExtension (name, name);

	for (i=0 ; i<numskins ; i++)
	{
		if (!strcmp (name, skins[i].name))
		{
			sc->skin = &skins[i];
			Skin_Cache (sc->skin);
			return;
		}
	}

	if (numskins == MAX_CACHED_SKINS)
	{	// ran out of spots, so flush everything
		Skin_Skins_f ();
		return;
	}

	skin = &skins[numskins];
	sc->skin = skin;
	numskins++;

	memset (skin, 0, sizeof(*skin));
	strncpy(skin->name, name, sizeof(skin->name) - 1);
}


/*
==========
Skin_Cache

Returns a pointer to the skin bitmap, or NULL to use the default
==========
*/
byte	*Skin_Cache (skin_t *skin)
{
	char	name[1024];
//	byte	*raw;
	byte	*out;//, *pix;
//	pcx_t	*pcx;
//	int		x, y;
//	int		dataByte;
//	int		runLength;

	if (cls.downloadtype == dl_skin)
		return NULL;		// use base until downloaded

	if (noskins->value==1) // JACK: So NOSKINS > 1 will show skins, but
		return NULL;	  // not download new ones.

	if (skin->failedload)
		return NULL;

	out = Cache_Check (&skin->cache);
	if (out)
		return out;

//
// load the pic from disk
//
	snprintf(name, sizeof(name), "skins/%s.pcx", skin->name);
	out = LoadPCX (name, &skin->cache, 320, 200);
	if (out == NULL) {
		Con_Printf ("Couldn't load skin %s\n", name);
		snprintf(name, sizeof(name), "skins/%s.pcx", baseskin->string);
		out = LoadPCX (name, &skin->cache, 320, 200);
		if (out == NULL) {
			skin->failedload = true;
			return NULL;
		}
	}

	skin->failedload = false;
	return out;
}


/*
=================
Skin_NextDownload
=================
*/
void Skin_NextDownload (void)
{
	player_info_t	*sc;
	int			i;

	if (cls.downloadnumber == 0)
		Con_Printf ("Checking skins...\n");
	cls.downloadtype = dl_skin;

	for ( 
		; cls.downloadnumber != MAX_CLIENTS
		; cls.downloadnumber++)
	{
		sc = &cl.players[cls.downloadnumber];
		if (!sc->name[0])
			continue;
		Skin_Find (sc);
		if (noskins->value)
			continue;
#ifndef UQUAKE
		if (!CL_CheckOrDownloadFile(va("skins/%s.pcx", sc->skin->name)))
			return;		// started a download
#endif
	}

	cls.downloadtype = dl_none;

	// now load them in for real
	for (i=0 ; i<MAX_CLIENTS ; i++)
	{
		sc = &cl.players[i];
		if (!sc->name[0])
			continue;
		Skin_Cache (sc->skin);
		sc->skin = NULL;
	}

	if (cls.state != ca_active)
	{	// get next signon phase
		MSG_WriteByte (&cls.netchan.message, clc_stringcmd);
		MSG_WriteString (&cls.netchan.message,
			va("begin %i", cl.servercount));
		Cache_Report ();		// print remaining memory
	}
}


/*
==========
Skin_Skins_f

Refind all skins, downloading if needed.
==========
*/
void	Skin_Skins_f (void)
{
	int		i;

	for (i=0 ; i<numskins ; i++)
	{
		if (skins[i].cache.data)
			Cache_Free (&skins[i].cache);
	}
	numskins = 0;

	cls.downloadnumber = 0;
	cls.downloadtype = dl_skin;
	Skin_NextDownload ();
}


/*
==========
Skin_AllSkins_f

Sets all skins to one specific one
==========
*/
void	Skin_AllSkins_f (void)
{
	strcpy (allskins, Cmd_Argv(1));
	Skin_Skins_f ();
}
