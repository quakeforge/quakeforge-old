/*
	cl_cam.c

	Player camera tracking in Spectator mode

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

/* ZOID
 *
 * This takes over player controls for spectator automatic camera.
 * Player moves as a spectator, but the camera tracks and enemy player
 */

#include <qtypes.h>
#include <quakedef.h>
#include <winquake.h>
#include <mathlib.h>
#include <client.h>
#include <sbar.h>
#include <cvar.h>
#include <console.h>
#include <world.h>

#define	PM_SPECTATORMAXSPEED	500
#define	PM_STOPSPEED	100
#define	PM_MAXSPEED			320
#define BUTTON_JUMP 2
#define BUTTON_ATTACK 1
#define MAX_ANGLE_TURN 10

static vec3_t desired_position;		// where the camera wants to be
static qboolean locked = false;
static int oldbuttons;

// track high fragger
cvar_t *cl_hightrack;

qboolean cam_forceview;
vec3_t cam_viewangles;
double cam_lastviewtime;

int spec_track = 0;		// player# of who we are tracking
int autocam = CAM_NONE;

cvar_t * cl_chasecam;
cvar_t *cl_chasecam_up;
cvar_t *cl_chasecam_back;
cvar_t *cl_chasecam_right;
cvar_t *cl_chasecam_mode;
vec3_t	chase_pos;
vec3_t	chase_angles;

vec3_t	chase_dest;
vec3_t	chase_dest_angles;

void
Chase_Init ( void )
{
	cl_chasecam = Cvar_Get ("cl_chasecam", "0", CVAR_ARCHIVE,
			"Enable the chasecam (boolean)");
	cl_chasecam_up = Cvar_Get ("cl_chasecam_up","16",CVAR_ARCHIVE,
			"Determines the height of the chasecam");
	cl_chasecam_back = Cvar_Get ("cl_chasecam_back","100",CVAR_ARCHIVE,
			"Determines how far away the chasecam is away from the player on the axis parallel to the ground.");
	cl_chasecam_right = Cvar_Get ("cl_chasecam_right","0", CVAR_ARCHIVE,
			"Pans the chasecam to the left/right of the player in chasecam mode.");
	cl_chasecam_mode = Cvar_Get ("cl_chasecam_mode", "0", CVAR_ARCHIVE, "Determines the mode used for the chasecam.\nRear (normal)=0\nOverhead=1\n----\nSettings recommended:\nIf using overhead set cl_chasecam_back 0, otherwise set to 100\nalso if you use overhead, a cl_chasecam_up of 100 is nice, otherwise use a value between 0 and 20");
}

void
Chase_Reset ( void )
{
	// for respawning and teleporting
	//	start position 12 units behind head
}

void
TraceLine ( vec3_t start, vec3_t end, vec3_t impact )
{
	trace_t	trace;

	memset (&trace, 0, sizeof(trace));
	SV_RecursiveHullCheck (cl.worldmodel->hulls, 0, 0, 1, start, end, &trace);

	VectorCopy (trace.endpos, impact);
}

void
Chase_Update ( void )
{
	int i;
	float	dist;
	vec3_t	forward, up, right;
	vec3_t	dest, stop;

	// Save this for sideview
	// int dir=1;

	// if can't see player, reset
	AngleVectors (cl.viewangles, forward, right, up);

	// calc exact destination

// This is for the side-view code which isn't working yet.
/*
	if(dir==-1&&cl.viewangles[YAW]<210&&cl.viewangles[YAW]>150)
		dir=-1;
*/

	if(cl_chasecam_mode->value!=0)
	{
		chase_dest[0] = r_refdef.vieworg[0] - cl_chasecam_right->value;
		chase_dest[1] = r_refdef.vieworg[1] - cl_chasecam_back->value;
		chase_dest[2] = r_refdef.vieworg[2] + cl_chasecam_up->value;
	}
	else
	{
		for (i=0 ; i<3 ; i++)
			chase_dest[i] = r_refdef.vieworg[i]
				- forward[i] * cl_chasecam_back->value
				- right[i] * cl_chasecam_right->value;
	}

	// find the spot the player is looking at
	VectorMA (r_refdef.vieworg, 4096, forward, dest);
	TraceLine (r_refdef.vieworg, dest, stop);

	// calculate pitch to look at the same spot from camera
	VectorSubtract (stop, r_refdef.vieworg, stop);
	dist = DotProduct (stop, forward);
	if (dist < 1)
		dist = 1;

	// Yaw stuff here is/was for the side view which i'm still working on.

// This here flipps the direction of the overhead.. might want this later but for yaw
//	r_refdef.viewangles[YAW]=-atan(stop[2] / dist) / M_PI * 180;

	// Top View :)
	if(cl_chasecam_mode->value==1)
		r_refdef.viewangles[PITCH]=90;
	else 	// If not top view..
		r_refdef.viewangles[PITCH] = -atan(stop[2] / dist) / M_PI * 180;


	// move towards destination
	VectorCopy (chase_dest, r_refdef.vieworg);

}

static void
vectoangles ( vec3_t vec, vec3_t ang )
{
	float	forward;
	float	yaw, pitch;

	if (vec[1] == 0 && vec[0] == 0)
	{
		yaw = 0;
		if (vec[2] > 0)
			pitch = 90;
		else
			pitch = 270;
	}
	else
	{
		yaw = (int) (atan2(vec[1], vec[0]) * 180 / M_PI);
		if (yaw < 0)
			yaw += 360;

		forward = sqrt (vec[0]*vec[0] + vec[1]*vec[1]);
		pitch = (int) (atan2(vec[2], forward) * 180 / M_PI);
		if (pitch < 0)
			pitch += 360;
	}

	ang[0] = pitch;
	ang[1] = yaw;
	ang[2] = 0;
}

static float
vlen ( vec3_t v )
{
	return sqrt(v[0]*v[0] + v[1]*v[1] + v[2]*v[2]);
}

// returns true if weapon model should be drawn in camera mode
qboolean
Cam_DrawViewModel ( void )
{
	if (!cl.spectator)
		return true;

	if (autocam && locked && cl_chasecam->value)
		return true;
	return false;
}

// returns true if we should draw this player, we don't if we are chase camming
qboolean
Cam_DrawPlayer ( int playernum )
{
	if (cl.spectator && autocam && locked && cl_chasecam->value &&
		spec_track == playernum)
		return false;
	return true;
}

void
Cam_Unlock ( void )
{
	if (autocam) {
		MSG_WriteByte (&cls.netchan.message, clc_stringcmd);
		MSG_WriteString (&cls.netchan.message, "ptrack");
		autocam = CAM_NONE;
		locked = false;
		Sbar_Changed();
	}
}

void
Cam_Lock ( int playernum )
{
	char st[40];

	snprintf(st, sizeof(st), "ptrack %i", playernum);
	MSG_WriteByte (&cls.netchan.message, clc_stringcmd);
	MSG_WriteString (&cls.netchan.message, st);
	spec_track = playernum;
	cam_forceview = true;
	locked = false;
	Sbar_Changed();
}

pmtrace_t
Cam_DoTrace ( vec3_t vec1, vec3_t vec2 )
{
#if 0
	memset(&pmove, 0, sizeof(pmove));

	pmove.numphysent = 1;
	VectorCopy (vec3_origin, pmove.physents[0].origin);
	pmove.physents[0].model = cl.worldmodel;
#endif

	VectorCopy (vec1, pmove.origin);
	return PM_PlayerMove(pmove.origin, vec2);
}

// Returns distance or 9999 if invalid for some reason
static float
Cam_TryFlyby ( player_state_t *self, player_state_t *player, vec3_t vec, qboolean checkvis )
{
	vec3_t v;
	pmtrace_t trace;
	float len;

	vectoangles(vec, v);
//	v[0] = -v[0];
	VectorCopy (v, pmove.angles);
	VectorNormalize(vec);
	VectorMA(player->origin, 800, vec, v);
	// v is endpos
	// fake a player move
	trace = Cam_DoTrace(player->origin, v);
	if (/*trace.inopen ||*/ trace.inwater)
		return 9999;
	VectorCopy(trace.endpos, vec);
	VectorSubtract(trace.endpos, player->origin, v);
	len = sqrt(v[0]*v[0] + v[1]*v[1] + v[2]*v[2]);
	if (len < 32 || len > 800)
		return 9999;
	if (checkvis) {
		VectorSubtract(trace.endpos, self->origin, v);
		len = sqrt(v[0]*v[0] + v[1]*v[1] + v[2]*v[2]);

		trace = Cam_DoTrace(self->origin, vec);
		if (trace.fraction != 1 || trace.inwater)
			return 9999;
	}
	return len;
}

// Is player visible?
static qboolean
Cam_IsVisible ( player_state_t *player, vec3_t vec )
{
	pmtrace_t trace;
	vec3_t v;
	float d;

	trace = Cam_DoTrace(player->origin, vec);
	if (trace.fraction != 1 || /*trace.inopen ||*/ trace.inwater)
		return false;
	// check distance, don't let the player get too far away or too close
	VectorSubtract(player->origin, vec, v);
	d = vlen(v);
	if (d < 16)
		return false;

	return true;
}

static qboolean
InitFlyby ( player_state_t *self, player_state_t *player, int checkvis )
{
	float f, max;
	vec3_t vec, vec2;
	vec3_t forward, right, up;

	if(cl_chasecam_mode->value>0)
		 return 0;

	VectorCopy(player->viewangles, vec);
	vec[0] = 0;
	AngleVectors (vec, forward, right, up);
//	for (i = 0; i < 3; i++)
//		forward[i] *= 3;

	max = 1000;
	VectorAdd(forward, up, vec2);
	VectorAdd(vec2, right, vec2);
	if ((f = Cam_TryFlyby(self, player, vec2, checkvis)) < max) {
		max = f;
		VectorCopy(vec2, vec);
	}
	VectorAdd(forward, up, vec2);
	VectorSubtract(vec2, right, vec2);
	if ((f = Cam_TryFlyby(self, player, vec2, checkvis)) < max) {
		max = f;
		VectorCopy(vec2, vec);
	}
	VectorAdd(forward, right, vec2);
	if ((f = Cam_TryFlyby(self, player, vec2, checkvis)) < max) {
		max = f;
		VectorCopy(vec2, vec);
	}
	VectorSubtract(forward, right, vec2);
	if ((f = Cam_TryFlyby(self, player, vec2, checkvis)) < max) {
		max = f;
		VectorCopy(vec2, vec);
	}
	VectorAdd(forward, up, vec2);
	if ((f = Cam_TryFlyby(self, player, vec2, checkvis)) < max) {
		max = f;
		VectorCopy(vec2, vec);
	}
	VectorSubtract(forward, up, vec2);
	if ((f = Cam_TryFlyby(self, player, vec2, checkvis)) < max) {
		max = f;
		VectorCopy(vec2, vec);
	}
	VectorAdd(up, right, vec2);
	VectorSubtract(vec2, forward, vec2);
	if ((f = Cam_TryFlyby(self, player, vec2, checkvis)) < max) {
		max = f;
		VectorCopy(vec2, vec);
	}
	VectorSubtract(up, right, vec2);
	VectorSubtract(vec2, forward, vec2);
	if ((f = Cam_TryFlyby(self, player, vec2, checkvis)) < max) {
		max = f;
		VectorCopy(vec2, vec);
	}
	// invert
	VectorSubtract(vec3_origin, forward, vec2);
	if ((f = Cam_TryFlyby(self, player, vec2, checkvis)) < max) {
		max = f;
		VectorCopy(vec2, vec);
	}
	VectorCopy(forward, vec2);
	if ((f = Cam_TryFlyby(self, player, vec2, checkvis)) < max) {
		max = f;
		VectorCopy(vec2, vec);
	}
	// invert
	VectorSubtract(vec3_origin, right, vec2);
	if ((f = Cam_TryFlyby(self, player, vec2, checkvis)) < max) {
		max = f;
		VectorCopy(vec2, vec);
	}
	VectorCopy(right, vec2);
	if ((f = Cam_TryFlyby(self, player, vec2, checkvis)) < max) {
		max = f;
		VectorCopy(vec2, vec);
	}

	// ack, can't find him
	if (max >= 1000) {
//		Cam_Unlock();
		return false;
	}
	locked = true;
	VectorCopy(vec, desired_position);

	return true;
}

static void
Cam_CheckHighTarget ( void )
{
	int i, j, max;
	player_info_t	*s;

	j = -1;
	for (i = 0, max = -9999; i < MAX_CLIENTS; i++) {
		s = &cl.players[i];
		if (s->name[0] && !s->spectator && s->frags > max) {
			max = s->frags;
			j = i;
		}
	}
	if (j >= 0) {
		if (!locked || cl.players[j].frags > cl.players[spec_track].frags)
			Cam_Lock(j);
	} else
		Cam_Unlock();
}

// ZOID
//
// Take over the user controls and track a player.
// We find a nice position to watch the player and move there
void
Cam_Track ( usercmd_t *cmd )
{
	player_state_t *player, *self;
	frame_t *frame;
	vec3_t vec;
	float len;

	if(cl_chasecam->value>0)
		return;

	if (!cl.spectator)
		return;

	if (cl_hightrack->value && !locked)
		Cam_CheckHighTarget();

	if (!autocam || cls.state != ca_active)
		return;

	if (locked && (!cl.players[spec_track].name[0] || cl.players[spec_track].spectator)) {
		locked = false;
		if (cl_hightrack->value)
			Cam_CheckHighTarget();
		else
			Cam_Unlock();
		return;
	}

	frame = &cl.frames[cls.netchan.incoming_sequence & UPDATE_MASK];
	player = frame->playerstate + spec_track;
	self = frame->playerstate + cl.playernum;

	if (!locked || !Cam_IsVisible(player, desired_position)) {
		if (!locked || realtime - cam_lastviewtime > 0.1) {
			if (!InitFlyby(self, player, true))
				InitFlyby(self, player, false);
			cam_lastviewtime = realtime;
		}
	} else
		cam_lastviewtime = realtime;

	// couldn't track for some reason
	if (!locked || !autocam)
		return;

	if (cl_chasecam->value) {
		cmd->forwardmove = cmd->sidemove = cmd->upmove = 0;

		VectorCopy(player->viewangles, cl.viewangles);
		VectorCopy(player->origin, desired_position);
		if (memcmp(desired_position, self->origin, sizeof(desired_position)) != 0) {
			MSG_WriteByte (&cls.netchan.message, clc_tmove);
			MSG_WriteCoord (&cls.netchan.message, desired_position[0]);
			MSG_WriteCoord (&cls.netchan.message, desired_position[1]);
			MSG_WriteCoord (&cls.netchan.message, desired_position[2]);
			// move there locally immediately
			VectorCopy(desired_position, self->origin);
		}
		self->weaponframe = player->weaponframe;

	} else {
		// Ok, move to our desired position and set our angles to view
		// the player
		VectorSubtract(desired_position, self->origin, vec);
		len = vlen(vec);
		cmd->forwardmove = cmd->sidemove = cmd->upmove = 0;
		if (len > 16) {		// close enough?
			MSG_WriteByte (&cls.netchan.message, clc_tmove);
			MSG_WriteCoord (&cls.netchan.message, desired_position[0]);
			MSG_WriteCoord (&cls.netchan.message, desired_position[1]);
			MSG_WriteCoord (&cls.netchan.message, desired_position[2]);
		}

		// move there locally immediately
		VectorCopy(desired_position, self->origin);

		VectorSubtract(player->origin, desired_position, vec);
		vectoangles(vec, cl.viewangles);
		cl.viewangles[0] = -cl.viewangles[0];
	}
}

#if 0
static float
adjustang ( float current, float ideal, float speed )
{
	float move;

	current = anglemod(current);
	ideal = anglemod(ideal);

	if (current == ideal)
		return current;

	move = ideal - current;
	if (ideal > current)
	{
		if (move >= 180)
			move = move - 360;
	}
	else
	{
		if (move <= -180)
			move = move + 360;
	}
	if (move > 0)
	{
		if (move > speed)
			move = speed;
	}
	else
	{
		if (move < -speed)
			move = -speed;
	}

//	Con_Printf("c/i: %4.2f/%4.2f move: %4.2f\n", current, ideal, move);
	return anglemod (current + move);
}
#endif

#if 0
void
Cam_SetView ( void )
{
	return;
	player_state_t *player, *self;
	frame_t *frame;
	vec3_t vec, vec2;

	if (cls.state != ca_active || !cl.spectator ||
		!autocam || !locked)
		return;

	frame = &cl.frames[cls.netchan.incoming_sequence & UPDATE_MASK];
	player = frame->playerstate + spec_track;
	self = frame->playerstate + cl.playernum;

	VectorSubtract(player->origin, cl.simorg, vec);
	if (cam_forceview) {
		cam_forceview = false;
		vectoangles(vec, cam_viewangles);
		cam_viewangles[0] = -cam_viewangles[0];
	} else {
		vectoangles(vec, vec2);
		vec2[PITCH] = -vec2[PITCH];

		cam_viewangles[PITCH] = adjustang(cam_viewangles[PITCH], vec2[PITCH], cl_camera_maxpitch->value);
		cam_viewangles[YAW] = adjustang(cam_viewangles[YAW], vec2[YAW], cl_camera_maxyaw->value);
	}
	VectorCopy(cam_viewangles, cl.viewangles);
	VectorCopy(cl.viewangles, cl.simangles);
}
#endif

void
Cam_FinishMove ( usercmd_t *cmd )
{
	int i;
	player_info_t	*s;
	int end;

	if (cls.state != ca_active)
		return;

	if (!cl.spectator)	// only in spectator mode
		return;

#if 0
	if (autocam && locked) {
		frame = &cl.frames[cls.netchan.incoming_sequence & UPDATE_MASK];
		player = frame->playerstate + spec_track;
		self = frame->playerstate + cl.playernum;

		VectorSubtract(player->origin, self->origin, vec);
		if (cam_forceview) {
			cam_forceview = false;
			vectoangles(vec, cam_viewangles);
			cam_viewangles[0] = -cam_viewangles[0];
		} else {
			vectoangles(vec, vec2);
			vec2[PITCH] = -vec2[PITCH];

			cam_viewangles[PITCH] = adjustang(cam_viewangles[PITCH], vec2[PITCH], cl_camera_maxpitch->value);
			cam_viewangles[YAW] = adjustang(cam_viewangles[YAW], vec2[YAW], cl_camera_maxyaw->value);
		}
		VectorCopy(cam_viewangles, cl.viewangles);
	}
#endif

	if (cmd->buttons & BUTTON_ATTACK) {
		if (!(oldbuttons & BUTTON_ATTACK)) {

			oldbuttons |= BUTTON_ATTACK;
			autocam++;

			if (autocam > CAM_TRACK) {
				Cam_Unlock();
				VectorCopy(cl.viewangles, cmd->angles);
				return;
			}
		} else
			return;
	} else {
		oldbuttons &= ~BUTTON_ATTACK;
		if (!autocam)
			return;
	}

	if (autocam && cl_hightrack->value) {
		Cam_CheckHighTarget();
		return;
	}

	if (locked) {
		if ((cmd->buttons & BUTTON_JUMP) && (oldbuttons & BUTTON_JUMP))
			return;		// don't pogo stick

		if (!(cmd->buttons & BUTTON_JUMP)) {
			oldbuttons &= ~BUTTON_JUMP;
			return;
		}
		oldbuttons |= BUTTON_JUMP;	// don't jump again until released
	}

//	Con_Printf("Selecting track target...\n");

	if (locked && autocam)
		end = (spec_track + 1) % MAX_CLIENTS;
	else
		end = spec_track;
	i = end;
	do {
		s = &cl.players[i];
		if (s->name[0] && !s->spectator) {
			Cam_Lock(i);
			return;
		}
		i = (i + 1) % MAX_CLIENTS;
	} while (i != end);
	// stay on same guy?
	i = spec_track;
	s = &cl.players[i];
	if (s->name[0] && !s->spectator) {
		Cam_Lock(i);
		return;
	}
	Con_Printf("No target found ...\n");
	autocam = locked = false;
}

void
Cam_Reset ( void )
{
	autocam = CAM_NONE;
	spec_track = 0;
}

void
CL_InitCam ( void )
{
	cl_hightrack = Cvar_Get ("cl_hightrack","0",0,"None");
}
