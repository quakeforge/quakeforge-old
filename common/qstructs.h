/*
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
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

*/
#ifndef _QSTRUCTS_H
#define _QSTRUCTS_H

#include <stdio.h>
#include <sys/types.h>
#include <qtypes.h>
#include <render.h>

//=============================================================================

// the host system specifies the base of the directory tree, the
// command line parms passed to the program, and the amount of memory
// available for the program to use

typedef struct
{
	char	*basedir;
	char	*cachedir;		// for development over ISDN lines
	int	argc;
	char	**argv;
	void	*membase;
	int	memsize;
} quakeparms_t;

//=============================================================================

typedef struct cvar_s
{
	char	*name;
	char	*string;
	qboolean archive;		// set to true to cause it to be saved to vars.rc
	qboolean info;			// added to serverinfo or userinfo when changed
	qboolean server;		// notifies players when changed (UQUAKE)
	float	value;
	struct cvar_s *next;
} cvar_t;

typedef struct
{
	int	destcolor[3];
	int	percent;			// 0-256
} cshift_t;

#if defined(QUAKEWORLD) || defined(UQUAKE)
#include "client.h"
#endif

//
// the client_state_t structure is wiped completely at every
// server signon
//
typedef struct
{
	int			movemessages;	// since connecting to this server
								// throw out the first couple, so the player
								// doesn't accidentally do something the 
								// first frame

// information for local display
	int			stats[MAX_CL_STATS];	// health, etc
	float		item_gettime[32];		//cl.time of aquiring item, for blinking
	float		faceanimtime;			// use anim frame if cl.time < this

	cshift_t	cshifts[NUM_CSHIFTS];	// color shifts for damage, powerups
	cshift_t	prev_cshifts[NUM_CSHIFTS];	// and content types

// the client maintains its own idea of view angles, which are
// sent to the server each frame.  And only reset at level change
// and teleport times
	vec3_t		viewangles;

// the client simulates or interpolates movement to get these values
	double		time;		// this is the time value that the client
							// is rendering at.  allways <= realtime

// pitch drifting vars
	float		pitchvel;
	qboolean	nodrift;
	float		driftmove;
	double		laststop;


	qboolean	paused;		// send over by server

	int			completed_time;	// latched at intermission start
	float		punchangle;	// temporar yview kick from weapon firing
	int			intermission;	// don't change view angle, full screen, etc
	
//
// information that is static for the entire time connected to a server
//
	struct model_s		*model_precache[MAX_MODELS];
	struct sfx_s		*sound_precache[MAX_SOUNDS];

	char		levelname[40];	// for display on solo scoreboard

// refresh related state
	struct model_s	*worldmodel;	// cl_entitites[0].model
	struct efrag_s	*free_efrags;
	int			num_statics;	// held in cl_staticentities array

	int			cdtrack;		// cd audio

	entity_t	viewent;		// weapon model
	int			playernum;

#ifdef QUAKEWORLD
// QW specific!
// all player information
	player_info_t	players[MAX_CLIENTS];
	int			servercount;	// server identification for prespawns

	char		serverinfo[MAX_SERVERINFO_STRING];

	int			parsecount;		// server message counter
	int			validsequence;	// this is the sequence number of the last good
								// packetentity_t we got. If this is 0, we can't
								// render a frame yet
	int			spectator;

	double		last_ping_request;	// while showing scoreboard

	frame_t		frames[UPDATE_BACKUP];

	vec3_t		simorg;
	vec3_t		simvel;
	vec3_t		simangles;
//
// information that is static for the entire time connected to a server
//
	char		model_name[MAX_MODELS][MAX_QPATH];
	char		sound_name[MAX_SOUNDS][MAX_QPATH];
#elif defined(UQUAKE)
// UQ specific.
	int			num_entities;	// held in cl_entities array
	float		last_received_message;	// (realtime) for net trouble icon
	double		mtime[2];	// the timestamp of last two messages	
	double		oldtime;	// previous cl.time, time-oldtime is used
							// to decay light values and smooth step ups
	
	qboolean	onground;
	float		viewheight;
	float		idealpitch;
	int			maxclients;
	int			gametype;
// frag scoreboard
	scoreboard_t	*scores;		// [cl.maxclients]

	usercmd_t	cmd;	// last command sent to the server
	int			items;			// inventory bit flags
	vec3_t		mviewangles[2];	// during demo playback viewangles is lerped
								// between these
	vec3_t		mvelocity[2];	// update by server, used for lean+bob
								// (0 is newest)
	vec3_t		velocity;	// lerped between mvelocity[0] and [1]
#endif
} client_state_t;

extern client_state_t cl;
#endif // _QSTRUCTS_H
