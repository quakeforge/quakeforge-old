/*
	gl_rpart.c

	(description)

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

#include <d_iface.h>
#include <glquake.h>
#include <mathlib.h>

extern particle_t	*active_particles, *free_particles;
extern int			ramp1[8], ramp2[8], ramp3[8];
extern cvar_t		*gl_particles;

/*
===============
R_DrawParticles
===============
*/
extern	cvar_t	*sv_gravity;

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

	// Do we actually want to render particles in the first place?
	if (!gl_particles->value) {
		// We still need to prune the particles...
		for (p=active_particles ; p ;)
		{
			// Is it dead?
			if (p->next && (p->next->die < cl.time)) {
				// First save our place..
				kill = p->next;
				p->next = p->next->next;
				// Remove from list and continue.
				kill->next = free_particles;
				free_particles = kill;
			}
			p = p->next;
		}

		return;
	}

	GL_Bind(particletexture);
	alphaTestEnabled = glIsEnabled(GL_ALPHA_TEST);

	if (alphaTestEnabled)
		glDisable(GL_ALPHA_TEST);
	glEnable (GL_BLEND);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glBegin (GL_TRIANGLES);

	VectorScale (vup, 1.5, up);
	VectorScale (vright, 1.5, right);
#ifdef UQUAKE
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

	for (p=active_particles ; p ;)
	{
		// Is it dead?
		if (p->next && (p->next->die < cl.time)) {
			// First save our place..
			kill = p->next;
			p->next = p->next->next;
			// Remove from list and continue.
			kill->next = free_particles;
			free_particles = kill;
		}

		// hack a scale up to keep particles from disapearing
		scale = (p->org[0] - r_origin[0])*vpn[0]
			  + (p->org[1] - r_origin[1])*vpn[1]
			  + (p->org[2] - r_origin[2])*vpn[2];
		if (scale < 20)
			scale = gl_particles->value;
		else
			scale = gl_particles->value + scale * 0.004;
		at = (byte *)&d_8to24table[(int)p->color];
		if (p->type==pt_fire)
			theAlpha = 255*(6-p->ramp)/6;
		else
			theAlpha = 255;
		glColor4ub (*at, *(at+1), *(at+2), theAlpha);
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
		p = p->next;
	}

	glEnd ();
	glDisable (GL_BLEND);
	if (alphaTestEnabled)
		glEnable(GL_ALPHA_TEST);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
}

