/*
	gl_rmain.c

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

#include <qtypes.h>
#include <quakedef.h>
#include <glquake.h>
#include <mathlib.h>
#include <console.h>
#include <view.h>
#include <sound.h>
#include <cvar.h>
#include <sys.h>

#include <draw.h>
#include <sbar.h>

entity_t	r_worldentity;

qboolean	r_cache_thrash;		// compatability

vec3_t		modelorg, r_entorigin;
entity_t	*currententity;

int		r_visframecount;	// bumped when going to a new PVS
int		r_framecount;		// used for dlight push checking

mplane_t	frustum[4];

int		c_brush_polys, c_alias_polys;

qboolean	envmap;			// true during envmap command capture

int		currenttexture = -1;	// to avoid unnecessary texture sets

int		cnttextures[2] = {-1, -1};     // cached

int		particletexture;	// little dot for particles
int		playertextures;		// up to 16 color translated skins

//
// view origin
//
vec3_t	vup;
vec3_t	vpn;
vec3_t	vright;
vec3_t	r_origin;

float	r_world_matrix[16];
float	r_base_world_matrix[16];

//
// screen size info
//
refdef_t	r_refdef;

mleaf_t		*r_viewleaf, *r_oldviewleaf;

texture_t	*r_notexture_mip;

int		d_lightstylevalue[256];	// 8.8 fraction of base light value

cvar_t	*r_clearcolor;

void R_MarkLeaves (void);

cvar_t	*r_norefresh;
cvar_t	*r_drawentities;
cvar_t	*r_drawviewmodel;
cvar_t	*r_speeds;
cvar_t	*r_fullbright;
cvar_t	*r_lightmap;
cvar_t	*r_shadows;
cvar_t	*r_wateralpha;
cvar_t	*r_dynamic;
cvar_t	*r_novis;
#ifdef QUAKEWORLD
cvar_t	*r_netgraph;
#endif

cvar_t	*r_fog;
cvar_t	*r_volfog;
cvar_t	*r_waterwarp;
cvar_t	*r_waterripple;
cvar_t	*r_sky;
cvar_t	*r_skyname;

cvar_t	*gl_finish;
cvar_t	*gl_clear;
cvar_t	*gl_cull;
cvar_t	*gl_texsort;
cvar_t	*gl_smoothmodels;
cvar_t	*gl_affinemodels;
cvar_t	*gl_polyblend;
cvar_t	*gl_playermip;
cvar_t	*gl_nocolors;

cvar_t	*gl_keeptjunctions;
cvar_t	*gl_doubleeyes;

extern	cvar_t	*gl_ztrick;
#ifdef QUAKEWORLD
extern	cvar_t	*scr_fov;
#endif
/*
	R_CullBox

	Returns true if the box is completely outside the frustom
*/
qboolean
R_CullBox (vec3_t mins, vec3_t maxs) {

	int		i;

	for (i=0 ; i<4 ; i++) {
		if (BoxOnPlaneSide (mins, maxs, &frustum[i]) == 2) {
			return true;
		}
	}
	return false;
}

void
R_RotateForEntity (entity_t *e) {
    glTranslatef (e->origin[0],  e->origin[1],  e->origin[2]);

    glRotatef (e->angles[1],  0, 0, 1);
    glRotatef (-e->angles[0],  0, 1, 0);
	//ZOID: fixed z angle
    glRotatef (e->angles[2],  1, 0, 0);
}

/*
 *	SPRITE MODELS
 */

/*
	R_GetSpriteFrame
*/
mspriteframe_t
*R_GetSpriteFrame (entity_t *currententity) {

	msprite_t		*psprite;
	mspritegroup_t	*pspritegroup;
	mspriteframe_t	*pspriteframe;
	int				i, numframes, frame;
	float			*pintervals, fullinterval, targettime, time;

	psprite = currententity->model->cache.data;
	frame = currententity->frame;

	if ((frame >= psprite->numframes) || (frame < 0)) {
		Con_Printf ("R_DrawSprite: no such frame %d\n", frame);
		frame = 0;
	}

	if (psprite->frames[frame].type == SPR_SINGLE) {
		pspriteframe = psprite->frames[frame].frameptr;
	} else {
		pspritegroup = (mspritegroup_t *)psprite->frames[frame].frameptr;
		pintervals = pspritegroup->intervals;
		numframes = pspritegroup->numframes;
		fullinterval = pintervals[numframes-1];

		time = cl.time + currententity->syncbase;

	// when loading in Mod_LoadSpriteGroup, we guaranteed all interval values
	// are positive, so we don't have to worry about division by 0
		targettime = time - ((int)(time / fullinterval)) * fullinterval;

		for (i=0 ; i<(numframes-1) ; i++) {
			if (pintervals[i] > targettime)
				break;
		}
		pspriteframe = pspritegroup->frames[i];
	}
	return pspriteframe;
}


/*
	R_DrawSpriteModel
*/
void
R_DrawSpriteModel (entity_t *e) {

	vec3_t	point;
	mspriteframe_t	*frame;
	float		*up, *right;
	vec3_t		v_forward, v_right, v_up;
	msprite_t		*psprite;

	// don't even bother culling, because it's just a single
	// polygon without a surface cache
	frame = R_GetSpriteFrame (e);
	psprite = currententity->model->cache.data;

	if (psprite->type == SPR_ORIENTED) {	// bullet marks on walls
		AngleVectors (currententity->angles, v_forward, v_right, v_up);
		up = v_up;
		right = v_right;
	} else {	// normal sprite
		up = vup;
		right = vright;
	}

	glColor3f (1,1,1);

	GL_DisableMultitexture();

    GL_Bind(frame->gl_texturenum);

	glEnable (GL_ALPHA_TEST);
	glBegin (GL_QUADS);

	glTexCoord2f (0, 1);
	VectorMA (e->origin, frame->down, up, point);
	VectorMA (point, frame->left, right, point);
	glVertex3fv (point);

	glTexCoord2f (0, 0);
	VectorMA (e->origin, frame->up, up, point);
	VectorMA (point, frame->left, right, point);
	glVertex3fv (point);

	glTexCoord2f (1, 0);
	VectorMA (e->origin, frame->up, up, point);
	VectorMA (point, frame->right, right, point);
	glVertex3fv (point);

	glTexCoord2f (1, 1);
	VectorMA (e->origin, frame->down, up, point);
	VectorMA (point, frame->right, right, point);
	glVertex3fv (point);

	glEnd ();

	glDisable (GL_ALPHA_TEST);
}

/*
	ALIAS MODELS
*/

#define NUMVERTEXNORMALS	162

float	r_avertexnormals[NUMVERTEXNORMALS][3] = {
#include "anorms.-c"
};

vec3_t	shadevector;
float	shadelight[4];
float	ambientlight;

// precalculated dot products for quantized angles
#define SHADEDOT_QUANT 16
float	r_avertexnormal_dots[SHADEDOT_QUANT][256] =
#include "anorm_dots.-c"
;

float	*shadedots = r_avertexnormal_dots[0];

int	lastposenum;

/*
	GL_DrawAliasFrame
*/
void
GL_DrawAliasFrame (aliashdr_t *paliashdr, int posenum)
{

	float		l;
	trivertx_t	*verts;
	int		*order;
	int		count;

	lastposenum = posenum;

	verts = (trivertx_t *)((byte *)paliashdr + paliashdr->posedata);
	verts += posenum * paliashdr->poseverts;
	order = (int *)((byte *)paliashdr + paliashdr->commands);

	while (1)
	{
		// get the vertex count and primitive type
		count = *order++;
		if (!count)
			break;		// done
		if (count < 0)
		{
			count = -count;
			glBegin (GL_TRIANGLE_FAN);
		} else {
			glBegin (GL_TRIANGLE_STRIP);
		}

		do
		{	// texture coordinates come from the draw list
			glTexCoord2f (((float *)order)[0],
					((float *)order)[1]);
			order += 2;

			// normals and vertexes come from the frame list

			l = shadedots[verts->lightnormalindex]
				* shadelight[3];
			if (shadelight[0] || shadelight[1] || shadelight[2])
				glColor3f (l*shadelight[0], l*shadelight[1],
						l*shadelight[2]);
			else
				glColor3f (l, l, l);

			glVertex3f (verts->v[0], verts->v[1], verts->v[2]);
			verts++;
		} while (--count);
		glEnd ();
	}
}


/*
	GL_DrawAliasShadow
*/
extern	vec3_t			lightspot;

void
GL_DrawAliasShadow (aliashdr_t *paliashdr, int posenum) {

	trivertx_t	*verts;
	int		*order;
	vec3_t	point;
	float	height, lheight;
	int		count;

	lheight = currententity->origin[2] - lightspot[2];

	height = 0;
	verts = (trivertx_t *)((byte *)paliashdr + paliashdr->posedata);
	verts += posenum * paliashdr->poseverts;
	order = (int *)((byte *)paliashdr + paliashdr->commands);

	height = -lheight + 1.0;

	while (1) {
		// get the vertex count and primitive type
		count = *order++;
		if (!count)
			break;		// done
		if (count < 0) {
			count = -count;
			glBegin (GL_TRIANGLE_FAN);
		} else {
			glBegin (GL_TRIANGLE_STRIP);
		}

		do {
			// texture coordinates come from the draw list
			// (skipped for shadows) glTexCoord2fv ((float *)order);
			order += 2;

			// normals and vertexes come from the frame list
			point[0] = verts->v[0] * paliashdr->scale[0] + paliashdr->scale_origin[0];
			point[1] = verts->v[1] * paliashdr->scale[1] + paliashdr->scale_origin[1];
			point[2] = verts->v[2] * paliashdr->scale[2] + paliashdr->scale_origin[2];

			point[0] -= shadevector[0]*(point[2]+lheight);
			point[1] -= shadevector[1]*(point[2]+lheight);
			point[2] = height;
//			height -= 0.001;
			glVertex3fv (point);

			verts++;
		} while (--count);
		glEnd ();
	}
}

/*
	R_SetupAliasFrame
*/
void R_SetupAliasFrame (int frame, aliashdr_t *paliashdr) {

	int				pose, numposes;
	float			interval;

	if ((frame >= paliashdr->numframes) || (frame < 0)) {
		Con_DPrintf ("R_AliasSetupFrame: no such frame %d\n", frame);
		frame = 0;
	}

	pose = paliashdr->frames[frame].firstpose;
	numposes = paliashdr->frames[frame].numposes;

	if (numposes > 1) {
		interval = paliashdr->frames[frame].interval;
		pose += (int)(cl.time / interval) % numposes;
	}
	GL_DrawAliasFrame (paliashdr, pose);
}

/*
	R_DrawAliasModel
*/
void
R_DrawAliasModel (entity_t *e) {

	int		i;
	int		*j;
	int		lnum;
	vec3_t		dist;
	float		add;
	model_t		*clmodel;
	vec3_t		mins, maxs;
	aliashdr_t	*paliashdr;
	float		an;
	int		anim;

	clmodel = currententity->model;

	VectorAdd (currententity->origin, clmodel->mins, mins);
	VectorAdd (currententity->origin, clmodel->maxs, maxs);

	if (R_CullBox (mins, maxs))
		return;

	VectorCopy (currententity->origin, r_entorigin);
	VectorSubtract (r_origin, r_entorigin, modelorg);

	/*
		get lighting information
	*/
	j = R_LightPoint (currententity->origin);
	shadelight[0] = (float)j[0];
	shadelight[1] = (float)j[1];
	shadelight[2] = (float)j[2];
	shadelight[3] = (float)j[3];
	ambientlight = shadelight[3];

	// allways give the gun some light
	if (e == &cl.viewent && ambientlight < 24)
		ambientlight = shadelight[3] = 24;

	for (lnum=0 ; lnum<MAX_DLIGHTS ; lnum++)
	{
		if (cl_dlights[lnum].die >= cl.time)
		{
			VectorSubtract (currententity->origin,
					cl_dlights[lnum].origin,
					dist);
			add = cl_dlights[lnum].radius - Length(dist);

			if (add > 0)
			{
				ambientlight += add;
				// ZOID: models should be affected by
				//       dlights as well
				if (r_dynamic->value) {
					shadelight[0] +=
						cl_dlights[lnum].color[0];
					shadelight[1] +=
						cl_dlights[lnum].color[1];
					shadelight[2] +=
						cl_dlights[lnum].color[2];
					shadelight[3] += add;
				}
			}
		}
	}
	// clamp lighting so it doesn't overbright as much
	if (ambientlight > 128)
		ambientlight = 128;
	if (ambientlight + shadelight[3] > 192)
		shadelight[3] = 192 - ambientlight;

	// ZOID: never allow players to go totally black
#ifdef QUAKEWORLD
	if (!strcmp(clmodel->name, "progs/player.mdl"))
#else
	i = currententity - cl_entities;
	if (i >= 1 && i <= cl.maxclients
			/*&& !strcmp (currententity->model->name,
					"progs/player.mdl")*/ )
#endif
	{
		if (ambientlight < 8)
			ambientlight = shadelight[3] = 8;
	} else if (!strcmp (clmodel->name, "progs/flame2.mdl")
		|| !strcmp (clmodel->name, "progs/flame.mdl") )
		// HACK HACK HACK -- no fullbright colors, so make torches full light
		ambientlight = shadelight[3] = 256;

	shadedots = r_avertexnormal_dots[((int)(e->angles[1]
				* (SHADEDOT_QUANT / 360.0)))
				& (SHADEDOT_QUANT - 1)];
	shadelight[3] /= 200.0;

	an = e->angles[1]/180*M_PI;
	shadevector[0] = cos(-an);
	shadevector[1] = sin(-an);
	shadevector[2] = 1;
	VectorNormalize (shadevector);

	/*
		locate the proper data
	*/
	paliashdr = (aliashdr_t *)Mod_Extradata (currententity->model);

	c_alias_polys += paliashdr->numtris;

	/*
		draw all the triangles
	*/
	GL_DisableMultitexture();
	glPushMatrix ();
	R_RotateForEntity (e);

	if (!strcmp (clmodel->name, "progs/eyes.mdl") &&
			gl_doubleeyes->value)
	{
		glTranslatef (paliashdr->scale_origin[0],
				paliashdr->scale_origin[1],
				paliashdr->scale_origin[2] - (22 + 8));
		// double size of eyes, since they are really hard to see in gl
		glScalef (paliashdr->scale[0]*2, paliashdr->scale[1]*2,
				paliashdr->scale[2]*2);
	} else {
		glTranslatef (paliashdr->scale_origin[0],
				paliashdr->scale_origin[1],
				paliashdr->scale_origin[2]);
		glScalef (paliashdr->scale[0], paliashdr->scale[1],
				paliashdr->scale[2]);
	}

	anim = (int)(cl.time*10) & 3;
	GL_Bind(paliashdr->gl_texturenum[currententity->skinnum][anim]);

	// we can't dynamically colormap textures, so they are cached
	// seperately for the players.  Heads are just uncolored.
#ifdef QUAKEWORLD
	if (currententity->scoreboard && !gl_nocolors->value) {
		i = currententity->scoreboard - cl.players;
		if (!currententity->scoreboard->skin) {
			Skin_Find(currententity->scoreboard);
			R_TranslatePlayerSkin(i);
		}
		if (i >= 0 && i<MAX_CLIENTS) {
		    GL_Bind(playertextures + i);
		}
	}
#else
	if (currententity->colormap != vid.colormap && !gl_nocolors->value) {
		i = currententity - cl_entities;
		if (i >= 1 && i <= cl.maxclients )
		    GL_Bind(playertextures - 1 + i);
	}
#endif

	if (gl_smoothmodels->value)
		glShadeModel (GL_SMOOTH);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	if (gl_affinemodels->value)
		glHint (GL_PERSPECTIVE_CORRECTION_HINT, GL_FASTEST);

	R_SetupAliasFrame (currententity->frame, paliashdr);

	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

	glShadeModel (GL_FLAT);
	if (gl_affinemodels->value)
		glHint (GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

	glPopMatrix ();

	if (r_shadows->value) {
		glPushMatrix ();
		R_RotateForEntity (e);
		glDisable (GL_TEXTURE_2D);
		glEnable (GL_BLEND);
		glColor4f (0,0,0,0.5);
		GL_DrawAliasShadow (paliashdr, lastposenum);
		glEnable (GL_TEXTURE_2D);
		glDisable (GL_BLEND);
		glColor4f (1,1,1,1);
		glPopMatrix ();
	}

}

/*
	R_DrawEntitiesOnList
*/
void
R_DrawEntitiesOnList ( void ) {

	int		i;

	if (!r_drawentities->value)
		return;

	// draw sprites seperately, because of alpha blending
	for (i=0 ; i<cl_numvisedicts ; i++) {
		currententity = &cl_visedicts[i];
		switch (currententity->model->type) {
		case mod_alias:
			R_DrawAliasModel (currententity);
			break;

		case mod_brush:
			R_DrawBrushModel (currententity);
			break;

		default:
			break;
		}
	}

	for (i=0 ; i<cl_numvisedicts ; i++) {
		currententity = &cl_visedicts[i];

		switch (currententity->model->type) {
		case mod_sprite:
			R_DrawSpriteModel (currententity);
			break;

		default :
			break;
		}
	}
}

/*
	R_DrawViewModel
*/
void
R_DrawViewModel ( void )
{

	float		ambient[4], diffuse[4];
	int		*j;
	int		shadelight[4];
	int		lnum;
	vec3_t		dist;
	float		add;
	dlight_t	*dl;
	int		ambientlight;

#ifdef QUAKEWORLD
	if (!r_drawviewmodel->value || !Cam_DrawViewModel())
		return;
#else
	if (!r_drawviewmodel->value)
		return;

	if (cl_chasecam->value)
		return;
#endif

	if (envmap)
		return;

	if (!r_drawentities->value)
		return;

	if (cl.stats[STAT_ITEMS] & IT_INVISIBILITY)
		return;

	if (cl.stats[STAT_HEALTH] <= 0)
		return;

	currententity = &cl.viewent;
	if (!currententity->model)
		return;

	j = R_LightPoint (currententity->origin);

	if (j[3] < 24)
		j[3] = 24;		// allways give some light on gun
	ambientlight = j[3];
	shadelight[0] = j[0];
	shadelight[1] = j[1];
	shadelight[2] = j[2];
	shadelight[3] = j[3];

// add dynamic lights
	for (lnum=0 ; lnum<MAX_DLIGHTS ; lnum++) {
		dl = &cl_dlights[lnum];
		if (!dl->radius)
			continue;
		if (!dl->radius)
			continue;
		if (dl->die < cl.time)
			continue;

		VectorSubtract (currententity->origin, dl->origin, dist);
		add = dl->radius - Length(dist);
		if (add > 0)
		{
			shadelight[0] += dl->color[0];
			shadelight[1] += dl->color[1];
			shadelight[2] += dl->color[2];
			shadelight[3] += add;
			ambientlight += add;
		}
	}

	ambient[0] = ambient[1] = ambient[2] = ambient[3] =
		(float)ambientlight / 128;

	diffuse[0] = diffuse[1] = diffuse[2] = diffuse[3] =
		(float)shadelight[3] / 128;

	// hack the depth range to prevent view model from poking into walls
	glDepthRange (gldepthmin, gldepthmin + 0.3*(gldepthmax-gldepthmin));
	R_DrawAliasModel (currententity);
	glDepthRange (gldepthmin, gldepthmax);
}


/*
	R_PolyBlend
*/
void
R_PolyBlend ( void ) {
	if (!gl_polyblend->value)
		return;
	if (!v_blend[3])
		return;

	GL_DisableMultitexture();

	glDisable (GL_ALPHA_TEST);
	glEnable (GL_BLEND);
	glDisable (GL_DEPTH_TEST);
	glDisable (GL_TEXTURE_2D);

    glLoadIdentity ();

    glRotatef (-90,  1, 0, 0);	    // put Z going up
    glRotatef (90,  0, 0, 1);	    // put Z going up

	glColor4fv (v_blend);

	glBegin (GL_QUADS);

	glVertex3f (10, 100, 100);
	glVertex3f (10, -100, 100);
	glVertex3f (10, -100, -100);
	glVertex3f (10, 100, -100);
	glEnd ();

	glDisable (GL_BLEND);
	glEnable (GL_TEXTURE_2D);
	glEnable (GL_ALPHA_TEST);
}


int
SignbitsForPlane ( mplane_t *out ) {
	int	bits, j;

	// for fast box on planeside test

	bits = 0;
	for (j=0 ; j<3 ; j++) {
		if (out->normal[j] < 0)
			bits |= 1<<j;
	}
	return bits;
}

void
R_SetFrustum ( void ) {

	int		i;

	if (r_refdef.fov_x == 90) {
		// front side is visible

		VectorAdd (vpn, vright, frustum[0].normal);
		VectorSubtract (vpn, vright, frustum[1].normal);

		VectorAdd (vpn, vup, frustum[2].normal);
		VectorSubtract (vpn, vup, frustum[3].normal);
	} else {
		// rotate VPN right by FOV_X/2 degrees
		RotatePointAroundVector( frustum[0].normal, vup, vpn, -(90-r_refdef.fov_x / 2 ) );
		// rotate VPN left by FOV_X/2 degrees
		RotatePointAroundVector( frustum[1].normal, vup, vpn, 90-r_refdef.fov_x / 2 );
		// rotate VPN up by FOV_X/2 degrees
		RotatePointAroundVector( frustum[2].normal, vright, vpn, 90-r_refdef.fov_y / 2 );
		// rotate VPN down by FOV_X/2 degrees
		RotatePointAroundVector( frustum[3].normal, vright, vpn, -( 90 - r_refdef.fov_y / 2 ) );
	}
	for (i=0 ; i<4 ; i++) {
		frustum[i].type = PLANE_ANYZ;
		frustum[i].dist = DotProduct (r_origin, frustum[i].normal);
		frustum[i].signbits = SignbitsForPlane (&frustum[i]);
	}
}

/*
	R_SetupFrame
*/
void
R_SetupFrame ( void ) {
// don't allow cheats in multiplayer
#ifdef QUAKEWORLD
	r_fullbright->value = 0;
	r_lightmap->value = 0;
	if (!atoi(Info_ValueForKey(cl.serverinfo, "watervis")))
		r_wateralpha->value = 1;
#else
	if (cl.maxclients > 1)
		Cvar_Set (r_fullbright, "0");
#endif

	R_AnimateLight ();
	r_framecount++;

// build the transformation matrix for the given view angles
	VectorCopy (r_refdef.vieworg, r_origin);

	AngleVectors (r_refdef.viewangles, vpn, vright, vup);

// current viewleaf
	r_oldviewleaf = r_viewleaf;
	r_viewleaf = Mod_PointInLeaf (r_origin, cl.worldmodel);

	V_SetContentsColor (r_viewleaf->contents);
	V_CalcBlend ();

	r_cache_thrash = false;

	c_brush_polys = 0;
	c_alias_polys = 0;
}

void
MYgluPerspective( GLdouble fovy, GLdouble aspect, GLdouble zNear, GLdouble zFar ) {

   GLdouble xmin, xmax, ymin, ymax;

   ymax = zNear * tan( fovy * M_PI / 360.0 );
   ymin = -ymax;

   xmin = ymin * aspect;
   xmax = ymax * aspect;

   glFrustum( xmin, xmax, ymin, ymax, zNear, zFar );
}


/*
	R_SetupGL
*/
void
R_SetupGL ( void ) {

	float	screenaspect;
	//float	yfov;
	extern	int glwidth, glheight;
	int		x, x2, y2, y, w, h;

	//
	// set up viewpoint
	//
	glMatrixMode(GL_PROJECTION);
    glLoadIdentity ();
	x = r_refdef.vrect.x * glwidth/vid.width;
	x2 = (r_refdef.vrect.x + r_refdef.vrect.width) * glwidth/vid.width;
	y = (vid.height-r_refdef.vrect.y) * glheight/vid.height;
	y2 = (vid.height - (r_refdef.vrect.y + r_refdef.vrect.height)) * glheight/vid.height;

	// fudge around because of frac screen scale
	if (x > 0)
		x--;
	if (x2 < glwidth)
		x2++;
	if (y2 < 0)
		y2--;
	if (y < glheight)
		y++;

	w = x2 - x;
	h = y - y2;

	if (envmap) {
		x = y2 = 0;
		w = h = 256;
	}

	glViewport (glx + x, gly + y2, w, h);
    screenaspect = (float)r_refdef.vrect.width/r_refdef.vrect.height;
//	yfov = 2*atan((float)r_refdef.vrect.height/r_refdef.vrect.width)*180/M_PI;
//	yfov = (2.0 * tan (scr_fov->value/360*M_PI)) / screenaspect;
//	yfov = 2*atan((float)r_refdef.vrect.height/r_refdef.vrect.width)*(scr_fov->value*2)/M_PI;
//	MYgluPerspective (yfov,  screenaspect,  4,  4096);
    MYgluPerspective (r_refdef.fov_y,  screenaspect,  4,  4096);

	glCullFace(GL_FRONT);

	glMatrixMode(GL_MODELVIEW);
    glLoadIdentity ();

    glRotatef (-90,  1, 0, 0);	    // put Z going up
    glRotatef (90,  0, 0, 1);	    // put Z going up
    glRotatef (-r_refdef.viewangles[2],  1, 0, 0);
    glRotatef (-r_refdef.viewangles[0],  0, 1, 0);
    glRotatef (-r_refdef.viewangles[1],  0, 0, 1);
    glTranslatef (-r_refdef.vieworg[0],  -r_refdef.vieworg[1],  -r_refdef.vieworg[2]);

	glGetFloatv (GL_MODELVIEW_MATRIX, r_world_matrix);

	/*
		set drawing parms
	*/
	if (gl_cull->value)
		glEnable(GL_CULL_FACE);
	else
		glDisable(GL_CULL_FACE);

	glDisable(GL_BLEND);
	glDisable(GL_ALPHA_TEST);
	glEnable(GL_DEPTH_TEST);
}

/*
	R_RenderScene

	r_refdef must be set before the first call
*/
void
R_RenderScene ( void ) {

	R_SetupFrame ();
	R_SetFrustum ();

	R_SetupGL ();

	R_MarkLeaves ();	// done here so we know if we're in water
	R_DrawWorld ();		// adds static entities to the list
	S_ExtraUpdate ();	// don't let sound get messed up if going slow
	R_DrawEntitiesOnList ();

	GL_DisableMultitexture();

	R_RenderDlights ();
	R_DrawParticles ();

#ifdef GLTEST
	Test_Draw ();
#endif

}


/*
	R_Clear
*/
void
R_Clear ( void ) {
	static int l;

	if (gl_ztrick->value)
	{
		static int trickframe;

		if (gl_clear->value)
		{
			if (l != (int)r_clearcolor->value)
			{
					l = (int)r_clearcolor->value;
				        glClearColor (host_basepal[l*3]/255.0,
						      host_basepal[l*3+1]/255.0,
						      host_basepal[l*3+2]/255.0,
						      1.0);
			}
			glClear (GL_COLOR_BUFFER_BIT);
		}

		trickframe++;
		if (trickframe & 1)
		{
			gldepthmin = 0;
			gldepthmax = 0.49999;
			glDepthFunc (GL_LEQUAL);
		}
		else
		{
			gldepthmin = 1;
			gldepthmax = 0.5;
			glDepthFunc (GL_GEQUAL);
		}
	}
	else
	{
		if (gl_clear->value)
			glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		else
			glClear (GL_DEPTH_BUFFER_BIT);

		gldepthmin = 0;
		gldepthmax = 1;
		glDepthFunc (GL_LEQUAL);
	}
	glDepthRange (gldepthmin, gldepthmax);
}

extern cvar_t *crosshaircolor;

/*
	R_RenderView

	r_refdef must be set before the first call
*/
void
R_RenderView ( void ) {

	double	time1 = 0, time2 = 0;
	// Fixme: the last argument should be a cvar... r_fog_gamma
	GLfloat colors[4] = {(GLfloat) 1.0, (GLfloat) 1.0, (GLfloat) 1, (GLfloat) 0.1};

	if (r_norefresh->value)
		return;

	if (!r_worldentity.model || !cl.worldmodel)
		Sys_Error ("R_RenderView: NULL worldmodel");

	if (r_speeds->value)
	{
		glFinish ();
		time1 = Sys_DoubleTime ();
		c_brush_polys = 0;
		c_alias_polys = 0;
	}

	if (gl_finish->value)
		glFinish ();

	R_Clear ();

	// render normal view

/*** Render Volumetric Fog ***/

	if(r_volfog->value)
	{
		R_RenderScene ();
		R_DrawViewModel ();

		glClear(GL_STENCIL_BUFFER_BIT);
		//glColorMask(GL_FALSE);
		glStencilFunc(GL_ALWAYS, 1, 1);
		glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
		//glEnable(GL_STENCIL_TEST);
		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LESS);
	                glFogi (GL_FOG_MODE, GL_EXP2);
        	        glFogfv (GL_FOG_COLOR, colors);
// fixme: GL_FOG_DENSITY should have r_volfog_density var
			glFogf (GL_FOG_DENSITY, r_volfog->value);

                glEnable(GL_FOG);
		R_DrawWaterSurfaces();
		glDisable(GL_FOG);

		glStencilFunc(GL_EQUAL, 1, 1);
		glStencilMask(GL_FALSE);
		glDisable(GL_DEPTH_TEST);
	}

/*** Depth fog code ***/

	else if (r_fog->value)
	{	// FIXME: would be nice if the user could select what fogmode... (r_fog_mode)
		glFogi (GL_FOG_MODE, GL_EXP2);
		glFogfv (GL_FOG_COLOR, colors);
		glFogf (GL_FOG_DENSITY, (GLfloat) r_fog->value);
//		glFogi (GL_FOG_MODE, GL_LINEAR);
//		glFogfv (GL_FOG_COLOR, colors);
//		glFogf (GL_FOG_START, 300.0);
//		glFogf (GL_FOG_END, 1500.0);
//		glFogf (GL_FOG_DENSITY, 0.2);
		glEnable (GL_FOG);

		R_RenderScene ();
	        R_DrawViewModel ();
		R_DrawWaterSurfaces ();

		glDisable(GL_FOG);
	}

/*** Regular rendering code ***/

else
	{
		R_RenderScene ();
		R_DrawViewModel ();
		R_DrawWaterSurfaces ();
	}

	R_PolyBlend ();

	if (r_speeds->value) {
		//glFinish ();
		time2 = Sys_DoubleTime ();
		Con_Printf ("%3i ms  %4i wpoly %4i epoly\n", (int)((time2-time1)*1000), c_brush_polys, c_alias_polys);
	}
}
