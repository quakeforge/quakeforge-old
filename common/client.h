/*
	client.h

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

#ifndef _CLIENT_H
#define _CLIENT_H
#include <config.h>

#ifdef HAS_ZLIB
#include <zlib.h>
#else
#include <nozip.h>
#endif

#include <net.h>
#include <qtypes.h>
#include <zone.h>
#include <common_protocol.h>
#include <vid.h>
#include <render.h>
#include <common.h>
#include <sys.h>
#include <cmd.h>
#include <cvar.h>
#include <qstructs.h>
#include <image.h>

typedef struct
{
	char		name[16];
	qboolean	failedload;		// the name isn't a valid skin
	cache_user_t	cache;
} skin_t;

// player_state_t is the information needed by a player entity
// to do move prediction and to generate a drawable entity
typedef struct
{
	int		messagenum;	// all players not updated each frame

	double		state_time;	// not same as packet time b/c
					// player commands come async
	usercmd_t	command;	// last command for prediction

	vec3_t		origin;
	vec3_t		viewangles;	// only for demos, not from server
	vec3_t		velocity;
	int		weaponframe;

	int		modelindex;
	int		frame;
	int		skinnum;
	int		effects;

	int		flags;		// dead, gib, etc

	float		waterjumptime;
	int		onground;	// -1 = in air or pmove entity number
	int		oldbuttons;
} player_state_t;


typedef struct player_info_s
{
	int		userid;
	char		userinfo[MAX_INFO_STRING];

	// scoreboard information
	char		name[MAX_SCOREBOARDNAME];
	float		entertime;
	int		frags;
	int		ping;
	byte	pl;

	// skin information
	int		topcolor;
	int		bottomcolor;

	int		_topcolor;
	int		_bottomcolor;

	int		spectator;
	byte		translations[VID_GRADES*256];
	skin_t		*skin;
} player_info_t;


typedef struct
{
	// generated on client side
	usercmd_t	cmd;		// cmd that generated the frame
	double		senttime;	// time cmd was sent off
	int		delta_sequence;	// sequence number to delta from
					// -1 = full update

	// received from server
	double		receivedtime;	// time message was received, or -1
	player_state_t	playerstate[MAX_CLIENTS];	// message received,
					// reflects performing the usercmd
	packet_entities_t	packet_entities;
	qboolean	invalid;	// if packet_entities delta is invalid
} frame_t;
//#endif

#define	CSHIFT_CONTENTS		0
#define	CSHIFT_DAMAGE		1
#define	CSHIFT_BONUS		2
#define	CSHIFT_POWERUP		3
#define	NUM_CSHIFTS		4


// only UQuake uses these
#define NAME_LENGTH		64
#define SIGNONS			4

//
// client_state_t should hold all pieces of the client state
//
#define	MAX_DLIGHTS		32
typedef struct
{
	int		key;		// so entities can reuse same entry
	vec3_t		origin;
	float		radius;
	float		die;		// stop lighting after this time
	float		decay;		// drop this each second
	float		minlight;	// don't add when contributing less
	qboolean	dark;
	float   color[4];
} dlight_t;

typedef struct
{
	int		length;
	char	map[MAX_STYLESTRING];
} lightstyle_t;

typedef struct
{
	char    name[MAX_SCOREBOARDNAME];
	float   entertime;
	int             frags;
	int             colors;                 // two 4 bit fields
	byte    translations[VID_GRADES*256];
} scoreboard_t;

#define MAX_EFRAGS	640	// this was 512 in QW, 640 in UQ

#define MAX_MAPSTRING		2048
#define	MAX_DEMOS		8
#define	MAX_DEMONAME		16

typedef enum {
	ca_dedicated,		// server with no ability to start a client
	ca_disconnected, 	// full screen console with no connection
	ca_demostart,		// starting up a demo
	ca_connected,		// connected, waiting for svc_serverdata
	ca_onserver,		// processing data lists, donwloading, etc
	ca_active		// everything is in, so frames can be rendered
} cactive_t;

typedef enum {
	dl_none,
	dl_model,
	dl_sound,
	dl_skin,
	dl_single
} dltype_t;		// download type

//
// the client_static_t structure is persistant through an arbitrary number
// of server connections
//
typedef struct
{
	cactive_t	state;		// connection info
	netchan_t	netchan;	// network stuff

	char		userinfo[MAX_INFO_STRING];
	char		servername[MAX_OSPATH];
	int		qport;

	// download stuff
	QFile		*download;		// file transfer from server
	char		downloadtempname[MAX_OSPATH];
	char		downloadname[MAX_OSPATH];
	int		downloadnumber;
	dltype_t	downloadtype;
	int		downloadpercent;

	int		challenge;
	float		latency;		// rolling average
	char		mapstring[MAX_QPATH];
	char		spawnparms[MAX_MAPSTRING];	// to restart level

	int		signon;
	struct qsocket_s	*netcon;

// demos - this stuff can't go into client_state_t
	int		demonum;
	char		demos[MAX_DEMOS][MAX_DEMONAME];
	qboolean	demorecording;
	qboolean	demoplayback;
	qboolean	timedemo;
	int		forcetrack;
	QFile		*demofile;
	float		td_lastframe;
	int		td_startframe;
	float		td_starttime;

} client_static_t;

extern client_static_t	cls;

//
// the client_state_t structure is wiped completely at every
// server signon
//
typedef struct
{
	int		movemessages;	// since connecting to this server
					// throw out the first couple, so the
					// player doesn't accidentally do
					// something the first frame

// information for local display
	int		stats[MAX_CL_STATS];	// health, etc
	float		item_gettime[32];	// for item blinking
	float		faceanimtime;		// for face anims

	cshift_t	cshifts[NUM_CSHIFTS];	// color shifts
	cshift_t	prev_cshifts[NUM_CSHIFTS];

// the client maintains its own idea of view angles, which are
// sent to the server each frame.  And only reset at level change
// and teleport times
	vec3_t		viewangles;

// the client simulates or interpolates movement to get these values
	double		time;	// time clientis rendering at, <= realtime

// pitch drifting vars
	float		pitchvel;
	qboolean	nodrift;
	float		driftmove;
	double		laststop;


	qboolean	paused;		// send over by server

	int		completed_time;	// latched at intermission start
	float		punchangle;	// temporary yview for weapon kick
	int		intermission;	// don't change view, full screen, etc

//
// information that is static for the entire time connected to a server
//
	struct model_s		*model_precache[MAX_MODELS];
	struct sfx_s		*sound_precache[MAX_SOUNDS];

	char		levelname[40];	// for display on solo scoreboard

// refresh related state
	struct model_s	*worldmodel;	// cl_entitites[0].model
	struct efrag_s	*free_efrags;
	int		num_statics;	// held in cl_staticentities array

	int		cdtrack;		// cd audio

	entity_t	viewent;		// weapon model
	int		playernum;
	int		gametype;
	int		maxclients;

// QW specific!
// all player information
	qboolean	qfserver;
	player_info_t	players[MAX_CLIENTS];
	int		servercount;	// server identification for prespawns

	char		serverinfo[MAX_SERVERINFO_STRING];

	int		parsecount;	// server message counter
	int		validsequence;	// this is the sequence number of
					// the last good packetentity_t we
					// got. If this is 0, we can't render
					// a frame yet
	int		spectator;

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
// UQ specific.
	int		num_entities;	// held in cl_entities array
	float		last_received_message;	// (time) for net trouble icon
	double		mtime[2];	// the timestamp of last two messages
	double		oldtime;	// previous cl.time, time-oldtime is
					// used to decay light values and
					// smooth step ups

	qboolean	onground;
	qboolean	inwater;
	float		viewheight;
	float		idealpitch;
	scoreboard_t	*scores;	// [cl.maxclients]

	usercmd_t	cmd;		// last command sent to the server
	vec3_t		mviewangles[2];	// in demos, viewangles is lerped
					// between these
	vec3_t		mvelocity[2];	// update by server, used for lean+bob
					// (0 is newest)
	vec3_t		velocity;	// lerped between mvelocity[0] and [1]
} client_state_t;

extern client_state_t cl;

//
// cvars
//
extern cvar_t	*cl_name;
extern cvar_t	*cl_color;
extern cvar_t	*cl_autofire;
extern cvar_t	*cl_nolerp;
extern cvar_t	*cl_warncmd;
extern cvar_t	*name;
extern cvar_t	*topcolor;
extern cvar_t	*bottomcolor;
extern cvar_t	*rate;
extern cvar_t	*host_speeds;
extern cvar_t	*cl_maxfps;
extern cvar_t	*cl_upspeed;
extern cvar_t	*cl_forwardspeed;
extern cvar_t	*cl_backspeed;
extern cvar_t	*cl_sidespeed;

extern cvar_t	*cl_movespeedkey;

extern cvar_t	*cl_yawspeed;
extern cvar_t	*cl_pitchspeed;

extern cvar_t	*cl_anglespeedkey;

extern cvar_t	*cl_shownet;
extern cvar_t	*cl_sbar;
extern cvar_t	*cl_hudswap;

extern cvar_t	*cl_pitchdriftspeed;
extern cvar_t	*cl_verstring;

extern cvar_t	*cl_talksound;

extern cvar_t	*lookspring;
extern cvar_t	*lookstrafe;
extern cvar_t	*sensitivity;

extern cvar_t	*m_pitch;
extern cvar_t	*m_yaw;
extern cvar_t	*m_forward;
extern cvar_t	*m_side;

extern cvar_t	*_windowed_mouse;

#define MAX_TEMP_ENTITIES	64	// lightning bolts, etc
#define	MAX_STATIC_ENTITIES	128	// torches, etc


// FIXME, allocate dynamically
extern entity_state_t	cl_baselines[MAX_EDICTS];
extern entity_t                cl_entities[MAX_EDICTS];
extern entity_t                cl_temp_entities[MAX_TEMP_ENTITIES];
extern efrag_t		cl_efrags[MAX_EFRAGS];
extern entity_t		cl_static_entities[MAX_STATIC_ENTITIES];
extern lightstyle_t	cl_lightstyle[MAX_LIGHTSTYLES];
extern dlight_t		cl_dlights[MAX_DLIGHTS];

extern qboolean		nomaster;
extern char		*server_version;

//=============================================================================


//
// cl_main
//
dlight_t *CL_AllocDlight (int key);
void	CL_DecayLights (void);

void CL_Init (void);
void CL_InitCvars();
void Host_WriteConfiguration (void);

void CL_EstablishConnection (char *host);
void CL_CheckForResend (void);
void CL_SendConnectPacket (void);
void SetPal (int i);
void CL_RelinkEntities (void);
void CL_ReadPackets (void);

void CL_Signon1 (void);
void CL_Signon2 (void);
void CL_Signon3 (void);
void CL_Signon4 (void);

void CL_Disconnect (void);
void CL_Disconnect_f (void);
void CL_NextDemo (void);
qboolean CL_DemoBehind(void);
void CL_BeginServerConnect(void);


#define			MAX_VISEDICTS	256

extern	int		cl_numvisedicts, cl_oldnumvisedicts;
extern	entity_t	*cl_visedicts, *cl_oldvisedicts;
extern	entity_t	cl_visedicts_list[2][MAX_VISEDICTS];
extern char		emodel_name[], pmodel_name[], prespawn_name[],
			modellist_name[], soundlist_name[];


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

void CL_ReadPackets (void);

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
void CL_WriteDemoCmd (usercmd_t *pcmd);
void CL_SignonReply (void);

void CL_Stop_f (void);
void CL_Record_f (void);
void CL_ReRecord_f (void);
void CL_PlayDemo_f (void);
void CL_TimeDemo_f (void);

//
// cl_parse.c
//
void CL_ParseServerMessage (void);
void CL_NewTranslation (int slot);
#define NET_TIMINGS 256
#define NET_TIMINGSMASK 255
extern int	packet_latency[NET_TIMINGS];
int CL_CalcNet (void);
qboolean	CL_CheckOrDownloadFile (char *filename);
qboolean CL_IsUploading(void);
void CL_NextUpload(void);
void CL_StartUpload (byte *data, int size);
void CL_StopUpload(void);

//
// view.c
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
void CL_ClearTEnts (void);
void CL_SignonReply (void);

//
// cl_ents.c
//
void CL_SetSolidPlayers (int playernum);
void CL_SetUpPlayerPrediction(qboolean dopred);
void CL_EmitEntities (void);
void CL_ClearProjectiles (void);
void CL_ParseProjectiles (void);
void CL_ParsePacketEntities (qboolean delta);
void CL_SetSolidEntities (void);
void CL_ParsePlayerinfo (void);

//
// cl_pred.c
//
void CL_InitPrediction (void);
void CL_PredictMove (void);
void CL_PredictUsercmd (player_state_t *from, player_state_t *to, usercmd_t *u, qboolean spectator);

//
// cl_cam.c
//
#define CAM_NONE	0
#define CAM_TRACK	1

extern	int		autocam;
extern int spec_track; // player# of who we are tracking

qboolean Cam_DrawViewModel(void);
qboolean Cam_DrawPlayer(int playernum);
void Cam_Track(usercmd_t *cmd);
void Cam_FinishMove(usercmd_t *cmd);
void Cam_Reset(void);
void CL_InitCam(void);

//
// skin.c
//

void	Skin_Find (player_info_t *sc);
byte	*Skin_Cache (skin_t *skin);
void	Skin_Skins_f (void);
void	Skin_AllSkins_f (void);
void	Skin_NextDownload (void);

#define RSSHOT_WIDTH 320
#define RSSHOT_HEIGHT 200

#endif // _CLIENT_H
