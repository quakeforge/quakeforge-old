/*
	glquake.h

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

// disable data conversion warnings

#ifndef _GLQUAKE_H
#define _GLQUAKE_H

#if !defined(__GNUC__) && !defined(__unix) && !defined(__unix__)
#pragma warning(disable : 4244)     // MIPS
#pragma warning(disable : 4136)     // X86
#pragma warning(disable : 4051)     // ALPHA
#endif

#ifdef _WIN32
#include <windows.h>
#endif

#include <GL/gl.h>
#include <GL/glu.h>
#include <model.h>
#include <client.h>
#include <wad.h>

void GL_BeginRendering (int *x, int *y, int *width, int *height);
void GL_EndRendering (void);

/*	continuation of loring's patch, he just #ifdef'd and #endif'd _WIN32
	this section, reason follows:

        The solution h is to put a #ifdef _WIN32 around the code, so Mesa 3.2
        is required under at least Windows, but probably nowhere else (ie, the
        issue is sidestepped for non-Windows systems).
*/
#ifdef _WIN32
// Function prototypes for the Texture Object Extension routines
typedef GLboolean (GLAPIENTRY *ARETEXRESFUNCPTR)(GLsizei, const GLuint *,
                    const GLboolean *);
typedef void (GLAPIENTRY *BINDTEXFUNCPTR)(GLenum, GLuint);
typedef void (GLAPIENTRY *DELTEXFUNCPTR)(GLsizei, const GLuint *);
typedef void (GLAPIENTRY *GENTEXFUNCPTR)(GLsizei, GLuint *);
typedef GLboolean (GLAPIENTRY *ISTEXFUNCPTR)(GLuint);
typedef void (GLAPIENTRY *PRIORTEXFUNCPTR)(GLsizei, const GLuint *,
                    const GLclampf *);
typedef void (GLAPIENTRY *TEXSUBIMAGEPTR)(int, int, int, int, int, int, int, int, void *);

extern	BINDTEXFUNCPTR bindTexFunc;
extern	DELTEXFUNCPTR delTexFunc;
extern	TEXSUBIMAGEPTR TexSubImage2DFunc;
#endif

extern	int texture_extension_number;
extern	int		texture_mode;

extern	float	gldepthmin, gldepthmax;

void GL_Upload32 (unsigned *data, int width, int height,  qboolean mipmap, qboolean alpha);
void GL_Upload8 (byte *data, int width, int height,  qboolean mipmap, qboolean alpha);
void GL_Upload8_EXT (byte *data, int width, int height,  qboolean mipmap, qboolean alpha);
int GL_LoadTexture (char *identifier, int width, int height, byte *data, qboolean mipmap, qboolean alpha);
int GL_FindTexture (char *identifier);

typedef struct
{
	float	x, y, z;
	float	s, t;
	float	r, g, b;
} glvert_t;

extern glvert_t glv;

extern	int glx, gly, glwidth, glheight;

#ifdef _WIN32
extern	PROC glArrayElementEXT;
extern	PROC glColorPointerEXT;
extern	PROC glTexturePointerEXT;
extern	PROC glVertexPointerEXT;
#endif

// r_local.h -- private refresh defs

#define ALIAS_BASE_SIZE_RATIO		(1.0 / 11.0)
					// normalizing factor so player model works out to about
					//  1 pixel per triangle
//#define	MAX_LBM_HEIGHT		480


#define SKYSHIFT		7
#define	SKYSIZE			(1 << SKYSHIFT)
#define SKYMASK			(SKYSIZE - 1)

#define BACKFACE_EPSILON	0.01


void R_TimeRefresh_f (void);
void R_ReadPointFile_f (void);
texture_t *R_TextureAnimation (texture_t *base);

/*
typedef struct surfcache_s
{
	struct surfcache_s	*next;
	struct surfcache_s 	**owner;		// NULL is an empty chunk of memory
	int					lightadj[MAXLIGHTMAPS]; // checked for strobe flush
	int					dlight;
	int					size;		// including header
	unsigned			width;
	unsigned			height;		// DEBUG only needed for debug
	float				mipscale;
	struct texture_s	*texture;	// checked for animating textures
	byte				data[4];	// width*height elements
} surfcache_t;
*/


//====================================================


extern	entity_t	r_worldentity;
extern	qboolean	r_cache_thrash;		// compatability
extern	vec3_t		modelorg, r_entorigin;
extern	entity_t	*currententity;
extern	int			r_visframecount;	// ??? what difs?
extern	int			r_framecount;
extern	mplane_t	frustum[4];
extern	int		c_brush_polys, c_alias_polys;


//
// view origin
//
extern	vec3_t	vup;
extern	vec3_t	vpn;
extern	vec3_t	vright;
extern	vec3_t	r_origin;

//
// screen size info
//
extern	refdef_t	r_refdef;
extern	mleaf_t		*r_viewleaf, *r_oldviewleaf;
extern	int		d_lightstylevalue[256];	// 8.8 fraction of base light value

extern	qboolean	envmap;
extern	int	currenttexture;
extern	int	cnttextures[2];
extern	int	particletexture;
extern	int	netgraphtexture;	// netgraph texture
extern	int	playertextures;

extern	int	skytexturenum;		// index in cl.loadmodel, not gl texture object

extern	cvar_t	*r_norefresh;
extern	cvar_t	*r_drawentities;
extern	cvar_t	*r_drawworld;
extern	cvar_t	*r_drawviewmodel;
extern	cvar_t	*r_speeds;
extern	cvar_t	*r_waterwarp;
extern	cvar_t	*r_fullbright;
extern	cvar_t	*r_lightmap;
extern	cvar_t	*r_shadows;
extern	cvar_t	*r_wateralpha;
extern	cvar_t	*r_dynamic;
extern	cvar_t	*r_novis;
extern	cvar_t	*r_netgraph;
extern  cvar_t  *r_fog;
extern  cvar_t  *r_waterwarp;
extern  cvar_t  *r_volfog;
extern  cvar_t  *r_waterripple;
extern	cvar_t	*r_sky;
extern	cvar_t	*r_skyname;

extern	cvar_t	*gl_clear;
extern	cvar_t	*gl_cull;
extern	cvar_t	*gl_poly;
extern	cvar_t	*gl_texsort;
extern	cvar_t	*gl_smoothmodels;
extern	cvar_t	*gl_affinemodels;
extern	cvar_t	*gl_polyblend;
extern	cvar_t	*gl_keeptjunctions;
extern	cvar_t	*gl_flashblend;
extern	cvar_t	*gl_nocolors;
extern	cvar_t	*gl_finish;
extern	cvar_t	*gl_doubleeyes;

extern	int		gl_lightmap_format;
extern	int		gl_solid_format;
extern	int		gl_alpha_format;

extern	cvar_t	*gl_max_size;
extern	cvar_t	*gl_playermip;

extern	float	r_world_matrix[16];

extern	const char *gl_vendor;
extern	const char *gl_renderer;
extern	const char *gl_version;
extern	const char *gl_extensions;

void R_TranslatePlayerSkin (int playernum);
void GL_Bind (int texnum);

// Multitexture
#define    TEXTURE0_SGIS				0x835E
#define    TEXTURE1_SGIS				0x835F

typedef void (GLAPIENTRY *lpMTexFUNC) (GLenum, GLfloat, GLfloat);
typedef void (GLAPIENTRY *lpSelTexFUNC) (GLenum);
extern lpMTexFUNC qglMTexCoord2fSGIS;
extern lpSelTexFUNC qglSelectTextureSGIS;

extern qboolean gl_mtexable;

void GL_DisableMultitexture(void);
void GL_EnableMultitexture(void);

//
// gl_warp.c
//
void GL_SubdivideSurface (msurface_t *fa);
void EmitBothSkyLayers (msurface_t *fa);
void EmitWaterPolys (msurface_t *fa);
void EmitSkyPolys (msurface_t *fa);
void R_DrawSkyChain (msurface_t *s);
void R_LoadSkys (void);
void R_ClearSkyBox (void);
void R_DrawSkyBox (void);

//
// gl_draw.c
//
int GL_LoadPicTexture (qpic_t *pic);
void GL_Set2D (void);

//
// gl_rmain.c
//
qboolean R_CullBox (vec3_t mins, vec3_t maxs);
void R_RotateForEntity (entity_t *e);

//
// gl_rlight.c
//
void R_MarkLights (dlight_t *light, int bit, mnode_t *node);
void R_AnimateLight (void);
void R_RenderDlights (void);
int * R_LightPoint (vec3_t p);

//
// gl_refrag.c
//
void R_StoreEfrags (efrag_t **ppefrag);

//
// gl_mesh.c
//
void GL_MakeAliasModelDisplayLists (model_t *m, aliashdr_t *hdr);

//
// gl_rsurf.c
//
void R_DrawBrushModel (entity_t *e);
void R_DrawWorld (void);
void GL_BuildLightmaps (void);

//
// gl_ngraph.c
//
void R_NetGraph (void);
#endif // _GLQUAKE_H
