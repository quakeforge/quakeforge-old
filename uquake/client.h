/*
client.h
Copyright (C) 1996-1997 Id Software, Inc.

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

#ifndef _CLIENT_H
#define _CLIENT_H

#include <config.h>

#ifdef HAS_ZLIB
#include <zlib.h>
#else
#include <nozip.h>
#endif

#include <qtypes.h>
#include <common.h>
#include <quakefs.h>
#include <vid.h>
#include <net.h>
#include <bspfile.h>
#include <model.h>

typedef struct
{
	vec3_t	viewangles;

// intended velocities
	float	forwardmove;
	float	sidemove;
	float	upmove;
#ifdef QUAKE2
	byte	lightlevel;
#endif
} usercmd_t;

typedef struct
{
	int		length;
	char	map[MAX_STYLESTRING];
} lightstyle_t;

typedef struct
{
	char	name[MAX_SCOREBOARDNAME];
	float	entertime;
	int		frags;
	int		colors;			// two 4 bit fields
	byte	translations[VID_GRADES*256];
} scoreboard_t;

#define	CSHIFT_CONTENTS	0
#define	CSHIFT_DAMAGE	1
#define	CSHIFT_BONUS	2
#define	CSHIFT_POWERUP	3
#define	NUM_CSHIFTS		4

#define	NAME_LENGTH	64


//
// client_state_t should hold all pieces of the client state
//

#define	SIGNONS		4			// signon messages to receive before connected

#define	MAX_DLIGHTS		32
typedef struct
{
	vec3_t	origin;
	float	radius;
	float	die;				// stop lighting after this time
	float	decay;				// drop this each second
	float	minlight;			// don't add when contributing less
	int		key;
#ifdef QUAKE2
	qboolean	dark;			// subtracts light instead of adding
#endif
} dlight_t;


#define	MAX_BEAMS	24
typedef struct
{
	int		entity;
	struct model_s	*model;
	float	endtime;
	vec3_t	start, end;
} beam_t;

#define	MAX_EFRAGS		640

#define	MAX_MAPSTRING	2048
#define	MAX_DEMOS		8
#define	MAX_DEMONAME	16

typedef enum {
ca_dedicated, 		// a dedicated server with no ability to start a client
ca_disconnected, 	// full screen console with no connection
ca_connected,		// valid netcon, talking to a server
ca_onserver,		// processing data lists, etc
ca_active		// everything is in, so frames can be rendered
} cactive_t;

//
// the client_static_t structure is persistant through an arbitrary number
// of server connections
//
typedef struct
{
	cactive_t	state;

	netchan_t	netchan;

#ifdef QUAKEWORLD
	char		userinfo[MAX_INFO_STRING];
	char		servername[MAX_OSPATH];

	// download stuff
	int		qport;
	FILE		*download;
	char		downloadtempname[MAX_OSPATH];
	char		downloadname[MAX_OSPATH];
	int		downloadnumber;
	dltyle_t	downloadtype;
	int		downloadpercent;

	int		challenge;
	float		latency;
#endif

#ifdef UQUAKE
// personalization data sent to server	
	char		mapstring[MAX_QPATH];
	char		spawnparms[MAX_MAPSTRING];	// to restart a level

// connection information
	int		signon;			// 0 to SIGNONS
	struct qsocket_s	*netcon;
//	sizebuf_t	message;		// net msg write buffer
#endif

// demos - this stuff can't go into client_state_t
	int		demonum;		// -1 == don't play
	char		demos[MAX_DEMOS][MAX_DEMONAME];	// when not playing
	qboolean	demorecording;
	qboolean	demoplayback;
	qboolean	timedemo;
	int		forcetrack;		// -1 == normal cd track
	gzFile		*demofile;
	int		td_lastframe;		// for msg timing
	int		td_startframe;		// inits to host_framecount
	float		td_starttime;		// of 2nd frame of timedemo
} client_static_t;

extern client_static_t	cls;

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
	int			gametype;
	int			maxclients;

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
	qboolean	inwater;
	float		viewheight;
	float		idealpitch;
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

//
// cvars
//
extern	cvar_t	cl_name;
extern	cvar_t	cl_color;

extern	cvar_t	cl_upspeed;
extern	cvar_t	cl_forwardspeed;
extern	cvar_t	cl_backspeed;
extern	cvar_t	cl_sidespeed;

extern	cvar_t	cl_movespeedkey;

extern	cvar_t	cl_yawspeed;
extern	cvar_t	cl_pitchspeed;

extern	cvar_t	cl_anglespeedkey;

extern	cvar_t	cl_autofire;

extern	cvar_t	cl_shownet;
extern	cvar_t	cl_nolerp;

extern	cvar_t	cl_sbar;
extern	cvar_t	cl_hudswap;

extern	cvar_t	cl_pitchdriftspeed;
extern	cvar_t	lookspring;
extern	cvar_t	lookstrafe;
extern	cvar_t	sensitivity;

extern	cvar_t	m_pitch;
extern	cvar_t	m_yaw;
extern	cvar_t	m_forward;
extern	cvar_t	m_side;

extern cvar_t          _windowed_mouse;

#define	MAX_TEMP_ENTITIES	64			// lightning bolts, etc
#define	MAX_STATIC_ENTITIES	128			// torches, etc

// FIXME, allocate dynamically
extern	efrag_t			cl_efrags[MAX_EFRAGS];
extern	entity_t		cl_entities[MAX_EDICTS];
extern	entity_t		cl_static_entities[MAX_STATIC_ENTITIES];
extern	lightstyle_t	cl_lightstyle[MAX_LIGHTSTYLES];
extern	dlight_t		cl_dlights[MAX_DLIGHTS];
extern	entity_t		cl_temp_entities[MAX_TEMP_ENTITIES];
extern	beam_t			cl_beams[MAX_BEAMS];

//=============================================================================

//
// cl_main
//
dlight_t *CL_AllocDlight (int key);
void	CL_DecayLights (void);

void CL_Init (void);

void CL_EstablishConnection (char *host);
void CL_Signon1 (void);
void CL_Signon2 (void);
void CL_Signon3 (void);
void CL_Signon4 (void);

void CL_Disconnect (void);
void CL_Disconnect_f (void);
void CL_NextDemo (void);

#define			MAX_VISEDICTS	256
extern	int				cl_numvisedicts;
extern	entity_t		*cl_visedicts[MAX_VISEDICTS];

//
// cl_input
//
typedef struct
{
	int		down[2];		// key nums holding it down
	int		state;			// low bit is down state
} kbutton_t;

extern	kbutton_t	in_mlook, in_klook;
extern 	kbutton_t 	in_strafe;
extern 	kbutton_t 	in_speed;

void CL_InitInput (void);
void CL_SendCmd (void);
void CL_SendMove (usercmd_t *cmd);

void CL_ParseTEnt (void);
void CL_UpdateTEnts (void);

void CL_ClearState (void);


int  CL_ReadFromServer (void);
void CL_WriteToServer (usercmd_t *cmd);
void CL_BaseMove (usercmd_t *cmd);


float CL_KeyState (kbutton_t *key);
char *Key_KeynumToString (int keynum);

//
// cl_demo.c
//
void CL_StopPlayback (void);
int CL_GetMessage (void);

void CL_Stop_f (void);
void CL_Record_f (void);
void CL_PlayDemo_f (void);
void CL_TimeDemo_f (void);

//
// cl_parse.c
//
void CL_ParseServerMessage (void);
void CL_NewTranslation (int slot);

//
// view
//
void V_StartPitchDrift (void);
void V_StopPitchDrift (void);

void V_RenderView (void);
void V_UpdatePalette (void);
void V_Register (void);
void V_ParseDamage (void);
void V_SetContentsColor (int contents);
void V_CalcBlend (void);


//
// cl_tent
//
void CL_InitTEnts (void);
void CL_SignonReply (void);
#endif // _CLIENT_H
