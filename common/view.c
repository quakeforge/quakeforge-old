/*
	view.c

	player eye positioning

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

#include <quakedef.h>
#include <r_local.h>
#include <draw.h>
#include <mathlib.h>
#include <qtypes.h>
#include <qstructs.h>
#include <cmd.h>
#include <screen.h>
#include <console.h>
#include <cvar.h>

extern int	onground;

/*

The view is allowed to move slightly from it's true position for bobbing,
but if it exceeds 8 pixels linear distance (spherical, not box), the list of
entities sent from the server may not include everything in the pvs, especially
when crossing a water boudnary.

*/

cvar_t	*lcd_x;			// FIXME: make this work sometime...
cvar_t	*lcd_yaw;

cvar_t	*scr_ofsx;
cvar_t	*scr_ofsy;
cvar_t	*scr_ofsz;

cvar_t	*cl_rollspeed;
cvar_t	*cl_rollangle;

cvar_t	*cl_bob;
cvar_t	*cl_bobcycle;
cvar_t	*cl_bobup;

cvar_t	*v_kicktime;
cvar_t	*v_kickroll;
cvar_t	*v_kickpitch;

cvar_t	*v_iyaw_cycle;
cvar_t	*v_iroll_cycle;
cvar_t	*v_ipitch_cycle;
cvar_t	*v_iyaw_level;
cvar_t	*v_iroll_level;
cvar_t	*v_ipitch_level;

cvar_t	*v_idlescale;

cvar_t	*crosshair;
cvar_t	*crosshaircolor;

cvar_t	*cl_crossx;
cvar_t	*cl_crossy;

cvar_t	*gl_cshiftpercent;

cvar_t	*v_contentblend;

float	v_dmg_time, v_dmg_roll, v_dmg_pitch;

extern	int			in_forward, in_forward2, in_back;

frame_t				*view_frame;
player_state_t		*view_message;

/*
	V_CalcRoll

	(desc)
*/
float
V_CalcRoll (vec3_t angles, vec3_t velocity)
{
	vec3_t	forward, right, up;
	float	sign;
	float	side;
	float	value;

	AngleVectors (angles, forward, right, up);
	side = DotProduct (velocity, right);
	sign = side < 0 ? -1 : 1;
	side = fabs(side);

	value = cl_rollangle->value;

	if (side < cl_rollspeed->value)
		side = side * value / cl_rollspeed->value;
	else
		side = value;

	return side*sign;

}


/*
	V_CalcBob

	(desc)
*/
float
V_CalcBob ( void )
{
#ifdef QUAKEWORLD
	static	double	bobtime;
	static float	bob;
#else
	float	bob;
#endif
	float	cycle;

#ifdef QUAKEWORLD
	if (cl.spectator)
		return 0;

	if (onground == -1)
		return bob;		// just use old value

	bobtime += host_frametime;
	cycle = bobtime - (int)(bobtime/cl_bobcycle->value)*cl_bobcycle->value;
#else
	cycle = cl.time - (int)(cl.time/cl_bobcycle->value)*cl_bobcycle->value;
#endif
	cycle /= cl_bobcycle->value;
	if (cycle < cl_bobup->value)
		cycle = M_PI * cycle / cl_bobup->value;
	else
		cycle = M_PI + M_PI*(cycle-cl_bobup->value)/(1.0 - cl_bobup->value);

// bob is proportional to [simulated] velocity in the xy plane
// (don't count Z, or jumping messes it up)

#ifdef QUAKEWORLD
	bob = sqrt(cl.simvel[0]*cl.simvel[0] + cl.simvel[1]*cl.simvel[1]) * cl_bob->value;
#else
	bob = sqrt(cl.velocity[0]*cl.velocity[0] + cl.velocity[1]*cl.velocity[1]) * cl_bob->value;
#endif
	bob = bob*0.3 + bob*0.7*sin(cycle);
	bob = bound( -7, bob, 4);
	return bob;

}


//=============================================================================


cvar_t	*v_centermove;
cvar_t	*v_centerspeed;

void
V_StartPitchDrift ( void )
{
	if (cl.laststop == cl.time) {
		return;		// something else is keeping it from drifting
	}

	if (cl.nodrift || !cl.pitchvel) {
		cl.pitchvel = v_centerspeed->value;
		cl.nodrift = false;
		cl.driftmove = 0;
	}
}

void
V_StopPitchDrift (void)
{
	cl.laststop = cl.time;
	cl.nodrift = true;
	cl.pitchvel = 0;
}

/*
	V_DriftPitch

	Move client pitch angle towards cl.idealpitch sent by the server.

	If the user is adjusting pitch manually, either with lookup/lookdown,
	mlook and mouse, or klook and keyboard, pitch drifting is constantly
	stopped.

	Drifting is enabled when the center view key is hit, mlook is released
	and lookspring is nonzero, or when
*/
void
V_DriftPitch ( void )
{
	float		delta, move;

#ifdef QUAKEWORLD
	if (view_message->onground == -1 || cls.demoplayback ) {
#else
	if (noclip_anglehack || !cl.onground || cls.demoplayback ) {
#endif
		cl.driftmove = 0;
		cl.pitchvel = 0;
		return;
	}

	// don't count small mouse motion
	if ( cl.nodrift ) {
#ifdef QUAKEWORLD
		if ( fabs(cl.frames[(cls.netchan.outgoing_sequence-1)&UPDATE_MASK].cmd.forwardmove) < 200)
#else
		if ( fabs(cl.cmd.forwardmove) < cl_forwardspeed->value)
#endif
			cl.driftmove = 0;
		else
			cl.driftmove += host_frametime;

		if ( cl.driftmove > v_centermove->value) {
			V_StartPitchDrift ();
		}
		return;
	}

	delta = cl.idealpitch - cl.viewangles[PITCH];

	if ( !delta ) {
		cl.pitchvel = 0;
		return;
	}

	move = host_frametime * cl.pitchvel;
	cl.pitchvel += host_frametime * v_centerspeed->value;

	//Con_Printf ("move: %f (%f)\n", move, host_frametime);

	if (delta > 0) {
		if (move > delta)
		{
			cl.pitchvel = 0;
			move = delta;
		}
		cl.viewangles[PITCH] += move;
	} else if (delta < 0) {
		if (move > -delta) {
			cl.pitchvel = 0;
			move = -delta;
		}
		cl.viewangles[PITCH] -= move;
	}
}


/*
==============================================================================

						PALETTE FLASHES

==============================================================================
*/


cshift_t	cshift_empty = { {130,80,50}, 0 };
cshift_t	cshift_water = { {130,80,50}, 128 };
cshift_t	cshift_slime = { {0,25,5}, 150 };
cshift_t	cshift_lava = { {255,80,0}, 150 };

cvar_t		*v_gamma;

byte		gammatable[256];	// palette is sent through this

byte		ramps[3][256];
float		v_blend[4];		// rgba 0.0 - 1.0

void
BuildGammaTable ( float g )
{
	int		i, inf;

	if (g == 1.0) {
		for (i=0 ; i<256 ; i++)
			gammatable[i] = i;
		return;
	}

	for (i=0 ; i<256 ; i++) {
		inf = 255 * pow ( (i+0.5)/255.5 , g ) + 0.5;
		inf = bound(0, inf, 255);
		gammatable[i] = inf;
	}
}

/*
	V_CheckGamma

	(desc)
*/
qboolean
V_CheckGamma ( void )
{
	static float oldgammavalue;

	if (v_gamma->value == oldgammavalue)
		return false;
	oldgammavalue = v_gamma->value;

	BuildGammaTable (v_gamma->value);
	vid.recalc_refdef = 1;				// force a surface cache flush

	return true;
}


/*
	V_ParseDamage

	(desc)
*/
void
V_ParseDamage ( void )
{
	int		armor, blood;
	vec3_t	from;
	int		i;
	vec3_t	forward, right, up;
#ifdef UQUAKE
	entity_t	*ent;
#endif
	float	side;
	float	count;

	armor = MSG_ReadByte ();
	blood = MSG_ReadByte ();
	for (i=0 ; i<3 ; i++)
		from[i] = MSG_ReadCoord ();

	count = blood*0.5 + armor*0.5;
	count = min(10, count);

	cl.faceanimtime = cl.time + 0.2;		// but sbar face into pain frame

	cl.cshifts[CSHIFT_DAMAGE].percent += 3*count;
	cl.cshifts[CSHIFT_DAMAGE].percent = bound(0, cl.cshifts[CSHIFT_DAMAGE].percent, 150);

	if ( armor > blood ) {
		cl.cshifts[CSHIFT_DAMAGE].destcolor[0] = 200;
		cl.cshifts[CSHIFT_DAMAGE].destcolor[1] = 100;
		cl.cshifts[CSHIFT_DAMAGE].destcolor[2] = 100;
	} else if ( armor ) {
		cl.cshifts[CSHIFT_DAMAGE].destcolor[0] = 220;
		cl.cshifts[CSHIFT_DAMAGE].destcolor[1] = 50;
		cl.cshifts[CSHIFT_DAMAGE].destcolor[2] = 50;
	} else {
		cl.cshifts[CSHIFT_DAMAGE].destcolor[0] = 255;
		cl.cshifts[CSHIFT_DAMAGE].destcolor[1] = 0;
		cl.cshifts[CSHIFT_DAMAGE].destcolor[2] = 0;
	}

	// calculate view angle kicks
#ifdef QUAKEWORLD
	VectorSubtract (from, cl.simorg, from);
#else
	ent = &cl_entities[cl.playernum + 1];

	VectorSubtract (from, ent->origin, from);
#endif
	VectorNormalize (from);

#ifdef QUAKEWORLD
	AngleVectors (cl.simangles, forward, right, up);
#else
	AngleVectors (ent->angles, forward, right, up);
#endif

	side = DotProduct (from, right);
	v_dmg_roll = count*side*v_kickroll->value;

	side = DotProduct (from, forward);
	v_dmg_pitch = count*side*v_kickpitch->value;

	v_dmg_time = v_kicktime->value;
}


/*
	V_cshift_f

	(desc)
*/
void
V_cshift_f ( void )
{
	int i;

	for ( i=0 ; i<3 ; i++ )
		cshift_empty.destcolor[i] = atoi(Cmd_Argv(i+1));
	cshift_empty.percent = atoi(Cmd_Argv(4));
}


/*
	V_BonusFlash_f

	When you run over an item, the server sends this command
*/
void
V_BonusFlash_f ( void )
{
	cl.cshifts[CSHIFT_BONUS].destcolor[0] = 215;
	cl.cshifts[CSHIFT_BONUS].destcolor[1] = 186;
	cl.cshifts[CSHIFT_BONUS].destcolor[2] = 69;
	cl.cshifts[CSHIFT_BONUS].percent = 50;
}

/*
	V_SetContentsColor

	Underwater, lava, etc each has a color shift
*/
void
V_SetContentsColor (int contents)
{
	if (!v_contentblend->value) {
		cl.cshifts[CSHIFT_CONTENTS] = cshift_empty;
		return;
	}

	switch (contents) {
		case CONTENTS_EMPTY:
			cl.cshifts[CSHIFT_CONTENTS] = cshift_empty;
			break;
		case CONTENTS_LAVA:
			cl.cshifts[CSHIFT_CONTENTS] = cshift_lava;
			break;
		case CONTENTS_SOLID:
		case CONTENTS_SLIME:
			cl.cshifts[CSHIFT_CONTENTS] = cshift_slime;
			break;
		default:
			cl.cshifts[CSHIFT_CONTENTS] = cshift_water;
	}

	if (v_contentblend->value > 0 && v_contentblend->value < 1)
		cl.cshifts[CSHIFT_CONTENTS].percent *= v_contentblend->value;
}

/*
	V_CalcPowerupCshift

	Calculate the color to shade the view under powerups.
*/
void
V_CalcPowerupCshift ( void )
{
	if (cl.stats[STAT_ITEMS] & IT_QUAD) {
		cl.cshifts[CSHIFT_POWERUP].destcolor[0] = 0;
		cl.cshifts[CSHIFT_POWERUP].destcolor[1] = 0;
		cl.cshifts[CSHIFT_POWERUP].destcolor[2] = 255;
		cl.cshifts[CSHIFT_POWERUP].percent = 30;
	} else if (cl.stats[STAT_ITEMS] & IT_SUIT) {
		cl.cshifts[CSHIFT_POWERUP].destcolor[0] = 0;
		cl.cshifts[CSHIFT_POWERUP].destcolor[1] = 255;
		cl.cshifts[CSHIFT_POWERUP].destcolor[2] = 0;
		cl.cshifts[CSHIFT_POWERUP].percent = 20;
	} else if (cl.stats[STAT_ITEMS] & IT_INVISIBILITY) {
		cl.cshifts[CSHIFT_POWERUP].destcolor[0] = 100;
		cl.cshifts[CSHIFT_POWERUP].destcolor[1] = 100;
		cl.cshifts[CSHIFT_POWERUP].destcolor[2] = 100;
		cl.cshifts[CSHIFT_POWERUP].percent = 100;
	} else if (cl.stats[STAT_ITEMS] & IT_INVULNERABILITY) {
		cl.cshifts[CSHIFT_POWERUP].destcolor[0] = 255;
		cl.cshifts[CSHIFT_POWERUP].destcolor[1] = 255;
		cl.cshifts[CSHIFT_POWERUP].destcolor[2] = 0;
		cl.cshifts[CSHIFT_POWERUP].percent = 30;
	} else {
		cl.cshifts[CSHIFT_POWERUP].percent = 0;
	}
}

/*
	V_CalcBlend
*/
void
V_CalcBlend ( void )
{
	float	r, g, b, a, a2;
	int		j;

	r = 0;
	g = 0;
	b = 0;
	a = 0;

	for (j=0 ; j<NUM_CSHIFTS ; j++) {
		if (!gl_cshiftpercent->value)
			continue;

		a2 = ((cl.cshifts[j].percent * gl_cshiftpercent->value) / 100.0) / 255.0;

		if (!a2)
			continue;
		a = a + a2*(1-a);
//		Con_Printf ("j:%i a:%f\n", j, a);
		a2 = a2/a;
		r = r*(1-a2) + cl.cshifts[j].destcolor[0]*a2;
		g = g*(1-a2) + cl.cshifts[j].destcolor[1]*a2;
		b = b*(1-a2) + cl.cshifts[j].destcolor[2]*a2;
	}

	v_blend[0] = r/255.0;
	v_blend[1] = g/255.0;
	v_blend[2] = b/255.0;
	v_blend[3] = bound(0.0, a, 1.0);
}

/*
==============================================================================

						VIEW RENDERING

==============================================================================
*/

float
angledelta ( float a )
{
	a = anglemod(a);
	if (a > 180)
		a -= 360;
	return a;
}

/*
	CalcGunAngle

	Calculate gun angle. :)
*/
void
CalcGunAngle ( void )
{
	float	yaw, pitch, move;
	static float oldyaw = 0;
	static float oldpitch = 0;

	yaw = r_refdef.viewangles[YAW];
	pitch = -r_refdef.viewangles[PITCH];

	yaw = angledelta(yaw - r_refdef.viewangles[YAW]) * 0.4;
	yaw = bound(-10, yaw, 10);

	pitch = angledelta(-pitch - r_refdef.viewangles[PITCH]) * 0.4;
	pitch = bound(-10, pitch, 10);

	move = host_frametime*20;
	if ( yaw > oldyaw ) {
		if (oldyaw + move < yaw)
			yaw = oldyaw + move;
	} else {
		if (oldyaw - move > yaw)
			yaw = oldyaw - move;
	}

	if ( pitch > oldpitch ) {
		if (oldpitch + move < pitch)
			pitch = oldpitch + move;
	} else {
		if (oldpitch - move > pitch)
			pitch = oldpitch - move;
	}

	oldyaw = yaw;
	oldpitch = pitch;

	cl.viewent.angles[YAW] = r_refdef.viewangles[YAW] + yaw;
	cl.viewent.angles[PITCH] = - (r_refdef.viewangles[PITCH] + pitch);
#ifdef UQUAKE
	cl.viewent.angles[ROLL] -= v_idlescale->value * sin(cl.time*v_iroll_cycle->value) * v_iroll_level->value;
	cl.viewent.angles[PITCH] -= v_idlescale->value * sin(cl.time*v_ipitch_cycle->value) * v_ipitch_level->value;
	cl.viewent.angles[YAW] -= v_idlescale->value * sin(cl.time*v_iyaw_cycle->value) * v_iyaw_level->value;
#endif
}

/*
	V_BoundOffsets

	absolutely bound refresh reletive to entity clipping hull so view can
	never be inside a solid wall
*/
void
V_BoundOffsets ( void )
{
#ifdef UQUAKE
	entity_t	*ent;

	ent = &cl_entities[cl.playernum + 1];
#endif

#ifdef QUAKEWORLD
	if (r_refdef.vieworg[0] < cl.simorg[0] - 14)
		r_refdef.vieworg[0] = cl.simorg[0] - 14;
	else if (r_refdef.vieworg[0] > cl.simorg[0] + 14)
		r_refdef.vieworg[0] = cl.simorg[0] + 14;
	if (r_refdef.vieworg[1] < cl.simorg[1] - 14)
		r_refdef.vieworg[1] = cl.simorg[1] - 14;
	else if (r_refdef.vieworg[1] > cl.simorg[1] + 14)
		r_refdef.vieworg[1] = cl.simorg[1] + 14;
	if (r_refdef.vieworg[2] < cl.simorg[2] - 22)
		r_refdef.vieworg[2] = cl.simorg[2] - 22;
	else if (r_refdef.vieworg[2] > cl.simorg[2] + 30)
		r_refdef.vieworg[2] = cl.simorg[2] + 30;
#else
	if (r_refdef.vieworg[0] < ent->origin[0] - 14)
		r_refdef.vieworg[0] = ent->origin[0] - 14;
	else if (r_refdef.vieworg[0] > ent->origin[0] + 14)
		r_refdef.vieworg[0] = ent->origin[0] + 14;
	if (r_refdef.vieworg[1] < ent->origin[1] - 14)
		r_refdef.vieworg[1] = ent->origin[1] - 14;
	else if (r_refdef.vieworg[1] > ent->origin[1] + 14)
		r_refdef.vieworg[1] = ent->origin[1] + 14;
	if (r_refdef.vieworg[2] < ent->origin[2] - 22)
		r_refdef.vieworg[2] = ent->origin[2] - 22;
	else if (r_refdef.vieworg[2] > ent->origin[2] + 30)
		r_refdef.vieworg[2] = ent->origin[2] + 30;
#endif
}

/*
	V_AddIdle

	Idle swaying
*/
void
V_AddIdle ( void )
{
	r_refdef.viewangles[ROLL] += v_idlescale->value * sin(cl.time*v_iroll_cycle->value) * v_iroll_level->value;
	r_refdef.viewangles[PITCH] += v_idlescale->value * sin(cl.time*v_ipitch_cycle->value) * v_ipitch_level->value;
	r_refdef.viewangles[YAW] += v_idlescale->value * sin(cl.time*v_iyaw_cycle->value) * v_iyaw_level->value;
#ifdef QUAKEWORLD
	cl.viewent.angles[ROLL] -= v_idlescale->value * sin(cl.time*v_iroll_cycle->value) * v_iroll_level->value;
	cl.viewent.angles[PITCH] -= v_idlescale->value * sin(cl.time*v_ipitch_cycle->value) * v_ipitch_level->value;
	cl.viewent.angles[YAW] -= v_idlescale->value * sin(cl.time*v_iyaw_cycle->value) * v_iyaw_level->value;
#endif
}


/*
	V_CalcViewRoll

	Roll is induced by movement and damage
*/
void
V_CalcViewRoll (void)
{
	float		side;

#ifdef QUAKEWORLD
	side = V_CalcRoll (cl.simangles, cl.simvel);
#else
	side = V_CalcRoll (cl_entities[cl.playernum + 1].angles, cl.velocity);
#endif
	r_refdef.viewangles[ROLL] += side;

	if (v_dmg_time > 0) {
		r_refdef.viewangles[ROLL] += v_dmg_time/v_kicktime->value*v_dmg_roll;
		r_refdef.viewangles[PITCH] += v_dmg_time/v_kicktime->value*v_dmg_pitch;
		v_dmg_time -= host_frametime;
	}

#ifdef UQUAKE
	if (cl.stats[STAT_HEALTH] <= 0)
	{
		r_refdef.viewangles[ROLL] = 80;	// dead view angle
		return;
	}
#endif
}


/*
	V_CalcIntermissionRefdef

	(desc)
*/
void
V_CalcIntermissionRefdef ( void )
{
	entity_t	*view;
#ifdef UQUAKE
	entity_t	*ent;
#endif
	float		old;

// view is the weapon model
#ifdef UQUAKE
// ent is the player model (visible when out of body)
	ent = &cl_entities[cl.playernum + 1];
// view is the weapon model (only visible from inside body)
#endif
	view = &cl.viewent;

#ifdef QUAKEWORLD
	VectorCopy (cl.simorg, r_refdef.vieworg);
	VectorCopy (cl.simangles, r_refdef.viewangles);
#else
	VectorCopy (ent->origin, r_refdef.vieworg);
	VectorCopy (ent->angles, r_refdef.viewangles);
#endif
	view->model = NULL;

// allways idle in intermission
	old = v_idlescale->value;
	v_idlescale->value = 1;
	V_AddIdle ();
	v_idlescale->value = old;
}

/*
	V_CalcRefdef

	(desc)
*/
void
V_CalcRefdef ( void )
{
	entity_t	*view;
#ifdef QUAKEWORLD
	int			h;
#else
	entity_t	*ent;
	vec3_t		angles;
#endif
	int			i;
	vec3_t		forward, right, up;
	float		bob;
	static float oldz = 0;

#ifdef QUAKEWORLD
	h = cl.stats[STAT_VIEWHEIGHT] + 22;
#endif
	V_DriftPitch ();

#ifdef UQUAKE
// ent is the player model (visible when out of body)
	ent = &cl_entities[cl.playernum + 1];
#endif
// view is the weapon model (only visible from inside body)
	view = &cl.viewent;

#ifdef UQUAKE
// transform the view offset by the model's matrix to get the offset from
// model origin for the view
	ent->angles[YAW] = cl.viewangles[YAW];	// the model should face
										// the view dir
	ent->angles[PITCH] = -cl.viewangles[PITCH];	// the model should face
										// the view dir


#endif
	bob = V_CalcBob ();

#ifdef QUAKEWORLD
// refresh position from simulated origin
	VectorCopy (cl.simorg, r_refdef.vieworg);

	r_refdef.vieworg[2] += bob;
#else
// refresh position
	VectorCopy (ent->origin, r_refdef.vieworg);
	r_refdef.vieworg[2] += cl.viewheight + bob;
#endif

// never let it sit exactly on a node line, because a water plane can
// dissapear when viewed with the eye exactly on it.
#ifdef QUAKEWORLD
// the server protocol only specifies to 1/8 pixel, so add 1/16 in each axis
	r_refdef.vieworg[0] += 1.0/16;
	r_refdef.vieworg[1] += 1.0/16;
	r_refdef.vieworg[2] += 1.0/16;
#else
// the server protocol only specifies to 1/16 pixel, so add 1/32 in each axis
	r_refdef.vieworg[0] += 1.0/32;
	r_refdef.vieworg[1] += 1.0/32;
	r_refdef.vieworg[2] += 1.0/32;
#endif

#ifdef QUAKEWORLD
	VectorCopy (cl.simangles, r_refdef.viewangles);
#else
	VectorCopy (cl.viewangles, r_refdef.viewangles);
#endif
	V_CalcViewRoll ();
	V_AddIdle ();

	// offsets
#ifdef QUAKEWORLD
	if (view_message->flags & PF_GIB)
		r_refdef.vieworg[2] += 8;	// gib view height
	else if (view_message->flags & PF_DEAD)
		r_refdef.vieworg[2] -= 16;	// corpse view height
	else
		r_refdef.vieworg[2] += h;	// view height

	if (view_message->flags & PF_DEAD)		// PF_GIB will also set PF_DEAD
		r_refdef.viewangles[ROLL] = 80;	// dead view angle
#else
// offsets
	angles[PITCH] = -ent->angles[PITCH];	// because entity pitches are
											//  actually backward
	angles[YAW] = ent->angles[YAW];
	angles[ROLL] = ent->angles[ROLL];
#endif

#ifdef UQUAKE
	AngleVectors (angles, forward, right, up);
#endif

#ifdef QUAKEWORLD
// offsets
	AngleVectors (cl.simangles, forward, right, up);
#else
	for (i=0 ; i<3 ; i++)
		r_refdef.vieworg[i] += scr_ofsx->value*forward[i]
			+ scr_ofsy->value*right[i]
			+ scr_ofsz->value*up[i];

#endif

#ifdef UQUAKE
	V_BoundOffsets ();
#endif
// set up gun position
#ifdef QUAKEWORLD
	VectorCopy (cl.simangles, view->angles);
#else
	VectorCopy (cl.viewangles, view->angles);
#endif

	CalcGunAngle ();

#ifdef QUAKEWORLD
	VectorCopy (cl.simorg, view->origin);
	view->origin[2] += h;
#else
	VectorCopy (ent->origin, view->origin);
	view->origin[2] += cl.viewheight;
#endif

	for (i=0 ; i<3 ; i++)
	{
		view->origin[i] += forward[i]*bob*0.4;
//		view->origin[i] += right[i]*bob*0.4;
//		view->origin[i] += up[i]*bob*0.8;
	}
	view->origin[2] += bob;

	// fudge position around to keep amount of weapon visible
	// roughly equal with different FOV
	if (scr_viewsize->value == 110)
		view->origin[2] += 1;
	else if (scr_viewsize->value == 100)
		view->origin[2] += 2;
	else if (scr_viewsize->value == 90)
		view->origin[2] += 1;
	else if (scr_viewsize->value == 80)
		view->origin[2] += 0.5;

#ifdef QUAKEWORLD
	if (view_message->flags & (PF_GIB|PF_DEAD) )
 		view->model = NULL;
 	else
		view->model = cl.model_precache[cl.stats[STAT_WEAPON]];
	view->frame = view_message->weaponframe;
#else
	view->model = cl.model_precache[cl.stats[STAT_WEAPON]];
	view->frame = cl.stats[STAT_WEAPONFRAME];
#endif
	view->colormap = vid.colormap;

	// set up the refresh position
	r_refdef.viewangles[PITCH] += cl.punchangle;

	// smooth out stair step ups
#ifdef QUAKEWORLD
	if ( (view_message->onground != -1) && (cl.simorg[2] - oldz > 0) ) {
		float steptime;

		steptime = host_frametime;

		oldz += steptime * 80;
		if (oldz > cl.simorg[2])
			oldz = cl.simorg[2];
		if (cl.simorg[2] - oldz > 12)
			oldz = cl.simorg[2] - 12;
		r_refdef.vieworg[2] += oldz - cl.simorg[2];
		view->origin[2] += oldz - cl.simorg[2];
	} else {
		oldz = cl.simorg[2];
	}
#else
	if (cl.onground && ent->origin[2] - oldz > 0) {
		float steptime;

		steptime = cl.time - cl.oldtime;
		if (steptime < 0)
//FIXME		I_Error ("steptime < 0");
			steptime = 0;

		oldz += steptime * 80;
		if (oldz > ent->origin[2])
			oldz = ent->origin[2];
		if (ent->origin[2] - oldz > 12)
			oldz = ent->origin[2] - 12;
		r_refdef.vieworg[2] += oldz - ent->origin[2];
		view->origin[2] += oldz - ent->origin[2];
	}

	oldz = ent->origin[2];
	if (cl_chasecam->value)
		Chase_Update ();
#endif
}

/*
	DropPunchAngle

	(desc)
*/
#ifdef QUAKEWORLD
void
DropPunchAngle (void)
{
	cl.punchangle -= 10*host_frametime;
	cl.punchangle = max(cl.punchangle, 0);
}
#endif

/*
	V_RenderView

	The player's clipping box goes from (-16 -16 -24) to (16 16 32) from
	the entity origin, so any view position inside that will be valid
*/
extern vrect_t scr_vrect;

void
V_RenderView (void)
{
#ifdef QUAKEWORLD
//	if (cl.simangles[ROLL])
//		Sys_Error ("cl.simangles[ROLL]");	// DEBUG
cl.simangles[ROLL] = 0;	// FIXME
#endif

	if (cls.state != ca_active)
		return;

#ifdef QUAKEWORLD
	view_frame = &cl.frames[cls.netchan.incoming_sequence & UPDATE_MASK];
	view_message = &view_frame->playerstate[cl.playernum];
#else
	// don't allow cheats in multiplayer
	if (cl.maxclients > 1) {
		Cvar_Set ("scr_ofsx", "0");
		Cvar_Set ("scr_ofsy", "0");
		Cvar_Set ("scr_ofsz", "0");
	}
#endif

#ifdef QUAKEWORLD
	DropPunchAngle ();
#endif
	if (cl.intermission) {	// intermission / finale rendering
		V_CalcIntermissionRefdef ();
	} else {
		if (!cl.paused /* && (sv.maxclients > 1 || key_dest == key_game) */ )
			V_CalcRefdef ();
	}

	R_PushDlights ();
#if 0
	if (lcd_x->value) {
		/*
			render two interleaved views
		*/
		int		i;

		vid.rowbytes <<= 1;
		vid.aspect *= 0.5;

		r_refdef.viewangles[YAW] -= lcd_yaw->value;
		for (i=0 ; i<3 ; i++)
			r_refdef.vieworg[i] -= right[i]*lcd_x->value;
		R_RenderView ();

		vid.buffer += vid.rowbytes>>1;

		R_PushDlights ();

		r_refdef.viewangles[YAW] += lcd_yaw->value*2;
		for (i=0 ; i<3 ; i++)
			r_refdef.vieworg[i] += 2*right[i]*lcd_x->value;
		R_RenderView ();

		vid.buffer -= vid.rowbytes>>1;

		r_refdef.vrect.height <<= 1;

		vid.rowbytes >>= 1;
		vid.aspect *= 2;
	} else {
		R_RenderView ();
	}
#else
	R_RenderView ();
#endif

	if (crosshair->value)
		Draw_Crosshair();
}

//============================================================================

/*
	V_Init

	Create and initialize view cvars and console commands
*/
void
V_Init ( void )
{
	Cmd_AddCommand ("v_cshift", V_cshift_f);
	Cmd_AddCommand ("bf", V_BonusFlash_f);
	Cmd_AddCommand ("centerview", V_StartPitchDrift);

	lcd_x = Cvar_Get ("lcd_x", "0", CVAR_NONE, "None");
	lcd_yaw = Cvar_Get ("lcd_yaw", "0", CVAR_NONE, "None");

	v_centermove	= Cvar_Get ("v_centermove", "0.15", CVAR_NONE, "None");
	v_centerspeed	= Cvar_Get ("v_centerspeed", "500", CVAR_NONE, "None");

	v_iyaw_cycle	= Cvar_Get ("v_iyaw_cycle", "2", CVAR_NONE, "None");
	v_iroll_cycle	= Cvar_Get ("v_iroll_cycle", "0.5", CVAR_NONE, "None");
	v_ipitch_cycle	= Cvar_Get ("v_ipitch_cycle", "1", CVAR_NONE, "None");
	v_iyaw_level	= Cvar_Get ("v_iyaw_level", "0.3", CVAR_NONE, "None");
	v_iroll_level	= Cvar_Get ("v_iroll_level", "0.1", CVAR_NONE, "None");
	v_ipitch_level	= Cvar_Get ("v_ipitch_level", "0.3", CVAR_NONE, "None");

	v_contentblend	= Cvar_Get ("v_contentblend", "1", CVAR_NONE, "Shift color in liquids");

	v_idlescale 	= Cvar_Get ("v_idlescale", "0", CVAR_NONE, "None");
	crosshaircolor	= Cvar_Get ("crosshaircolor", "79", CVAR_ARCHIVE, "Crosshair Color");
	crosshair		= Cvar_Get ("crosshair", "0", CVAR_ARCHIVE, "Crosshair selection");
	cl_crossx		= Cvar_Get ("cl_crossx", "0", CVAR_ARCHIVE, "Crosshair X location");
	cl_crossy		= Cvar_Get ("cl_crossy", "0", CVAR_ARCHIVE, "Crosshair Y location");
	gl_cshiftpercent = Cvar_Get ("gl_cshiftpercent", "100", CVAR_NONE, "Percentage of color shifting");

	scr_ofsx = Cvar_Get ("scr_ofsx", "0", CVAR_NONE, "None");
	scr_ofsy = Cvar_Get ("scr_ofsy", "0", CVAR_NONE, "None");
	scr_ofsz = Cvar_Get ("scr_ofsz", "0", CVAR_NONE, "None");

	cl_rollspeed = Cvar_Get ("cl_rollspeed", "200", CVAR_NONE, "None");
	cl_rollangle = Cvar_Get ("cl_rollangle", "2.0", CVAR_NONE, "None");
	cl_bob = Cvar_Get ("cl_bob", "0.02", CVAR_NONE, "None");
	cl_bobcycle = Cvar_Get ("cl_bobcycle", "0.6", CVAR_NONE, "None");
	cl_bobup = Cvar_Get ("cl_bobup", "0.5", CVAR_NONE, "None");

	v_kicktime = Cvar_Get ("v_kicktime", "0.5", CVAR_NONE, "None");
	v_kickroll = Cvar_Get ("v_kickroll", "0.6", CVAR_NONE, "None");
	v_kickpitch = Cvar_Get ("v_kickpitch", "0.6", CVAR_NONE, "None");
	v_gamma = Cvar_Get ("v_gamma", "1.0", CVAR_ARCHIVE, "Monitor gamma");

	BuildGammaTable (v_gamma->value);	// no gamma yet
}

