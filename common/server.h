/*
	server.h

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

#ifndef _SERVER_H
#define _SERVER_H

#if !(defined(QUAKEWORLD) || defined(UQUAKE))
#error This is NOT safe to include for common items.
#endif

#ifdef _WIN32
#pragma warning( disable : 4244 4127 4201 4214 4514 4305 4115 4018)
#endif

#include <qtypes.h>
#include <progs.h>
#include <setjmp.h>

#ifdef QUAKEWORLD
#define	QW_SERVER
#endif
#define	MAX_MASTERS	8				// max recipients for heartbeat packets
#define	MAX_SIGNON_BUFFERS	8
#define MAX_MSECS 100

typedef enum {
	RD_NONE,
	RD_CLIENT,
	RD_PACKET
} redirect_t;

typedef enum {
#ifdef QUAKEWORLD
	ss_dead,			// no map loaded
#endif
	ss_loading,			// spawning level edicts
	ss_active			// actively running
} server_state_t;
// some qc commands are only valid before the server has finished
// initializing (precache commands, static sounds / objects, etc)

struct qsocket_s;

#ifdef QUAKEWORLD
typedef struct
{
	qboolean	active;				// false when server is going down
	server_state_t	state;			// precache commands are only valid during load

	double		time;

	int			lastcheck;			// used by PF_checkclient
	double		lastchecktime;		// for monster ai

	qboolean	paused;				// are we paused?

	//check player/eyes models for hacks
	unsigned	model_player_checksum;
	unsigned	eyes_player_checksum;

	char		name[64];			// map name
	char		modelname[MAX_QPATH];		// maps/<name>.bsp, for model_precache[0]
	struct model_s 	*worldmodel;
	char		*model_precache[MAX_MODELS];	// NULL terminated
	char		*sound_precache[MAX_SOUNDS];	// NULL terminated
	char		*lightstyles[MAX_LIGHTSTYLES];
	struct model_s		*models[MAX_MODELS];

	int			num_edicts;			// increases towards MAX_EDICTS
	edict_t		*edicts;			// can NOT be array indexed, because
									// edict_t is variable sized, but can
									// be used to reference the world ent

	byte		*pvs, *phs;			// fully expanded and decompressed

	// added to every client's unreliable buffer each frame, then cleared
	sizebuf_t	datagram;
	byte		datagram_buf[MAX_DATAGRAM];

	// added to every client's reliable buffer each frame, then cleared
	sizebuf_t	reliable_datagram;
	byte		reliable_datagram_buf[MAX_MSGLEN];

	// the multicast buffer is used to send a message to a set of clients
	sizebuf_t	multicast;
	byte		multicast_buf[MAX_MSGLEN];

	// the master buffer is used for building log packets
	sizebuf_t	master;
	byte		master_buf[MAX_DATAGRAM];

	// the signon buffer will be sent to each client as they connect
	// includes the entity baselines, the static entities, etc
	// large levels will have >MAX_DATAGRAM sized signons, so
	// multiple signon messages are kept
	sizebuf_t	signon;
	int			num_signon_buffers;
	int			signon_buffer_size[MAX_SIGNON_BUFFERS];
	byte		signon_buffers[MAX_SIGNON_BUFFERS][MAX_DATAGRAM];
} server_t;
#else
typedef struct
{
	qboolean	active;				// false if only a net client

	qboolean	paused;
	qboolean	loadgame;			// handle connections specially

	double		time;

	int			lastcheck;			// used by PF_checkclient
	double		lastchecktime;

	char		name[64];			// map name
#ifdef QUAKE2
	char		startspot[64];
#endif
	char		modelname[64];		// maps/<name>.bsp, for model_precache[0]
	struct model_s 	*worldmodel;
	char		*model_precache[MAX_MODELS];	// NULL terminated
	struct model_s	*models[MAX_MODELS];
	char		*sound_precache[MAX_SOUNDS];	// NULL terminated
	char		*lightstyles[MAX_LIGHTSTYLES];
	int			num_edicts;
	int			max_edicts;
	edict_t		*edicts;			// can NOT be array indexed, because
									// edict_t is variable sized, but can
									// be used to reference the world ent
	server_state_t	state;			// some actions are only valid during load

	sizebuf_t	datagram;
	byte		datagram_buf[MAX_DATAGRAM];

	sizebuf_t	reliable_datagram;	// copied to all clients at end of frame
	byte		reliable_datagram_buf[MAX_DATAGRAM];

	sizebuf_t	signon;
	byte		signon_buf[8192];
} server_t;
#endif

#define	NUM_PING_TIMES		16
#define	NUM_SPAWN_PARMS		16

typedef enum
{
	cs_free,		// can be reused for a new connection
	cs_zombie,		// client has been disconnected, but don't reuse
					// connection for a couple seconds
	cs_connected,	// has been assigned to a client_t, but not in game yet
	cs_spawned		// client is fully in game
} sv_client_state_t;

typedef struct
{
	// received from client

	// reply
	double				senttime;
	float				ping_time;
	packet_entities_t	entities;
} client_frame_t;

#define MAX_BACK_BUFFERS 4

#ifdef QUAKEWORLD
typedef struct client_s
{
	sv_client_state_t	state;

	int			spectator;		// non-interactive
	qboolean		sendinfo;		// at end of frame, send info to all
										// this prevents malicious multiple broadcasts
	float			lastnametime;		// time of last name change
	int			lastnamecount;		// time of last name change
	unsigned		checksum;		// checksum for calcs
	qboolean		drop;			// lose this guy next opportunity
	int			lossage;		// loss percentage

	int			userid;			// identifying number
	char			userinfo[MAX_INFO_STRING];	// infostring

	usercmd_t		lastcmd;		// for filling in big drops and partial predictions
	double			localtime;		// of last message
	int			oldbuttons;

	float			maxspeed;		// localized maxspeed
	float			entgravity;		// localized ent gravity

	edict_t			*edict;			// EDICT_NUM(clientnum+1)
	char			name[32];		// for printing to other people
										// extracted from userinfo
	int			messagelevel;		// for filtering printed messages

	// the datagram is written to after every frame, but only cleared
	// when it is sent out to the client.  overflow is tolerated.
	sizebuf_t		datagram;
	byte			datagram_buf[MAX_DATAGRAM];

	// back buffers for client reliable data
	sizebuf_t		backbuf;
	int			num_backbuf;
	int			backbuf_size[MAX_BACK_BUFFERS];
	byte			backbuf_data[MAX_BACK_BUFFERS][MAX_MSGLEN];

	double			connection_started;	// or time of disconnect for zombies
	qboolean		send_message;		// set on frames a datagram arived on

// spawn parms are carried from level to level
	float			spawn_parms[NUM_SPAWN_PARMS];

// client known data for deltas
	int			old_frags;

	int			stats[MAX_CL_STATS];


	client_frame_t		frames[UPDATE_BACKUP];	// updates can be deltad from here

	QFile			*download;		// file being downloaded
	int			downloadsize;		// total bytes
	int			downloadcount;		// bytes sent

	int			spec_track;		// entnum of player tracking

	double			whensaid[10];       // JACK: For floodprots
 	int			whensaidhead;       // Head value for floodprots
 	double			lockedtill;

	qboolean		upgradewarn;		// did we warn him?

	QFile			*upload;
	char			uploadfn[MAX_QPATH];
	netadr_t		snap_from;
	qboolean		remote_snap;

//===== NETWORK ============
	int			chokecount;
	int			delta_sequence;		// -1 = no compression
	int			ping;
	netchan_t		netchan;
	int			msecs, msec_cheating;
	double			last_check;
} client_t;
#else
typedef struct client_s
{
	qboolean		active;				// false = client is free
	qboolean		spawned;			// false = don't send datagrams
	qboolean		dropasap;			// has been told to go to another level
	qboolean		privileged;			// can execute any host command
	qboolean		sendsignon;			// only valid before spawned

	double			last_message;		// reliable messages must be sent
										// periodically

	struct qsocket_s *netconnection;	// communications handle

	usercmd_t		cmd;				// movement
	vec3_t			wishdir;			// intended motion calced from cmd

	sizebuf_t		message;			// can be added to at any time,
										// copied and clear once per frame
	byte			msgbuf[MAX_MSGLEN];
	edict_t			*edict;				// EDICT_NUM(clientnum+1)
	char			name[32];			// for printing to other people
	int				colors;

	float			ping_times[NUM_PING_TIMES];
	int				num_pings;			// ping_times[num_pings%NUM_PING_TIMES]

// spawn parms are carried from level to level
	float			spawn_parms[NUM_SPAWN_PARMS];

// client known data for deltas
	int				old_frags;
} client_t;
#endif

// a client can leave the server in one of four ways:
// dropping properly by quiting or disconnecting
// timing out if no valid messages are received for timeout.value seconds
// getting kicked off by the server operator
// a program error, like an overflowed reliable buffer

//=============================================================================

#define	STATFRAMES	100
typedef struct
{
	double	active;
	double	idle;
	int		count;
	int		packets;

	double	latched_active;
	double	latched_idle;
	int		latched_packets;
} svstats_t;

// MAX_CHALLENGES is made large to prevent a denial
// of service attack that could cycle all of them
// out before legitimate users connected
#define	MAX_CHALLENGES	1024

typedef struct
{
	netadr_t	adr;
	int			challenge;
	int			time;
} challenge_t;

#ifdef QUAKEWORLD
typedef struct
{
	int			spawncount;			// number of servers spawned since start,
									// used to check late spawns
	client_t	clients[MAX_CLIENTS];
	int			serverflags;		// episode completion information

	double		last_heartbeat;
	int			beatcount;
	int			mheartbeat_sequence;
	svstats_t	stats;

	char		info[MAX_SERVERINFO_STRING];

	// log messages are used so that fraglog processes can get stats
	int			logsequence;	// the message currently being filled
	double		logtime;		// time of last swap
	sizebuf_t	log[2];
	byte		log_buf[2][MAX_DATAGRAM];

	challenge_t	challenges[MAX_CHALLENGES];	// to prevent invalid IPs from connecting
} server_static_t;
#else
typedef struct
{
	int			maxclients;
	int			maxclientslimit;
	struct client_s	*clients;		// [maxclients]
	int			serverflags;		// episode completion information
	qboolean	changelevel_issued;	// cleared when at SV_SpawnServer
} server_static_t;
#endif

//=============================================================================

// HeartBeat Related things
#define HEARTBEAT_SECONDS		3
#define MAX_BEATCOUNT			100		// 5 Minutes


#define	MULTICAST_ALL			0
#define	MULTICAST_PHS			1
#define	MULTICAST_PVS			2

#define	MULTICAST_ALL_R			3
#define	MULTICAST_PHS_R			4
#define	MULTICAST_PVS_R			5

//============================================================================

#ifdef QUAKE2
// server flags
#define	SFL_EPISODE_1		1
#define	SFL_EPISODE_2		2
#define	SFL_EPISODE_3		4
#define	SFL_EPISODE_4		8
#define	SFL_NEW_UNIT		16
#define	SFL_NEW_EPISODE		32
#define	SFL_CROSS_TRIGGERS	65280
#endif

//============================================================================

extern	cvar_t	*teamplay;
extern	cvar_t	*skill;
extern	cvar_t	*deathmatch;
extern	cvar_t	*coop;
extern	cvar_t	*fraglimit;
extern	cvar_t	*timelimit;

extern	server_static_t	svs;				// persistant server info
extern	server_t		sv;					// local server

extern	client_t	*host_client;

extern	jmp_buf 	host_abortserver;

extern	double		host_time;

extern	edict_t		*sv_player;
extern	cvar_t	*sv_mintic, *sv_maxtic;
extern	cvar_t	*sv_maxspeed;

#ifdef QUAKEWORLD
extern	netadr_t	master_adr[MAX_MASTERS];	// address of the master server
#else
extern	netadr_t	master_adr;	// address of the master server
#endif

extern	cvar_t	*spawn;
extern	cvar_t	*teamplay;
extern	cvar_t	*deathmatch;
extern	cvar_t	*fraglimit;
extern	cvar_t	*timelimit;

extern	server_static_t	svs;				// persistant server info
extern	server_t		sv;					// local server

extern	client_t	*host_client;

extern	edict_t		*sv_player;

extern	char		localmodels[MAX_MODELS][5];	// inline model names for precache

extern	char		localinfo[MAX_LOCALINFO_STRING+1];

extern	int			host_hunklevel;
extern	QFile		*sv_logfile;
extern	QFile		*sv_fraglogfile;

//===========================================================
//
// sv_main.c
//

#ifdef QUAKEWORLD
void SV_AddGravity (edict_t *ent, float scale);
void SV_Shutdown (void);
void SV_DropClient (client_t *drop);
void SV_WriteClientdataToMessage (client_t *client, sizebuf_t *msg);
void SV_ClientPrintf (client_t *cl, int level, char *fmt, ...);
void SV_BroadcastPrintf (int level, char *fmt, ...);
void SV_Frame (double time);
void SV_Physics_Client (edict_t	*ent);
void SV_WriteEntitiesToClient (client_t *client, sizebuf_t *msg);
#else
void SV_AddGravity (edict_t *ent);
void SV_Shutdown (qboolean crash);
void SV_DropClient (qboolean crash);
void SV_WriteClientdataToMessage (edict_t *ent, sizebuf_t *msg);
void SV_ClientPrintf (char *fmt, ...);
void SV_BroadcastPrintf (char *fmt, ...);
void SV_Frame ( void );
void SV_Physics_Client (edict_t	*ent, int num);
void SV_WriteEntitiesToClient (edict_t	*clent, sizebuf_t *msg);
#endif

void SV_AddClientToServer (struct qsocket_s	*ret);
void SV_BeginRedirect (redirect_t rd);
void SV_BroadcastCommand (char *fmt, ...);
int SV_CalcPing (client_t *cl);
qboolean SV_CheckBottom (edict_t *ent);
void SV_CheckForNewClients (void);
void SV_CheckVelocity (edict_t *ent);
void SV_ClearDatagram (void);
void SV_ClientThink (void);
void SV_EndRedirect (void);
void SV_Error (char *error, ...);
void SV_ExecuteClientMessage (client_t *cl);
void SV_ExecuteUserCommand (char *s);
void SV_ExtractFromUserinfo (client_t *cl);
void SV_FinalMessage (char *message);
void SV_FindModelNumbers (void);
void SV_FlushSignon (void);
void SV_FullClientUpdate (client_t *client, sizebuf_t *buf);
void SV_FullClientUpdateToClient (client_t *client, client_t *cl);
void SV_Impact (edict_t *e1, edict_t *e2);
#ifdef QUAKEWORLD
void SV_Init (quakeparms_t *parms);
#else
void SV_Init (void);
#endif
void SV_InitOperatorCommands (void);
int SV_ModelIndex (char *name);
void SV_MoveToGoal (void);
void SV_Multicast (vec3_t origin, int to);
void SV_Physics (void);
void SV_Physics_Toss (edict_t *ent);
void SV_ProgStartFrame (void);
qboolean SV_RunThink (edict_t *ent);
void SV_RunClients (void);
void SV_RunNewmis (void);
void SV_SaveSpawnparms (void);
void SV_SendClientMessages (void);
void SV_SendMessagesToAll (void);
void SV_SendServerinfo (client_t *client);
void SV_SetIdealPitch (void);
void SV_SetMoveVars(void);
#ifdef QUAKE2
void SV_SpawnServer (char *server, char *startspot);
#else
void SV_SpawnServer (char *server);
#endif
void SV_StartParticle (vec3_t org, vec3_t dir, int color, int count);
void SV_StartSound (edict_t *entity, int channel, char *sample, int volume,
    float attenuation);
void SV_StartSound (edict_t *entity, int channel, char *sample, int volume,
    float attenuation);
void SV_Status_f (void);
void SV_TogglePause (const char *msg);
void SV_UserInit (void);

qboolean SV_movestep (edict_t *ent, vec3_t move, qboolean relink);
qboolean SV_movestep (edict_t *ent, vec3_t move, qboolean relink);

void HeartBeat (void);
void HeartBeat_Master (void);
void Master_Packet (void);
void Shutdown_Master (void);

void ClientReliableCheckBlock(client_t *cl, int maxsize);
void ClientReliable_FinishWrite(client_t *cl);
void ClientReliableWrite_Begin(client_t *cl, int c, int maxsize);
void ClientReliableWrite_Angle(client_t *cl, float f);
void ClientReliableWrite_Angle16(client_t *cl, float f);
void ClientReliableWrite_Byte(client_t *cl, int c);
void ClientReliableWrite_Char(client_t *cl, int c);
void ClientReliableWrite_Float(client_t *cl, float f);
void ClientReliableWrite_Coord(client_t *cl, float f);
void ClientReliableWrite_Long(client_t *cl, int c);
void ClientReliableWrite_Short(client_t *cl, int c);
void ClientReliableWrite_String(client_t *cl, char *s);
void ClientReliableWrite_SZ(client_t *cl, void *data, int len);
//
// host
//
extern	quakeparms_t host_parms;

extern	cvar_t		*sys_nostdout;

extern	qboolean	host_initialized;		// true if into command execution
extern	double		host_frametime;
extern	double		realtime;			// not bounded in any way, changed at
										// start of every frame, never reset


#endif // _SERVER_H
