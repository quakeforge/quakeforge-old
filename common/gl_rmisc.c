/*
	gl_rmisc.c

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
#include <cvar.h>
#include <console.h>
#include <sys.h>
#include <lib_replace.h>
#include <cmd.h>

void R_InitBubble();

void R_FireColor_f();

extern cvar_t *r_clearcolor;

/*
==================
R_InitTextures
==================
*/
void	R_InitTextures (void)
{
	int		x,y, m;
	byte	*dest;

// create a simple checkerboard texture for the default
	r_notexture_mip = Hunk_AllocName (sizeof(texture_t) + 16*16+8*8+4*4+2*2, "notexture");

	r_notexture_mip->width = r_notexture_mip->height = 16;
	r_notexture_mip->offsets[0] = sizeof(texture_t);
	r_notexture_mip->offsets[1] = r_notexture_mip->offsets[0] + 16*16;
	r_notexture_mip->offsets[2] = r_notexture_mip->offsets[1] + 8*8;
	r_notexture_mip->offsets[3] = r_notexture_mip->offsets[2] + 4*4;

	for (m=0 ; m<4 ; m++)
	{
		dest = (byte *)r_notexture_mip + r_notexture_mip->offsets[m];
		for (y=0 ; y< (16>>m) ; y++)
			for (x=0 ; x< (16>>m) ; x++)
			{
				if (  (y< (8>>m) ) ^ (x< (8>>m) ) )
					*dest++ = 0;
				else
					*dest++ = 0xff;
			}
	}
}

byte	dottexture[8][8] =
{
	{0,1,1,0,0,0,0,0},
	{1,1,1,1,0,0,0,0},
	{1,1,1,1,0,0,0,0},
	{0,1,1,0,0,0,0,0},
	{0,0,0,0,0,0,0,0},
	{0,0,0,0,0,0,0,0},
	{0,0,0,0,0,0,0,0},
	{0,0,0,0,0,0,0,0},
};
void R_InitParticleTexture (void)
{
	int		x,y;
	byte	data[8][8][4];

	//
	// particle texture
	//
	particletexture = texture_extension_number++;
    GL_Bind(particletexture);

	for (x=0 ; x<8 ; x++)
	{
		for (y=0 ; y<8 ; y++)
		{
			data[y][x][0] = 255;
			data[y][x][1] = 255;
			data[y][x][2] = 255;
			data[y][x][3] = dottexture[x][y]*255;
		}
	}
	glTexImage2D (GL_TEXTURE_2D, 0, gl_alpha_format, 8, 8, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);

	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}

/*
===============
R_Envmap_f

Grab six views for environment mapping tests
===============
*/
void R_Envmap_f (void)
{
	byte	buffer[256*256*4];

	glDrawBuffer  (GL_FRONT);
	glReadBuffer  (GL_FRONT);
	envmap = true;

	r_refdef.vrect.x = 0;
	r_refdef.vrect.y = 0;
	r_refdef.vrect.width = 256;
	r_refdef.vrect.height = 256;

	r_refdef.viewangles[0] = 0;
	r_refdef.viewangles[1] = 0;
	r_refdef.viewangles[2] = 0;
	GL_BeginRendering (&glx, &gly, &glwidth, &glheight);
	R_RenderView ();
	glReadPixels (0, 0, 256, 256, GL_RGBA, GL_UNSIGNED_BYTE, buffer);
	COM_WriteFile ("env0.rgb", buffer, sizeof(buffer));

	r_refdef.viewangles[1] = 90;
	GL_BeginRendering (&glx, &gly, &glwidth, &glheight);
	R_RenderView ();
	glReadPixels (0, 0, 256, 256, GL_RGBA, GL_UNSIGNED_BYTE, buffer);
	COM_WriteFile ("env1.rgb", buffer, sizeof(buffer));

	r_refdef.viewangles[1] = 180;
	GL_BeginRendering (&glx, &gly, &glwidth, &glheight);
	R_RenderView ();
	glReadPixels (0, 0, 256, 256, GL_RGBA, GL_UNSIGNED_BYTE, buffer);
	COM_WriteFile ("env2.rgb", buffer, sizeof(buffer));

	r_refdef.viewangles[1] = 270;
	GL_BeginRendering (&glx, &gly, &glwidth, &glheight);
	R_RenderView ();
	glReadPixels (0, 0, 256, 256, GL_RGBA, GL_UNSIGNED_BYTE, buffer);
	COM_WriteFile ("env3.rgb", buffer, sizeof(buffer));

	r_refdef.viewangles[0] = -90;
	r_refdef.viewangles[1] = 0;
	GL_BeginRendering (&glx, &gly, &glwidth, &glheight);
	R_RenderView ();
	glReadPixels (0, 0, 256, 256, GL_RGBA, GL_UNSIGNED_BYTE, buffer);
	COM_WriteFile ("env4.rgb", buffer, sizeof(buffer));

	r_refdef.viewangles[0] = 90;
	r_refdef.viewangles[1] = 0;
	GL_BeginRendering (&glx, &gly, &glwidth, &glheight);
	R_RenderView ();
	glReadPixels (0, 0, 256, 256, GL_RGBA, GL_UNSIGNED_BYTE, buffer);
	COM_WriteFile ("env5.rgb", buffer, sizeof(buffer));

	envmap = false;
	glDrawBuffer  (GL_BACK);
	glReadBuffer  (GL_BACK);
	GL_EndRendering ();
}

/*
===============
R_Init
===============
*/
void R_Init (void)
{
#ifndef QUAKEWORLD
	extern cvar_t *gl_finish;
#endif /* QUAKEWORLD */

	Cmd_AddCommand ("timerefresh", R_TimeRefresh_f);
	Cmd_AddCommand ("envmap", R_Envmap_f);
	Cmd_AddCommand ("pointfile", R_ReadPointFile_f);

	Cmd_AddCommand ("r_firecolor", R_FireColor_f);

	r_norefresh = Cvar_Get ("r_norefresh", "0", CVAR_NONE,
			"None");
	r_lightmap = Cvar_Get ("r_lightmap", "0", CVAR_NONE,
			"None");
	r_fullbright = Cvar_Get ("r_fullbright", "0", CVAR_NONE,
			"Toggles whether light shading is in effect");
	r_drawentities = Cvar_Get ("r_drawentities", "1", CVAR_NONE,
			"Toggles drawing of doors, monsters, etc");
	r_drawviewmodel = Cvar_Get ("r_drawviewmodel", "1", CVAR_NONE,
			"Toggles drawing of your weapon");
	r_shadows = Cvar_Get ("r_shadows", "0", CVAR_NONE,
			"None");

	// FIXME: Change these back to 1 ..   --KB
	r_interpanimation = Cvar_Get ("r_interpanimation", "0", CVAR_ARCHIVE,
			"toggles interpolation of model animation");
	r_interptransform = Cvar_Get ("r_interptransform", "0", CVAR_ARCHIVE,
			"toggles interpolation of model transformation");

	r_wateralpha = Cvar_Get ("r_wateralpha", "1", CVAR_NONE,
			"Alpha value for water textures, valid range is "
			"between 0 and 1.  Leave it at 1 unless you have "
			"maps which are vis'd for wateralpha.");
	r_dynamic = Cvar_Get ("r_dynamic", "1", CVAR_NONE,
			"Toggles dynamic lighting");
	r_novis = Cvar_Get ("r_novis", "0", CVAR_NONE,
			"Toggles whether or not to ignore vis information.  "
			"Leave it at 0 to avoid a nasty FPS hit.");
	r_speeds = Cvar_Get ("r_speeds", "0", CVAR_NONE,
			"Toggles display of drawing time");
#ifdef QUAKEWORLD
	r_netgraph = Cvar_Get ("r_netgraph", "0", CVAR_NONE,
			"Toggles the netgraph");
#endif /* QUAKEWORLD */
	r_fog = Cvar_Get ("r_fog", "0", CVAR_NONE,
			"The amount og global fog to apply.  Values larger "
			"than 0.0005 don't look very good.");
	r_waterwarp = Cvar_Get ("r_waterwarp", "0", CVAR_NONE,
		"Toggles whether surfaces are warped when in a liquid");
	r_volfog = Cvar_Get ("r_volfog", "0", CVAR_NONE,
			"volumetric fog - don't use this yet");
	r_waterripple = Cvar_Get ("r_waterripple", "0", CVAR_NONE,
			"The amount of ripple to add to liquid surfaces, "
			"values of 5 to 8 or so seem to work well, works "
			"best with maps which are vised for wateralpha");
	r_clearcolor = Cvar_Get ("r_clearcolor", "2", 0,
			"this is the color of \"void space\"");
	r_sky = Cvar_Get ("r_sky", "0", CVAR_NONE,
			"Toggles skyboxes");
	r_skyname = Cvar_Get ("r_skyname", "sky", CVAR_NONE, 
			"Sets the name of the current skybox");

	gl_clear = Cvar_Get ("gl_clear", "0", CVAR_NONE,
			"Toggles whether or not to clear void space");
	gl_texsort = Cvar_Get ("gl_texsort", "1", CVAR_NONE,
			"None");

 	if (gl_mtexable)
		Cvar_Set (gl_texsort, "0");

	gl_cull = Cvar_Get ("gl_cull", "1", CVAR_NONE,
			"None");
	gl_smoothmodels = Cvar_Get ("gl_smoothmodels", "1", CVAR_NONE,
			"None");
	gl_affinemodels = Cvar_Get ("gl_affinemodels", "0", CVAR_NONE,
			"None");
	gl_polyblend = Cvar_Get ("gl_polyblend", "1", CVAR_NONE,
			"None");
	gl_flashblend = Cvar_Get ("gl_flashblend", "1", CVAR_NONE,
			"Toggles whether or not to use flashblend effect "
			"for dynamic lights.  Turning this off looks very "
			"cool, but can cost a few FPS for the effect");
	gl_playermip = Cvar_Get ("gl_playermip", "0", CVAR_NONE,
			"None");
	gl_nocolors = Cvar_Get ("gl_nocolors", "0", CVAR_NONE,
			"None");
	gl_finish = Cvar_Get ("gl_finish", "0", CVAR_NONE,
			"None");

	gl_particles = Cvar_Get ("gl_particles", "1", CVAR_ARCHIVE,
			"Toggles and sets the size of particles");
	gl_fires = Cvar_Get ("gl_fires", "1", CVAR_ARCHIVE,
			"Toggles lavaball and rocket fireballs");

	gl_keeptjunctions = Cvar_Get ("gl_keeptjunctions", "1", CVAR_NONE,
			"None");

	R_InitBubble();

	gl_doubleeyes = Cvar_Get ("gl_doubleeyes", "1", CVAR_ARCHIVE,
			"None");

	R_InitParticles ();
	R_InitParticleTexture ();

#ifdef GLTEST
	Test_Init ();
#endif

#ifdef QUAKEWORLD
	netgraphtexture = texture_extension_number;
	texture_extension_number++;
#endif /* QUAKEWORLD */

	playertextures = texture_extension_number;
#ifdef QUAKEWORLD
	texture_extension_number += MAX_CLIENTS;
#else /* not QUAKEWORLD */
	texture_extension_number += 16;
#endif /* QUAKEWORLD */
}

/*
	R_TranslatePlayerSkin

	Translate a skin texture by the per-player color lookup
*/
void R_TranslatePlayerSkin (int playernum)
{
	int		top, bottom;
	byte	translate[256];
	unsigned	translate32[256];
	int		i, j;
	byte	*original;
	unsigned	pixels[512*256], *out;
	unsigned	scaled_width, scaled_height;
	int			inwidth, inheight;
	int			tinwidth, tinheight;
	byte		*inrow;
	unsigned	frac, fracstep;
#ifdef QUAKEWORLD	
	player_info_t	*player;
	extern byte 	player_8bit_texels[320*200];
	char s[512];
#else
	aliashdr_t	*paliashdr;
	model_t 	*model;
	int 		s;
#endif

	GL_DisableMultitexture();

#ifdef QUAKEWORLD
	player = &cl.players[playernum];
	if (!player->name[0])
		return;

	strcpy(s, Info_ValueForKey(player->userinfo, "skin"));
	COM_StripExtension(s, s);

	if (player->skin && !stricmp(s, player->skin->name))
		player->skin = NULL;

	if (player->_topcolor != player->topcolor ||
		player->_bottomcolor != player->bottomcolor || !player->skin) {
		player->_topcolor = player->topcolor;
		player->_bottomcolor = player->bottomcolor;

		top = bound (0, player->topcolor, 13);
		bottom = bound (0, player->bottomcolor, 13);
		top *= 16;
		bottom *= 16;

#elif UQUAKE
	{
		top = cl.scores[playernum].colors & 0xf0;
		bottom = (cl.scores[playernum].colors &15)<<4;
#endif

		for (i=0 ; i<256 ; i++)
			translate[i] = i;

		for (i=0 ; i<16 ; i++) {
			if (top < 128)	// the artists made some backwards ranges.  sigh.
				translate[TOP_RANGE+i] = top+i;
			else
				translate[TOP_RANGE+i] = top+15-i;

			if (bottom < 128)
				translate[BOTTOM_RANGE+i] = bottom+i;
			else
				translate[BOTTOM_RANGE+i] = bottom+15-i;
		}

		//
		// locate the original skin pixels
		//

#ifdef QUAKEWORLD
		// real model width
		tinwidth = 296;
		tinheight = 194;

		if (!player->skin)
			Skin_Find(player);
		
		original = Skin_Cache(player->skin);
		if (original) { // skin data width
			inwidth = 320;
			inheight = 200;
		} else {
			original = player_8bit_texels;
			inwidth = tinwidth;
			inheight = tinheight;
		}
#else	// UQUAKE
		currententity = &cl_entities[1+playernum];
		model = currententity->model;

		if ( !model )
			return;		// player doesn't have a model yet
		if ( model->type != mod_alias )
			return; 	// only translate skins on alias models

		paliashdr = (aliashdr_t *)Mod_Extradata (model);
		s = paliashdr->skinwidth * paliashdr->skinheight;
		if (currententity->skinnum < 0 || currententity->skinnum >= paliashdr->numskins) {
			Con_Printf("(%d): Invalid player skin #%d\n", playernum, currententity->skinnum);
			original = (byte *)paliashdr + paliashdr->texels[0];
		} else
			original = (byte *)paliashdr + paliashdr->texels[currententity->skinnum];
		if (s & 3)
			Sys_Error ("R_TranslateSkin: s&3");

		tinwidth = inwidth = paliashdr->skinwidth;
		tinheight = inheight = paliashdr->skinheight;
#endif

		// because this happens during gameplay, do it fast
		// instead of sending it through gl_upload 8
		GL_Bind(playertextures + playernum);

		scaled_width = min (gl_max_size->value, 512);
		scaled_height = min (gl_max_size->value, 256);

		// allow users to crunch sizes down even more if they want
		scaled_width >>= (int)gl_playermip->value;
		scaled_height >>= (int)gl_playermip->value;

		if ( VID_Is8bit() ) { // 8bit texture upload
			byte *out2;

			out2 = (byte *)pixels;
			memset(pixels, 0, sizeof(pixels));
			fracstep = tinwidth*0x10000/scaled_width;
			
			for (i=0 ; i < scaled_height ; i++, out2 += scaled_width) {
				inrow = original + inwidth*(i*tinheight/scaled_height);
				frac = fracstep >> 1;
				for (j=0 ; j < scaled_width ; j += 4) {
					out2[j] = translate[inrow[frac>>16]];
					frac += fracstep;
					out2[j+1] = translate[inrow[frac>>16]];
					frac += fracstep;
					out2[j+2] = translate[inrow[frac>>16]];
					frac += fracstep;
					out2[j+3] = translate[inrow[frac>>16]];
					frac += fracstep;
				}
			}

			GL_Upload8_EXT ((byte *)pixels, scaled_width, scaled_height, false, false);
		} else {
			for (i=0 ; i < 256 ; i++)
				translate32[i] = d_8to24table[translate[i]];
			out = pixels;
			memset(pixels, 0, sizeof(pixels));
			
			fracstep = tinwidth*0x10000/scaled_width;
			for ( i=0 ; i < scaled_height ; i++, out += scaled_width ) {
				inrow = original + inwidth*(i*tinheight/scaled_height);
				frac = fracstep >> 1;
				for ( j=0 ; j < scaled_width ; j += 4) {
					out[j] = translate32[inrow[frac>>16]];
					frac += fracstep;
					out[j+1] = translate32[inrow[frac>>16]];
					frac += fracstep;
					out[j+2] = translate32[inrow[frac>>16]];
					frac += fracstep;
					out[j+3] = translate32[inrow[frac>>16]];
					frac += fracstep;
				}
			}

			glTexImage2D (GL_TEXTURE_2D, 0, gl_solid_format,
							scaled_width, scaled_height, 0, GL_RGBA,
							GL_UNSIGNED_BYTE, pixels);

			glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		}
	}
	return;
}

/*
===============
R_NewMap
===============
*/
void R_NewMap (void)
{
	int		i;

	for (i=0 ; i<256 ; i++)
		d_lightstylevalue[i] = 264;		// normal light value

	memset (&r_worldentity, 0, sizeof(r_worldentity));
	r_worldentity.model = cl.worldmodel;

// clear out efrags in case the level hasn't been reloaded
// FIXME: is this one short?
	for (i=0 ; i<cl.worldmodel->numleafs ; i++)
		cl.worldmodel->leafs[i].efrags = NULL;

	r_viewleaf = NULL;
	R_ClearParticles ();

	GL_BuildLightmaps ();

	// identify sky texture
	skytexturenum = -1;
	for (i=0 ; i<cl.worldmodel->numtextures ; i++)
	{
		if (!cl.worldmodel->textures[i])
			continue;
		if (!Q_strncmp(cl.worldmodel->textures[i]->name,"sky",3) )
			skytexturenum = i;
 		cl.worldmodel->textures[i]->texturechain = NULL;
	}
	R_LoadSkys ();
}


/*
====================
R_TimeRefresh_f

For program optimization
====================
*/
void R_TimeRefresh_f (void)
{
	int			i;
	float		start, stop, time;

	glDrawBuffer  (GL_FRONT);
	glFinish ();

	start = Sys_DoubleTime ();
	for (i=0 ; i<128 ; i++)
	{
		r_refdef.viewangles[1] = i/128.0*360.0;
		R_RenderView ();
	}

	glFinish ();
	stop = Sys_DoubleTime ();
	time = stop-start;
	Con_Printf ("%f seconds (%f fps)\n", time, 128/time);

	glDrawBuffer  (GL_BACK);
	GL_EndRendering ();
}

void D_FlushCaches (void)
{
}


