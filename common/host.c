/*
	host.c
	
	(description)
	
	Copyright (C) 1999,2000  contributors of the QuakeForge project
	Please see the file "AUTHORS" for a list of contributors

	Author: Jeff Teunissen	<deek@quakeforge.net>
	Date:	09 Feb 2000
	
	This file is part of the QuakeForge Core system.
	
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
	Boston, MA  02111-1307, USA.
*/

#include <stdarg.h>
#include <stdio.h>
#include <setjmp.h>

#include <input.h>
#include <net.h>
#include <console.h>
#include <quakedef.h>
#ifdef UQUAKE
#include <server.h>
#endif
#include <client.h>
#include <view.h>
#include <wad.h>
#include <sound.h>
#include <cdaudio.h>
#include <keys.h>
#include <draw.h>
#include <screen.h>
#include <sbar.h>
#include <mathlib.h>
#include <menu.h>


extern int host_hunklevel;

jmp_buf 	host_abort;

double		realtime;				// without any filtering or bounding
double		oldrealtime;			// last frame run
qboolean	isDedicated;
qboolean	vid_initialized;
int			fps_count;
int 		vcrFile = -1;
double		host_time;

//cvar_t	sys_ticrate	= {"sys_ticrate","0.05"};
cvar_t	*sys_ticrate;
//cvar_t	serverprofile	= {"serverprofile", "0"};
cvar_t	*serverprofile;
//cvar_t	host_framerate	= {"host_framerate", "0"};	// set for slow motion
cvar_t	*host_framerate;
//cvar_t	samelevel	= {"samelevel", "0"};
cvar_t	*samelevel;
//cvar_t	noexit		= {"noexit", "0", CVAR_USERINFO|CVAR_SERVERINFO};
cvar_t	*noexit;
//cvar_t	pausable	= {"pausable", "1"};
cvar_t	*pausable;
//cvar_t	temp1		= {"temp1", "0"};
cvar_t *temp1;
//cvar_t	sv_filter	= {"sv_filter", "1"};	// strip \n, \r in names?
cvar_t *sv_filter;
//cvar_t	teamplay	= {"teamplay","0",CVAR_USERINFO|CVAR_SERVERINFO};
cvar_t	*teamplay;
//cvar_t	deathmatch	= {"deathmatch","0"};		// 0, 1, or 2
cvar_t	*deathmatch;
//cvar_t	coop		= {"coop","0"};			// 0 or 1
cvar_t	*coop;
//cvar_t	fraglimit	= {"fraglimit","0",CVAR_USERINFO|CVAR_SERVERINFO};
cvar_t	*fraglimit;
//cvar_t	skill		= {"skill","1"};		// 0 - 3
cvar_t	*skill;
//cvar_t	timelimit	= {"timelimit","0",CVAR_USERINFO|CVAR_SERVERINFO};
cvar_t	*timelimit;

#ifdef UQUAKE
client_t	*host_client;			// current client
#endif

void Host_InitLocal (void);
void Host_FindMaxClients (void);

#ifdef QUAKEWORLD
#define MAXTIME 0.2
#define MINTIME 0.0
#elif UQUAKE
#define MAXTIME 0.1
#define MINTIME 0.001
#endif
#define MAXFPS	72.0


/*
	Host_ClearMemory

	Clears all the memory used by both the client and server, but do not
	reinitialize anything.
*/
void
Host_ClearMemory (void)
{
	Con_DPrintf ("Clearing memory\n");
	D_FlushCaches ();
	Mod_ClearAll ();
	if (host_hunklevel)
		Hunk_FreeToLowMark (host_hunklevel);

#ifdef UQUAKE
	cls.signon = 0;
	
	memset (&sv, 0, sizeof(sv));
#endif
	memset (&cl, 0, sizeof(cl));
}


/*
	Host_EndGame

	On clients and non-dedicated servers, close currently-running game and
	drop to console. On dedicated servers, exit.
*/
void
Host_EndGame ( char *message, ... )
{
	va_list argptr;
	char	string[1024];

	va_start (argptr, message);
	vsnprintf (string, sizeof(string), message, argptr);
	va_end (argptr);
	
	Con_Printf ("\n===========================\n");
	Con_Printf ("Host_EndGame: %s\n", string);
	Con_Printf ("===========================\n\n");
	
#if QUAKEWORLD
	CL_Disconnect ();
	
	longjmp (host_abort, 1);
#elif UQUAKE
	if ( sv.active )
		SV_Shutdown (false);

	if ( cls.state == ca_dedicated )
		Sys_Error ("Host_EndGame: %s\n",string);	// dedicated servers exit
	
	if ( cls.demonum != -1 )
		CL_NextDemo ();
	else
		CL_Disconnect ();

	longjmp (host_abort, 1);
#endif
}

/*
===================
Host_GetConsoleCommands

Add them exactly as if they had been typed at the console
===================
*/
void Host_GetConsoleCommands (void)
{
	char	*cmd;

	while (1)
	{
		cmd = Sys_ConsoleInput ();
		if (!cmd)
			break;
		Cbuf_AddText (cmd);
	}
}

/*
	Host_FilterTime

	Returns false if the time is too short to run a frame
*/
qboolean
Host_FilterTime ( float time )
{
	float	fps;

	realtime += time;
	if ( oldrealtime > realtime )
		oldrealtime = 0;

	if (cl_maxfps->value)
		fps = max(30.0, min(cl_maxfps->value, MAXFPS));
	else
#ifdef QUAKEWORLD
		fps = max(30.0, min(rate->value/80.0, MAXFPS));
#elif UQUAKE
		fps = MAXFPS;
#endif

	if (!cls.timedemo && realtime - oldrealtime < 1.0/fps)
		return false;		// framerate is too high

	host_frametime = realtime - oldrealtime;
	oldrealtime = realtime;

#ifdef UQUAKE
	if (host_framerate->value > 0) {
		host_frametime = host_framerate->value;
	} else {		// don't allow really long or short frames
#endif
		host_frametime = min(MAXTIME, max(host_frametime, MINTIME));
#ifdef UQUAKE
	}
#endif
	
	return true;
}

/*
#ifdef QUAKEWORLD
int		nopacketcount;		// for Host_FrameMain
#endif
*/
/*
	Host_FrameMain

	Run everything that happens on a per-frame basis
*/
void
Host_FrameMain ( float time )
{
	static double	time1 = 0;
	static double	time2 = 0;
	static double	time3 = 0;
	int				pass1, pass2, pass3;

#ifdef UQUAKE
	cl_visedicts = cl_visedicts_list[0];
#endif
	if ( setjmp(host_abort) )
		return;			// something bad happened, or the server disconnected

#ifdef UQUAKE
	// keep the random time dependent
	rand ();
#endif

	// decide the simulation time
	if ( !Host_FilterTime(time) )
		return;
		
	// get new events
	IN_SendKeyEvents ();
	IN_Frame();

	// process console commands
	Cbuf_Execute ();

	// Poll server for results
#ifdef QUAKEWORLD
	CL_ReadPackets ();
#elif UQUAKE
	NET_Poll ();
#endif

	// send intentions now
#ifdef QUAKEWORLD
	// resend a connection request if necessary
	if (cls.state == ca_disconnected) {
		CL_CheckForResend ();
	} else {
		CL_SendCmd ();
	}

	// Set up prediction for other players
	CL_SetUpPlayerPrediction(false);

	// do client side motion prediction
	CL_PredictMove ();

	// Set up prediction for other players
	CL_SetUpPlayerPrediction(true);

	// build a refresh entity list
	CL_EmitEntities ();
#elif UQUAKE
	// if running the server locally, make intentions now
	if (sv.active)
		CL_SendCmd ();
	
	// check for commands typed to the host
	Host_GetConsoleCommands ();
	
	if (sv.active)
		SV_Frame ();		// Send frame to clients

	/*	
		if running the server remotely, send intentions now after incoming
		messages have been read
	*/
	if (!sv.active)
		CL_SendCmd ();

	host_time += host_frametime;

// fetch results from server
	if (cls.state >= ca_connected) {
		CL_ReadFromServer ();
	}
#endif

	// update video
	if (host_speeds->value)
		time1 = Sys_DoubleTime ();

	SCR_UpdateScreen ();

	if (host_speeds->value)
		time2 = Sys_DoubleTime ();
		
	// update audio
#ifdef QUAKEWORLD
	if (cls.state == ca_active)	{
#elif UQUAKE
	if (cls.signon == SIGNONS) {
#endif
		S_Update (r_origin, vpn, vright, vup);
		CL_DecayLights ();
	}
	else
		S_Update (vec3_origin, vec3_origin, vec3_origin, vec3_origin);
	
	CDAudio_Update();

	if (host_speeds->value) {
		pass1 = (time1 - time3)*1000;
		time3 = Sys_DoubleTime ();
		pass2 = (time2 - time1)*1000;
		pass3 = (time3 - time2)*1000;
		Con_Printf ("%3i tot %3i server %3i gfx %3i snd\n",
					pass1+pass2+pass3, pass1, pass2, pass3);
	}

	host_framecount++;
	fps_count++;
}


/*
	Host_Frame

	For QW, just call Host_FrameMain; for UQ, check if we want profiling
	information. If we do, time how long it took for the per-frame stuff
	to be handled and write it to console.
*/
void
Host_Frame ( float time )
{
#ifdef QUAKEWORLD
	Host_FrameMain (time);
	return;
#elif UQUAKE
	double			time1, time2;
	static double	timetotal;
	static int		timecount;
	int				i, c, m;

	if (!serverprofile->value) {
		Host_FrameMain (time);
		return;
	}
	
	time1 = Sys_DoubleTime ();
	Host_FrameMain (time);
	time2 = Sys_DoubleTime ();	
	
	timetotal += time2 - time1;
	timecount++;
	
	if (timecount < 1000)
		return;

	m = timetotal*1000/timecount;
	timecount = 0;
	timetotal = 0;
	c = 0;
	for (i=0 ; i < svs.maxclients ; i++) {
		if (svs.clients[i].active)
			c++;
	}

	Con_Printf ("serverprofile: %2i clients %2i msec\n",  c,  m);
#endif
}


/*
	Host_Error

	Shut down client. If server running, shut that down too.
*/
void
Host_Error ( char *error, ... )
{
	va_list 			argptr;
	char				string[1024];
	static qboolean 	inerror = false;
	
	if ( inerror ) {
		Sys_Error ("Host_Error: Called recursively from within an error");
	}
	inerror = true;
	
	va_start (argptr, error);
	vsnprintf (string, sizeof(string), error, argptr);
	va_end (argptr);
	Con_Printf ("Host_Error: %s\n", string);

#ifdef UQUAKE	
	if (sv.active)
		SV_Shutdown (false);

	if (cls.state == ca_dedicated)
		Sys_Error ("Host_Error: %s\n",string);	// dedicated servers exit
#endif
	
	CL_Disconnect ();
	cls.demonum = -1;

	inerror = false;

// FIXME
	Sys_Error ("Host_Error: %s\n",string);

#ifdef UQUAKE
	longjmp (host_abort, 1);
#endif
}


/*
	Host_InitVCR
	
	Set up playback and recording of demos.
*/
extern int vcrFile;
#define	VCR_SIGNATURE	0x56435231		// "VCR1"

void
Host_InitVCR ( quakeparms_t *parms )
{
	int		i, len, n;
	char	*p;
	
	if ( COM_CheckParm("-playback") ) {
		if ( com_argc != 2 )
			Sys_Error("No other parameters allowed with -playback\n");

		Sys_FileOpenRead("quake.vcr", &vcrFile);
		if ( vcrFile == -1 )
			Sys_Error("playback file not found\n");

		Sys_FileRead (vcrFile, &i, sizeof(int));
		if ( i != VCR_SIGNATURE )
			Sys_Error("Invalid signature in vcr file\n");

		Sys_FileRead (vcrFile, &com_argc, sizeof(int));
		com_argv = malloc(com_argc * sizeof(char *));
		com_argv[0] = parms->argv[0];
		for (i = 0 ; i < com_argc ; i++) {
			Sys_FileRead (vcrFile, &len, sizeof(int));
			p = malloc(len);
			Sys_FileRead (vcrFile, p, len);
			com_argv[i+1] = p;
		}
		com_argc++; /* add one for arg[0] */
		parms->argc = com_argc;
		parms->argv = com_argv;
	}

	n = COM_CheckParm("-record");
	if ( n ) {
		vcrFile = Sys_FileOpenWrite("quake.vcr");

		i = VCR_SIGNATURE;
		Sys_FileWrite(vcrFile, &i, sizeof(int));
		i = com_argc - 1;
		Sys_FileWrite(vcrFile, &i, sizeof(int));
		for (i = 1 ; i < com_argc ; i++) {
			if (i == n) {
				len = 10;
				Sys_FileWrite(vcrFile, &len, sizeof(int));
				Sys_FileWrite(vcrFile, "-playback", len);
				continue;
			}
			len = Q_strlen(com_argv[i]) + 1;
			Sys_FileWrite(vcrFile, &len, sizeof(int));
			Sys_FileWrite(vcrFile, com_argv[i], len);
		}
	}
}

/*
	Host_InitDisplay

	Video initialization
*/
void
Host_InitDisplay ()
{
}

/*
	Host_Init
	
	System Startup
*/
void
Host_Init (quakeparms_t *parms)
{
	COM_InitArgv (parms->argc, parms->argv);

#ifdef QUAKEWORLD
	COM_AddParm ("-game");
	COM_AddParm ("qw");
	Sys_mkdir("qw");
#endif

	if ( COM_CheckParm ("-minmemory") )
		parms->memsize = MINIMUM_MEMORY;
	
	host_parms = *parms;

	if (parms->memsize < MINIMUM_MEMORY)
		Sys_Error ("Only %4.1fMB of memory reported, can't execute game", parms->memsize / (float) 0x100000);
	
	Memory_Init (parms->membase, parms->memsize);
	CL_InitCvars();
	SCR_InitCvars();
	VID_InitCvars ();
	Cbuf_Init ();
	Cmd_Init ();
	V_Init ();
	
#ifdef QUAKEWORLD
	COM_Init ();

	NET_Init (PORT_CLIENT);
	Netchan_Init ();
#elif UQUAKE
	Chase_Init ();
	Host_InitVCR (parms);
	
	COM_Init ();
	Host_InitLocal ();
#endif

	W_LoadWadFile ("gfx.wad");
	Key_Init ();
	Con_Init ();	
	M_Init ();	

#ifdef UQUAKE	
	PR_Init ();
#endif
	
	Mod_Init ();

#ifdef UQUAKE
	NET_Init ();	
	SV_Init ();
#endif

	Con_Printf ("Exe: "__TIME__" "__DATE__"\n");
	Con_Printf ("%4.1f megabytes RAM used.\n", (parms->memsize / (1024 * 1024.0)) );
	
	R_InitTextures ();		// needed even for UQ dedicated server

#ifdef UQUAKE
	if (cls.state != ca_dedicated) {
#endif
		host_basepal = (byte *)COM_LoadHunkFile ("gfx/palette.lmp");
		if (!host_basepal)
			Sys_Error ("Couldn't load gfx/palette.lmp");
		host_colormap = (byte *)COM_LoadHunkFile ("gfx/colormap.lmp");
		if (!host_colormap)
			Sys_Error ("Couldn't load gfx/colormap.lmp");

	VID_Init(host_basepal);
	IN_Init();
	vid_initialized = true;

		Host_InitDisplay();
		S_Init();
		Draw_Init();
		SCR_Init();
		R_Init();

#ifdef QUAKEWORLD
		cls.state = ca_disconnected;
#endif

		CDAudio_Init();
		Sbar_Init();
		CL_Init();
#ifdef UQUAKE
	}
#endif

	Cbuf_InsertText ("exec quake.rc\n");

#ifdef QUAKEWORLD	
	Cbuf_AddText ("echo Type connect <internet address> or use GameSpy to connect to a game.\n");
	Cbuf_AddText ("cl_warncmd 1\n");
#endif

	Hunk_AllocName (0, "-HOST_HUNKLEVEL-");
	host_hunklevel = Hunk_LowMark ();
	host_initialized = true;

#ifdef QUAKEWORLD	
	Con_Printf ("\nClient Version %s\n\n", QF_VERSION);
#endif
	Sys_Printf ("======= QuakeForge Initialized =======\n");	
}


/*
	Host_Shutdown

	Callback from Sys_Quit and Sys_Error.  It would be better to run quit
	through here before the final handoff to the sys code.
*/
void
Host_Shutdown( void )
{
	static qboolean 	isdown = false;
	
	if ( isdown ) {
		printf ("recursive shutdown\n");
		return;
	}
	isdown = true;

#if UQUAKE
// keep Con_Printf from trying to update the screen
	scr_disabled_for_loading = true;
#endif

	Host_WriteConfiguration (); 
		
	CDAudio_Shutdown ();
	NET_Shutdown ();
	S_Shutdown();
	// Don't unload what hasn't been loaded!
	if (vid_initialized == true) {
		IN_Shutdown();
		//plugin_unload(IN->handle);
	}

#if QUAKEWORLD		
	if (host_basepal) {
#elif UQUAKE
	if (cls.state != ca_dedicated) {
#endif
		VID_Shutdown();
	}
	Cvar_Shutdown ();
}

/*
	Host_WriteConfiguration

	Write key bindings and archived cvars to config.cfg
*/
void
Host_WriteConfiguration ( void )
{
	QFile	*f;

	// Only write cvars if we are non-dedicated
	if (host_initialized & !isDedicated) {
		f = Qopen (va("%s/config.cfg",com_gamedir), "w");
		if ( !f ) {
			Con_Printf ("Couldn't write config.cfg.\n");
			return;
		}
		
		Key_WriteBindings (f);
		Cvar_WriteVariables (f);

		Qclose (f);
	}
}


#ifdef UQUAKE
/*
	Host_InitLocal
	
	(desc)
*/
void
Host_InitLocal ( void )
{
	Host_InitCommands ();
       
//	Cvar_RegisterVariable (&host_framerate);
	host_framerate = Cvar_Get ("host_framerate","0",0,"None");

//	Cvar_RegisterVariable (&sys_ticrate);
	sys_ticrate = Cvar_Get ("sys_ticrate","0.05",0,"None");
//	Cvar_RegisterVariable (&serverprofile);
	serverprofile = Cvar_Get ("serverprofile","0",0,"None");

//	Cvar_RegisterVariable (&fraglimit);
	fraglimit = Cvar_Get ("fraglimit","0",CVAR_USERINFO|CVAR_SERVERINFO,
				"None");
//	Cvar_RegisterVariable (&timelimit);
	timelimit = Cvar_Get ("timelimit","0",CVAR_USERINFO|CVAR_SERVERINFO,
				"None");
//	Cvar_RegisterVariable (&teamplay);
	teamplay = Cvar_Get ("teamplay","0",CVAR_USERINFO|CVAR_SERVERINFO,
				"None");
//	Cvar_RegisterVariable (&samelevel);
	samelevel = Cvar_Get ("samelevel","0",0,"None");
//	Cvar_RegisterVariable (&noexit);
	noexit = Cvar_Get ("noexit","0",CVAR_USERINFO|CVAR_SERVERINFO,"None");
//	Cvar_RegisterVariable (&skill);
	skill = Cvar_Get ("skill","1",0,"None");
//	Cvar_RegisterVariable (&deathmatch);
	deathmatch = Cvar_Get ("deathmatch","0",0,"None");
//	Cvar_RegisterVariable (&coop);
	coop = Cvar_Get ("coop","0",0,"None");

//	Cvar_RegisterVariable (&pausable);
	pausable = Cvar_Get ("pausable","1",0,"None");

//	Cvar_RegisterVariable (&temp1);
	temp1 = Cvar_Get ("temp1","0",0,"None");
//	Cvar_RegisterVariable (&sv_filter);
	sv_filter = Cvar_Get ("sv_filter","1",0,"None");

	Host_FindMaxClients ();
	
	host_time = 1.0;		// so a think at time 0 won't get called
}


/*
	Host_FindMaxClients
	
	(desc)
*/
void	Host_FindMaxClients (void)
{
	int		i;

	svs.maxclients = 1;
		
	i = COM_CheckParm ("-dedicated");
	if (i)
	{
		cls.state = ca_dedicated;
		if (i != (com_argc - 1))
		{
			svs.maxclients = Q_atoi (com_argv[i+1]);
		}
		else
			svs.maxclients = 8;
	}
	else
		cls.state = ca_disconnected;

	i = COM_CheckParm ("-listen");
	if (i)
	{
		if (cls.state == ca_dedicated)
			Sys_Error ("Only one of -dedicated or -listen can be specified");
		if (i != (com_argc - 1))
			svs.maxclients = Q_atoi (com_argv[i+1]);
		else
			svs.maxclients = 8;
	}
	if (svs.maxclients < 1)
		svs.maxclients = 8;
	else if (svs.maxclients > MAX_SCOREBOARD)
		svs.maxclients = MAX_SCOREBOARD;

	svs.maxclientslimit = svs.maxclients;
	if (svs.maxclientslimit < 4)
		svs.maxclientslimit = 4;
	svs.clients = Hunk_AllocName (svs.maxclientslimit*sizeof(client_t), "clients");

	if (svs.maxclients > 1)
		deathmatch->value = 1.0;
	else
		deathmatch->value = 0.0;
}

/*
=================
Host_ClientCommands

Send text over to the client to be executed
=================
*/
void Host_ClientCommands (char *fmt, ...)
{
	va_list		argptr;
	char		string[1024];
	
	va_start (argptr, fmt);
	vsnprintf (string, sizeof(string), fmt, argptr);
	va_end (argptr);
	
	MSG_WriteByte (&host_client->message, svc_stufftext);
	MSG_WriteString (&host_client->message, string);
}
#endif
