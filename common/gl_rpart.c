/*
	gl_rpart.c

	particle engine

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

#include <quakedef.h>
#include <r_local.h>
#include <glquake.h>
#include <mathlib.h>
#include <lib_replace.h>
#include <console.h>
#include <server.h>
#include <stdlib.h>
#include <cmd.h>

#define MAX_PARTICLES			2048	// max particles at once
#define ABSOLUTE_MIN_PARTICLES		512	// min particle clamp
#define MAX_FIRES					128	// rocket flames

int		ramp1[8] = {0x6f, 0x6d, 0x6b, 0x69, 0x67, 0x65, 0x63, 0x61};
int		ramp2[8] = {0x6f, 0x6e, 0x6d, 0x6c, 0x6b, 0x6a, 0x68, 0x66};
int		ramp3[8] = {0x6d, 0x6b, 6, 5, 4, 3};

particle_t	*active_particles, *free_particles;

particle_t	*particles;
int			r_numparticles;

vec3_t			r_pright, r_pup, r_ppn;


fire_t		r_fires[MAX_FIRES];

/*
===============
R_InitParticles
===============
*/
void R_InitParticles (void)
{
	int		i;

	i = COM_CheckParm ("-particles");

	if (i)
	{
		r_numparticles = (int)(Q_atoi(com_argv[i+1]));
		if (r_numparticles < ABSOLUTE_MIN_PARTICLES)
			r_numparticles = ABSOLUTE_MIN_PARTICLES;
	}
	else
	{
		r_numparticles = MAX_PARTICLES;
	}

	particles = (particle_t *)
			Hunk_AllocName (r_numparticles * sizeof(particle_t), "particles");
}

#ifdef QUAKE2
void R_DarkFieldParticles (entity_t *ent)
{
	int			i, j, k;
	particle_t	*p;
	float		vel;
	vec3_t		dir;
	vec3_t		org;

	org[0] = ent->origin[0];
	org[1] = ent->origin[1];
	org[2] = ent->origin[2];
	for (i=-16 ; i<16 ; i+=8)
		for (j=-16 ; j<16 ; j+=8)
			for (k=0 ; k<32 ; k+=8)
			{
				if (!free_particles)
					return;
				p = free_particles;
				free_particles = p->next;
				p->next = active_particles;
				active_particles = p;

				p->die = cl.time + 0.2 + (rand()&7) * 0.02;
				p->color = 150 + rand()%6;
				p->type = pt_slowgrav;

				dir[0] = j*8;
				dir[1] = i*8;
				dir[2] = k*8;

				p->org[0] = org[0] + i + (rand()&3);
				p->org[1] = org[1] + j + (rand()&3);
				p->org[2] = org[2] + k + (rand()&3);

				VectorNormalize (dir);						
				vel = 50 + (rand()&63);
				VectorScale (dir, vel, p->vel);
			}
}
#endif


/*
===============
R_EntityParticles
===============
*/

#define NUMVERTEXNORMALS	162
extern	float	r_avertexnormals[NUMVERTEXNORMALS][3];
vec3_t	avelocities[NUMVERTEXNORMALS];
float	beamlength = 16;
vec3_t	avelocity = {23, 7, 3};
float	partstep = 0.01;
float	timescale = 0.01;

void R_EntityParticles (entity_t *ent)
{
	int			count;
	int			i;
	particle_t	*p;
	float		angle;
	float		sr, sp, sy, cr, cp, cy;
	vec3_t		forward;
	float		dist;

	dist = 64;
	count = 50;

	if (!avelocities[0][0])
	{
		for (i=0 ; i<NUMVERTEXNORMALS*3 ; i++)
			avelocities[0][i] = (rand()&255) * 0.01;
	}


	for (i=0 ; i<NUMVERTEXNORMALS ; i++)
	{
		angle = cl.time * avelocities[i][0];
		sy = sin(angle);
		cy = cos(angle);
		angle = cl.time * avelocities[i][1];
		sp = sin(angle);
		cp = cos(angle);
		angle = cl.time * avelocities[i][2];
		sr = sin(angle);
		cr = cos(angle);

		forward[0] = cp*cy;
		forward[1] = cp*sy;
		forward[2] = -sp;

		if (!free_particles)
			return;
		p = free_particles;
		free_particles = p->next;
		p->next = active_particles;
		active_particles = p;

		p->die = cl.time + 0.01;
		p->color = 0x6f;
		p->type = pt_explode;

		p->org[0] = ent->origin[0] + r_avertexnormals[i][0]*dist + forward[0]*beamlength;			
		p->org[1] = ent->origin[1] + r_avertexnormals[i][1]*dist + forward[1]*beamlength;			
		p->org[2] = ent->origin[2] + r_avertexnormals[i][2]*dist + forward[2]*beamlength;			
	}
}


/*
===============
R_ClearParticles
===============
*/
void R_ClearParticles (void)
{
	int		i;

	free_particles = &particles[0];
	active_particles = NULL;

	for (i=0 ;i<r_numparticles ; i++)
		particles[i].next = &particles[i+1];
	particles[r_numparticles-1].next = NULL;
}


void R_ReadPointFile_f (void)
{
	QFile	*f;
	vec3_t	org;
	int		r;
	int		c;
	particle_t	*p;
	char	name[MAX_OSPATH];
	char	buf[256];

// FIXME	snprintf(name, sizeof(name),"maps/%s.pts", sv.name);

	COM_FOpenFile (name, &f);
	if (!f)
	{
		Con_Printf ("couldn't open %s\n", name);
		return;
	}

	Con_Printf ("Reading %s...\n", name);
	c = 0;
	for ( ;; )
	{
		if (!Qgets(f,buf,sizeof(buf)))
			break;
		r = sscanf (buf,"%f %f %f\n", &org[0], &org[1], &org[2]);
		if (r != 3)
			break;
		c++;

		if (!free_particles)
		{
			Con_Printf ("Not enough free particles\n");
			break;
		}
		p = free_particles;
		free_particles = p->next;
		p->next = active_particles;
		active_particles = p;

		p->die = 99999;
		p->color = (-c)&15;
		p->type = pt_static;
		VectorCopy (vec3_origin, p->vel);
		VectorCopy (org, p->org);
	}

	Qclose (f);
	Con_Printf ("%i points read\n", c);
}

/*
===============
R_ParseParticleEffect

Parse an effect out of the server message
===============
*/
void R_ParseParticleEffect (void)
{
	vec3_t		org, dir;
	int			i, count, msgcount, color;

	for (i=0 ; i<3 ; i++)
		org[i] = MSG_ReadCoord ();
	for (i=0 ; i<3 ; i++)
		dir[i] = MSG_ReadChar () * (1.0/16);
	msgcount = MSG_ReadByte ();
	color = MSG_ReadByte ();

	if (msgcount == 255)
		count = 1024;
	else
		count = msgcount;

	R_RunParticleEffect (org, dir, color, count);
}

/*
===============
R_ParticleExplosion

===============
*/
void R_ParticleExplosion (vec3_t org)
{
	int			i, j;
	particle_t	*p;

	for (i=0 ; i<1024 ; i++)
	{
		if (!free_particles)
			return;
		p = free_particles;
		free_particles = p->next;
		p->next = active_particles;
		active_particles = p;

		p->die = cl.time + 5;
		p->color = ramp1[0];
		p->ramp = rand()&3;
		if (i & 1)
		{
			p->type = pt_explode;
			for (j=0 ; j<3 ; j++)
			{
				p->org[j] = org[j] + ((rand()%32)-16);
				p->vel[j] = (rand()%512)-256;
			}
		}
		else
		{
			p->type = pt_explode2;
			for (j=0 ; j<3 ; j++)
			{
				p->org[j] = org[j] + ((rand()%32)-16);
				p->vel[j] = (rand()%512)-256;
			}
		}
	}
}

/*
===============
R_ParticleExplosion2

===============
*/
void R_ParticleExplosion2 (vec3_t org, int colorStart, int colorLength)
{
	int			i, j;
	particle_t	*p;
	int			colorMod = 0;

	for (i=0; i<512; i++)
	{
		if (!free_particles)
			return;
		p = free_particles;
		free_particles = p->next;
		p->next = active_particles;
		active_particles = p;

		p->die = cl.time + 0.3;
		p->color = colorStart + (colorMod % colorLength);
		colorMod++;

		p->type = pt_blob;
		for (j=0 ; j<3 ; j++)
		{
			p->org[j] = org[j] + ((rand()%32)-16);
			p->vel[j] = (rand()%512)-256;
		}
	}
}

/*
===============
R_BlobExplosion

===============
*/
void R_BlobExplosion (vec3_t org)
{
	int			i, j;
	particle_t	*p;

	for (i=0 ; i<1024 ; i++)
	{
		if (!free_particles)
			return;
		p = free_particles;
		free_particles = p->next;
		p->next = active_particles;
		active_particles = p;

		p->die = cl.time + 1 + (rand()&8)*0.05;

		if (i & 1)
		{
			p->type = pt_blob;
			p->color = 66 + rand()%6;
			for (j=0 ; j<3 ; j++)
			{
				p->org[j] = org[j] + ((rand()%32)-16);
				p->vel[j] = (rand()%512)-256;
			}
		}
		else
		{
			p->type = pt_blob2;
			p->color = 150 + rand()%6;
			for (j=0 ; j<3 ; j++)
			{
				p->org[j] = org[j] + ((rand()%32)-16);
				p->vel[j] = (rand()%512)-256;
			}
		}
	}
}

/*
===============
R_RunParticleEffect

===============
*/
void R_RunParticleEffect (vec3_t org, vec3_t dir, int color, int count)
{
	int			i, j;
	particle_t	*p;
#ifdef QUAKEWORLD
	int			scale;

	if (count > 130)
		scale = 3;
	else if (count > 20)
		scale = 2;
	else
		scale = 1;
#endif
	
	for (i=0 ; i<count ; i++) {
		if (!free_particles)
			return;
		p = free_particles;
		free_particles = p->next;
		p->next = active_particles;
		active_particles = p;

#ifdef QUAKEWORLD
		p->die = cl.time + 0.1*(rand()%5);
		p->color = (color&~7) + (rand()&7);
		p->type = pt_grav;
		for (j=0 ; j<3 ; j++) {
			p->org[j] = org[j] + scale*((rand()&15)-8);
			p->vel[j] = dir[j]*15;// + (rand()%300)-150;
		}
#elif UQUAKE
		if (count == 1024) {
			// rocket explosion
			p->die = cl.time + 5;
			p->color = ramp1[0];
			p->ramp = rand()&3;
			if (i & 1) {
				p->type = pt_explode;
				for (j=0 ; j<3 ; j++) {
					p->org[j] = org[j] + ((rand()%32)-16);
					p->vel[j] = (rand()%512)-256;
				}
			} else {
				p->type = pt_explode2;
				for (j=0 ; j<3 ; j++) {
					p->org[j] = org[j] + ((rand()%32)-16);
					p->vel[j] = (rand()%512)-256;
				}
			}
		} else {
			p->die = cl.time + 0.1*(rand()%5);
			p->color = (color&~7) + (rand()&7);
			p->type = pt_slowgrav;
			for (j=0 ; j<3 ; j++) {
				p->org[j] = org[j] + ((rand()&15)-8);
				p->vel[j] = dir[j]*15;// + (rand()%300)-150;
			}
		}
#endif
	}
}


/*
===============
R_LavaSplash

===============
*/
void R_LavaSplash (vec3_t org)
{
	int			i, j, k;
	particle_t	*p;
	float		vel;
	vec3_t		dir;

	for (i=-16 ; i<16 ; i++)
		for (j=-16 ; j<16 ; j++)
			for (k=0 ; k<1 ; k++)
			{
				if (!free_particles)
					return;
				p = free_particles;
				free_particles = p->next;
				p->next = active_particles;
				active_particles = p;

				p->die = cl.time + 2 + (rand()&31) * 0.02;
				p->color = 224 + (rand()&7);
#ifdef QUAKEWORLD
				p->type = pt_grav;
#else
				p->type = pt_slowgrav;
#endif

				dir[0] = j*8 + (rand()&7);
				dir[1] = i*8 + (rand()&7);
				dir[2] = 256;

				p->org[0] = org[0] + dir[0];
				p->org[1] = org[1] + dir[1];
				p->org[2] = org[2] + (rand()&63);

				VectorNormalize (dir);
				vel = 50 + (rand()&63);
				VectorScale (dir, vel, p->vel);
			}
}

/*
===============
R_TeleportSplash

===============
*/
void R_TeleportSplash (vec3_t org)
{
	int			i, j, k;
	particle_t	*p;
	float		vel;
	vec3_t		dir;

	for (i=-16 ; i<16 ; i+=4)
		for (j=-16 ; j<16 ; j+=4)
			for (k=-24 ; k<32 ; k+=4)
			{
				if (!free_particles)
					return;
				p = free_particles;
				free_particles = p->next;
				p->next = active_particles;
				active_particles = p;

				p->die = cl.time + 0.2 + (rand()&7) * 0.02;
				p->color = 7 + (rand()&7);
#ifdef QUAKEWORLD
				p->type = pt_grav;
#else
				p->type = pt_slowgrav;
#endif

				dir[0] = j*8;
				dir[1] = i*8;
				dir[2] = k*8;

				p->org[0] = org[0] + i + (rand()&3);
				p->org[1] = org[1] + j + (rand()&3);
				p->org[2] = org[2] + k + (rand()&3);

				VectorNormalize (dir);
				vel = 50 + (rand()&63);
				VectorScale (dir, vel, p->vel);
			}
}

/*
	R_RocketTrail
*/
void
R_RocketTrail (vec3_t start, vec3_t end, int type, entity_t *ent)
{
	vec3_t		vec;
	float		len;
	int			j;
	particle_t	*p;
	int			dec;
	static int	tracercount;

	VectorSubtract (end, start, vec);
	len = VectorNormalize (vec);
	if (type < 128) {
		dec = 3;
	} else {
		dec = 1;
		type -= 128;
	}

	while (len > 0) {
		len -= dec;

		if (!free_particles)
			return;
		p = free_particles;
		free_particles = p->next;
		p->next = active_particles;
		active_particles = p;

		VectorCopy (vec3_origin, p->vel);
		p->die = cl.time + 2;

		switch (type) {
			case 0:		// rocket trail
				R_AddFire (start, end, ent);
				p->ramp = (rand()&3);
				p->color = ramp3[(int)p->ramp];
				p->type = pt_fire;
				for (j=0 ; j<3 ; j++)
					p->org[j] = start[j] + ((rand()%6)-3);
				break;
			case 1:		// smoke smoke
				p->ramp = (rand()&3) + 2;
				p->color = ramp3[(int)p->ramp];
				p->type = pt_fire;
				for (j=0 ; j<3 ; j++)
					p->org[j] = start[j] + ((rand()%6)-3);
				break;
			case 2:		// blood
#ifdef QUAKEWORLD
				p->type = pt_slowgrav;
#else
				p->type = pt_grav;
#endif
				p->color = 67 + (rand()&3);
				for (j=0 ; j<3 ; j++)
					p->org[j] = start[j] + ((rand()%6)-3);
				break;
			case 3:
			case 5:		// tracer
				p->die = cl.time + 0.5;
				p->type = pt_static;

				if (type == 3)
					p->color = 52 + ((tracercount&4)<<1);
				else
					p->color = 230 + ((tracercount&4)<<1);

				tracercount++;

				VectorCopy (start, p->org);
				if (tracercount & 1) {
					p->vel[0] = 30*vec[1];
					p->vel[1] = 30*-vec[0];
				} else {
					p->vel[0] = 30*-vec[1];
					p->vel[1] = 30*vec[0];
				}
				break;
			case 4:		// slight blood
#ifdef QUAKEWORLD
				p->type = pt_slowgrav;
#else
				p->type = pt_grav;
#endif
				p->color = 67 + (rand()&3);
				for (j=0 ; j<3 ; j++)
					p->org[j] = start[j] + ((rand()%6)-3);
				len -= 3;
				break;
			case 6: 	// voor trail
				p->color = 9*16 + 8 + (rand()&3);
				p->type = pt_static;
				p->die = cl.time + 0.3;
				for (j=0 ; j<3 ; j++)
					p->org[j] = start[j] + ((rand()&15)-8);
				break;
		}
		VectorAdd (start, vec, start);
	}
}

#ifdef UQUAKE
extern	cvar_t	*sv_gravity;
#endif

/*
===============
R_DrawParticles
===============
*/

void R_DrawParticles (void)
{
	particle_t		*p, *kill;
	float			grav;
	int				i;
	float			time2, time3;
	float			time1;
	float			dvel;
	float			frametime;
	unsigned char	*at;
	unsigned char	theAlpha;
	vec3_t			up, right;
	float			scale;
	qboolean		alphaTestEnabled;

	if (gl_particles->value)
	{
		GL_Bind(particletexture);
		alphaTestEnabled = glIsEnabled(GL_ALPHA_TEST);
	
		if (alphaTestEnabled)
			glDisable(GL_ALPHA_TEST);
		glEnable (GL_BLEND);
		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
		glBegin (GL_TRIANGLES);
	
		VectorScale (vup, 1.5, up);
		VectorScale (vright, 1.5, right);
#ifdef 	UQUAKE
		frametime = cl.time - cl.oldtime;
#else
		frametime = host_frametime;
#endif
		time3 = frametime * 15;
		time2 = frametime * 10; // 15;
		time1 = frametime * 5;
#ifdef UQUAKE
		grav = frametime * sv_gravity->value * 0.05;
#else
		grav = frametime * 800 * 0.05;
#endif
		dvel = 4*frametime;

		for ( ;; )
		{
			kill = active_particles;
			if (kill && kill->die < cl.time)
			{
				active_particles = kill->next;
				kill->next = free_particles;
				free_particles = kill;
				continue;
			}
			break;
		}
	
		for (p=active_particles ; p ; p=p->next)
		{
			for ( ;; )
			{
				kill = p->next;
				if (kill && kill->die < cl.time)
				{
					p->next = kill->next;
					kill->next = free_particles;
					free_particles = kill;
					continue;
				}
				break;
			}
	
			// hack a scale up to keep particles from disapearing
			scale = (p->org[0] - r_origin[0])*vpn[0]
				+ (p->org[1] - r_origin[1])*vpn[1]
				+ (p->org[2] - r_origin[2])*vpn[2];
			if (scale < 20)
				scale = gl_particles->value;
			else
				scale = gl_particles->value + scale * 0.004;
#if 0 // was in uquake, but give it a go
			glColor3ubv ((byte *)&d_8to24table[(int)p->color]);
#else
			at = (byte *)&d_8to24table[(int)p->color];
			if (p->type==pt_fire)
				theAlpha = 255*(6-p->ramp)/6;
//				theAlpha = 192;
//			else if (p->type==pt_explode || p->type==pt_explode2)
//				theAlpha = 255*(8-p->ramp)/8;
			else
				theAlpha = 255;
			glColor4ub (*at, *(at+1), *(at+2), theAlpha);
//			glColor3ubv (at);
//			glColor3ubv ((byte *)&d_8to24table[(int)p->color]);
#endif
			glTexCoord2f (0,0);
			glVertex3fv (p->org);
			glTexCoord2f (1,0);
			glVertex3f (p->org[0] + up[0]*scale, p->org[1] + up[1]*scale, p->org[2] + up[2]*scale);
			glTexCoord2f (0,1);
			glVertex3f (p->org[0] + right[0]*scale, p->org[1] + right[1]*scale, p->org[2] + right[2]*scale);

			p->org[0] += p->vel[0]*frametime;
			p->org[1] += p->vel[1]*frametime;
			p->org[2] += p->vel[2]*frametime;

			switch (p->type)
			{
			case pt_static:
				break;
			case pt_fire:
				p->ramp += time1;
				if (p->ramp >= 6)
					p->die = -1;
				else
					p->color = ramp3[(int)p->ramp];
				p->vel[2] += grav;
				break;
	
			case pt_explode:
				p->ramp += time2;
				if (p->ramp >=8)
					p->die = -1;
				else
					p->color = ramp1[(int)p->ramp];
				for (i=0 ; i<3 ; i++)
					p->vel[i] += p->vel[i]*dvel;
				p->vel[2] -= grav;
				break;
	
			case pt_explode2:
				p->ramp += time3;
				if (p->ramp >=8)
					p->die = -1;
				else
					p->color = ramp2[(int)p->ramp];
				for (i=0 ; i<3 ; i++)
					p->vel[i] -= p->vel[i]*frametime;
				p->vel[2] -= grav;
				break;
	
			case pt_blob:
				for (i=0 ; i<3 ; i++)
					p->vel[i] += p->vel[i]*dvel;
				p->vel[2] -= grav;
				break;
	
			case pt_blob2:
				for (i=0 ; i<2 ; i++)
					p->vel[i] -= p->vel[i]*dvel;
				p->vel[2] -= grav;
				break;
			case pt_grav:
				// This causes some particles to fall to the
				// ground.  It's been reported as a bug,
				// so for now it's being fixed.
				// We can implement it as a feature, but it's
				// not all that cool.
//				p->vel[2] -= grav * 20;
//				break;
			case pt_slowgrav:
				p->vel[2] -= grav;
				break;
			}
		}

		glEnd ();
		glDisable (GL_BLEND);
		if (alphaTestEnabled)
			glEnable(GL_ALPHA_TEST);
		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
	} // if (gl_particles->value)
}


/*
	R_AddFire

	Nifty ball of fire GL effect.  Kinda a meshing of the dlight and
	particle engine code.
*/
float r_firecolor_flame[4]={0.9,0.7,0.3,1.0};
float r_firecolor_light[4]={0.9,0.7,0.3,0.66};

void
R_AddFire (vec3_t start, vec3_t end, entity_t *ent)
{
	float		len;
	fire_t		*f;
	dlight_t	*dl;
	vec3_t		vec;
	int			key;

	if (!gl_fires->value)
		return;

	VectorSubtract (end, start, vec);
	len = VectorNormalize (vec);
	key = ent-cl_entities+1;

	if (len)
	{
		f = R_AllocFire (key);
		VectorCopy (end, f->origin);
		VectorCopy (start, f->owner);
		f->size = 20;
		f->die = cl.time + 0.5;
		f->decay = -1;
		f->color=r_firecolor_flame;

		dl = CL_AllocDlight (key);
		VectorCopy (end, dl->origin);
		dl->radius = 200;
		dl->die = cl.time + 0.5;
		dl->color=r_firecolor_light;
	}
}

/*
	R_AllocFire

	Clears out and returns a new fireball
*/
fire_t *
R_AllocFire (int key)
{
	int		i;
	fire_t		*f;
	if (key)	// first try to find/reuse a keyed spot
	{
		f = r_fires;
		for (i = 0; i < MAX_FIRES; i++, f++)
			if (f->key == key)
			{
				memset (f, 0, sizeof(*f));
				f->key = key;
				f->color = f->_color;
				return f;
			}
	}

	f = r_fires;	// no match, look for a free spot
	for (i = 0; i < MAX_FIRES; i++, f++)
	{
		if (f->die < cl.time)
		{
			memset (f, 0, sizeof(*f));
			f->key = key;
			f->color = f->_color;
			return f;
		}
	}

	f = &r_fires[0];
	memset (f, 0, sizeof(*f));
	f->key = key;
	f->color = f->_color;
	return f;	
}

/*
	R_DrawFire

	draws one fireball - probably never need to call this directly
*/
void
R_DrawFire (fire_t *f)
{
	int		i, j;
	vec3_t		vec,vec2;
	float		radius;
	float		*b_sin, *b_cos;

	b_sin = bubble_sintable;
	b_cos = bubble_costable;

	radius = f->size + 0.35;

	// figure out if we're inside the area of effect
	VectorSubtract (f->origin, r_origin, vec);
	if (Length (vec) < radius)
	{
		AddLightBlend (1, 0.5, 0, f->size * 0.0003);	// we are
		return;
	}

	// we're not - draw it
	glBegin (GL_TRIANGLE_FAN);
	glColor4fv (f->color);
	for (i=0 ; i<3 ; i++)
		vec[i] = f->origin[i] - vpn[i] * radius;
	glVertex3fv (vec);
	glColor3f (0.0, 0.0, 0.0);

	// don't panic, this just draws a bubble...
	for (i=16 ; i>=0 ; i--)
	{
		for (j=0 ; j<3 ; j++) {
			vec[j] = f->origin[j] + (*b_cos * vright[j]
				+ vup[j]*(*b_sin)) * radius;
			vec2[j] = f->owner[j] + (*b_cos * vright[j]
				+ vup[j]*(*b_sin)) * radius;
		}
		glVertex3fv (vec);
		glVertex3fv (vec2);

		b_sin++;
		b_cos++;
	}
	glEnd ();
}

/*
	R_UpdateFires

	Draws each fireball in sequence
*/
void
R_UpdateFires (void)
{
	int		i;
	fire_t	*f;

	if (!gl_fires->value)
		return;

	glDepthMask (0);
	glDisable (GL_TEXTURE_2D);
	glShadeModel (GL_SMOOTH);
	glEnable (GL_BLEND);
	glBlendFunc (GL_ONE, GL_ONE);

	f = r_fires;
	for (i = 0; i < MAX_FIRES; i++, f++)
	{
		if (f->die < cl.time || !f->size)
			continue;
		f->size += f->decay;
		f->color[3] /= 2.0;
		R_DrawFire (f);
	}

	glColor3f (1.0, 1.0, 1.0);
	glDisable (GL_BLEND);
	glEnable (GL_TEXTURE_2D);
	glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDepthMask (1);
}

void
R_FireColor_f(void)
{
	int i;

	if (Cmd_Argc() == 1) {
		Con_Printf ("r_firecolor %f %f %f %f %f\n",
					r_firecolor_flame[0],
					r_firecolor_flame[1],
					r_firecolor_flame[2],
					r_firecolor_flame[3],
					r_firecolor_light[3]);
		return;
	}
	if (Cmd_Argc() !=6) {
		Con_Printf ("Usage r_firecolor R G B Af Al\n");
		return;
	}
	for (i=0; i<4; i++) {
		r_firecolor_flame[i]=atof(Cmd_Argv(i+1));
		r_firecolor_light[i]=r_firecolor_flame[i];
	}
	r_firecolor_light[3]=atof(Cmd_Argv(i+1));
}
