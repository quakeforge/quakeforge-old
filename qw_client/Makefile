#
# QuakeWorld client makefile for unified tree
#

VERSION=2.40

ifneq (,$(findstring alpha,$(shell uname -m)))
ARCH=axp
else
ARCH=i386
endif

PROJECT_DIR=..

COMMON_DIR=$(PROJECT_DIR)/common
QW_COMMON_DIR=$(PROJECT_DIR)/qw_common
CLIENT_DIR=$(PROJECT_DIR)/qw_client
SERVER_DIR=$(PROJECT_DIR)/qw_server

BUILD_DIR=$(PROJECT_DIR)/build

X11_DIR=/usr/X11R6

INCL=-I. -I$(CLIENT_DIR) -I$(QW_COMMON_DIR) -I$(COMMON_DIR) -I$(X11_DIR)/include

CC=gcc

BASE_CFLAGS=-Wall -DQUAKEWORLD -Dstricmp=strcasecmp $(INCL)
DEBUG_CFLAGS=$(BASE_CFLAGS) -g

ifeq ($(ARCH),i386)
RELEASE_CFLAGS=$(BASE_CFLAGS) -m486 -O6 -ffast-math -funroll-loops \
	-fomit-frame-pointer -fexpensive-optimizations -malign-loops=2 \
	-malign-jumps=2 -malign-functions=2
else
RELEASE_CFLAGS=$(BASE_CFLAGS) -ffast-math -funroll-loops \
	-fomit-frame-pointer -fexpensive-optimizations
endif
GL_CFLAGS=-DGLQUAKE -I$(X11_DIR)/include -I/usr/include/glide

LDFLAGS=-lm
SVGA_LDFLAGS=$(LDFLAGS) -lvga
X11_LDFLAGS=-L$(X11_DIR)/lib $(LDFLAGS) -lX11 -lXext
GL_X11_LDFLAGS=-L$(X11_DIR)/lib $(LDFLAGS) -lGL -lX11 -lXext
GL_SVGA_LDFLAGS=-L$(X11_DIR)/lib -lvga -lGL -lglide2x -lX11 -lXext

DO_CC=$(CC) $(CFLAGS) -o $@ -c $<
DO_AS=$(CC) $(CFLAGS) -DELF -x assembler-with-cpp -o $@ -c $<

DO_GL_CC=$(CC) $(CFLAGS) $(GL_CFLAGS) -o $@ -c $<
DO_GL_AS=$(CC) $(CFLAGS) $(GL_CFLAGS) -DELF -x assembler-with-cpp -o $@ -c $<

DO_O_CC=$(CC) -O $(CFLAGS) -o $@ -c $<

#############################################################################
# SETUP AND BUILD
#############################################################################

SOFT_TARGETS=$(BUILD_DIR)/qw-client-svga \
	$(BUILD_DIR)/qw-client-x11
GL_TARGETS=$(BUILD_DIR)/qw-client-gl \
	$(BUILD_DIR)/qw-client-glide

debug:		debug-soft debug-gl

release:	release-soft release-gl

all:		debug-soft debug-gl release-soft release-gl

debug-soft:	$(BUILD_DIR) $(BUILD_DIR)/soft_qw_obj $(SOFT_OBJS) $(SOFT_AS_OBJS) $(SOFT_SVGA_OBJS) $(SOFT_X11_OBJS)
		
	$(MAKE) soft-targets CFLAGS="$(DEBUG_CFLAGS)"

debug-gl:	$(BUILD_DIR) $(BUILD_DIR)/gl_qw_obj $(GL_OBJS) $(GL_AS_OBJS) $(GL_SVGA_OBJS) $(GL_X11_OBJS)
		
	$(MAKE) gl-targets CFLAGS="$(DEBUG_CFLAGS)"

release-soft:	$(BUILD_DIR) $(BUILD_DIR)/soft_qw_obj $(SOFT_OBJS) $(SOFT_AS_OBJS) $(SOFT_SVGA_OBJS) $(SOFT_X11_OBJS)
		
	$(MAKE) soft-targets CFLAGS="$(RELEASE_CFLAGS)"

release-gl:	$(BUILD_DIR) $(BUILD_DIR)/gl_qw_obj $(GL_OBJS) $(GL_AS_OBJS) $(GL_SVGA_OBJS) $(GL_X11_OBJS)
		
	$(MAKE) gl-targets CFLAGS="$(RELEASE_CFLAGS)"

soft-targets:	$(SOFT_TARGETS)
gl-targets:	$(GL_TARGETS)

$(BUILD_DIR):
	@mkdir $(BUILD_DIR)
	
$(BUILD_DIR)/soft_qw_obj:
	@mkdir $(BUILD_DIR)/soft_qw_obj
	
$(BUILD_DIR)/gl_qw_obj:
	@mkdir $(BUILD_DIR)/gl_qw_obj

#############################################################################
# CLIENT
#############################################################################

SOFT_OBJS = \
	$(BUILD_DIR)/soft_qw_obj/cl_demo.o \
	$(BUILD_DIR)/soft_qw_obj/cl_ents.o \
	$(BUILD_DIR)/soft_qw_obj/cl_input.o \
	$(BUILD_DIR)/soft_qw_obj/cl_main.o \
	$(BUILD_DIR)/soft_qw_obj/cl_parse.o \
	$(BUILD_DIR)/soft_qw_obj/cl_pred.o \
	$(BUILD_DIR)/soft_qw_obj/cl_tent.o \
	$(BUILD_DIR)/soft_qw_obj/cl_cam.o \
	$(BUILD_DIR)/soft_qw_obj/cmd.o \
	$(BUILD_DIR)/soft_qw_obj/common.o \
	$(BUILD_DIR)/soft_qw_obj/console.o \
	$(BUILD_DIR)/soft_qw_obj/crc.o \
	$(BUILD_DIR)/soft_qw_obj/cvar.o \
	$(BUILD_DIR)/soft_qw_obj/d_edge.o \
	$(BUILD_DIR)/soft_qw_obj/d_fill.o \
	$(BUILD_DIR)/soft_qw_obj/d_init.o \
	$(BUILD_DIR)/soft_qw_obj/d_modech.o \
	$(BUILD_DIR)/soft_qw_obj/d_part.o \
	$(BUILD_DIR)/soft_qw_obj/d_polyse.o \
	$(BUILD_DIR)/soft_qw_obj/d_scan.o \
	$(BUILD_DIR)/soft_qw_obj/d_sky.o \
	$(BUILD_DIR)/soft_qw_obj/d_sprite.o \
	$(BUILD_DIR)/soft_qw_obj/d_surf.o \
	$(BUILD_DIR)/soft_qw_obj/d_vars.o \
	$(BUILD_DIR)/soft_qw_obj/d_zpoint.o \
	$(BUILD_DIR)/soft_qw_obj/draw.o \
	$(BUILD_DIR)/soft_qw_obj/keys.o \
	$(BUILD_DIR)/soft_qw_obj/mathlib.o \
	$(BUILD_DIR)/soft_qw_obj/md4.o \
	$(BUILD_DIR)/soft_qw_obj/menu.o \
	$(BUILD_DIR)/soft_qw_obj/model.o \
	$(BUILD_DIR)/soft_qw_obj/net_chan.o \
	$(BUILD_DIR)/soft_qw_obj/net_udp.o \
	$(BUILD_DIR)/soft_qw_obj/nonintel.o \
	$(BUILD_DIR)/soft_qw_obj/pmove.o \
	$(BUILD_DIR)/soft_qw_obj/pmovetst.o \
	$(BUILD_DIR)/soft_qw_obj/r_aclip.o \
	$(BUILD_DIR)/soft_qw_obj/r_alias.o \
	$(BUILD_DIR)/soft_qw_obj/r_bsp.o \
	$(BUILD_DIR)/soft_qw_obj/r_draw.o \
	$(BUILD_DIR)/soft_qw_obj/r_edge.o \
	$(BUILD_DIR)/soft_qw_obj/r_efrag.o \
	$(BUILD_DIR)/soft_qw_obj/r_light.o \
	$(BUILD_DIR)/soft_qw_obj/r_main.o \
	$(BUILD_DIR)/soft_qw_obj/r_misc.o \
	$(BUILD_DIR)/soft_qw_obj/r_part.o \
	$(BUILD_DIR)/soft_qw_obj/r_sky.o \
	$(BUILD_DIR)/soft_qw_obj/r_sprite.o \
	$(BUILD_DIR)/soft_qw_obj/r_surf.o \
	$(BUILD_DIR)/soft_qw_obj/r_vars.o \
	$(BUILD_DIR)/soft_qw_obj/sbar.o \
	$(BUILD_DIR)/soft_qw_obj/screen.o \
	$(BUILD_DIR)/soft_qw_obj/skin.o \
	$(BUILD_DIR)/soft_qw_obj/snd_dma.o \
	$(BUILD_DIR)/soft_qw_obj/snd_mem.o \
	$(BUILD_DIR)/soft_qw_obj/snd_mix.o \
	$(BUILD_DIR)/soft_qw_obj/view.o \
	$(BUILD_DIR)/soft_qw_obj/wad.o \
	$(BUILD_DIR)/soft_qw_obj/zone.o \
	$(BUILD_DIR)/soft_qw_obj/cd_linux.o \
	$(BUILD_DIR)/soft_qw_obj/sys_linux.o \
	$(BUILD_DIR)/soft_qw_obj/snd_linux.o \

ifeq ($(ARCH),i386)
SOFT_AS_OBJS = \
	$(BUILD_DIR)/soft_qw_obj/d_copy.o \
	$(BUILD_DIR)/soft_qw_obj/d_draw.o \
	$(BUILD_DIR)/soft_qw_obj/d_draw16.o \
	$(BUILD_DIR)/soft_qw_obj/d_parta.o \
	$(BUILD_DIR)/soft_qw_obj/d_polysa.o \
	$(BUILD_DIR)/soft_qw_obj/d_scana.o \
	$(BUILD_DIR)/soft_qw_obj/d_spr8.o \
	$(BUILD_DIR)/soft_qw_obj/d_varsa.o \
	$(BUILD_DIR)/soft_qw_obj/math.o \
	$(BUILD_DIR)/soft_qw_obj/r_aclipa.o \
	$(BUILD_DIR)/soft_qw_obj/r_aliasa.o \
	$(BUILD_DIR)/soft_qw_obj/r_drawa.o \
	$(BUILD_DIR)/soft_qw_obj/r_edgea.o \
	$(BUILD_DIR)/soft_qw_obj/r_varsa.o \
	$(BUILD_DIR)/soft_qw_obj/snd_mixa.o \
	$(BUILD_DIR)/soft_qw_obj/surf16.o \
	$(BUILD_DIR)/soft_qw_obj/surf8.o \
	$(BUILD_DIR)/soft_qw_obj/sys_dosa.o
else
SOFT_AS_OBJS=
endif

SOFT_SVGA_OBJS = $(BUILD_DIR)/soft_qw_obj/vid_svgalib.o
SOFT_X11_OBJS = $(BUILD_DIR)/soft_qw_obj/vid_x.o

$(BUILD_DIR)/qw-client-svga:	$(SOFT_OBJS) $(SOFT_AS_OBJS) $(SOFT_SVGA_OBJS)
	$(CC) $(CFLAGS) -o $@ $(SOFT_OBJS) $(SOFT_AS_OBJS) $(SOFT_SVGA_OBJS) \
		$(SVGA_LDFLAGS)

$(BUILD_DIR)/qw-client-x11:	$(SOFT_OBJS) $(SOFT_AS_OBJS) $(SOFT_X11_OBJS)
	$(CC) $(CFLAGS) -o $@ $(SOFT_OBJS) $(SOFT_AS_OBJS) $(SOFT_X11_OBJS) \
		$(X11_LDFLAGS)

$(BUILD_DIR)/soft_qw_obj/cl_demo.o:	$(CLIENT_DIR)/cl_demo.c
	$(DO_CC)

$(BUILD_DIR)/soft_qw_obj/cl_ents.o:	$(CLIENT_DIR)/cl_ents.c
	$(DO_CC)

$(BUILD_DIR)/soft_qw_obj/cl_input.o:	$(CLIENT_DIR)/cl_input.c
	$(DO_CC)

$(BUILD_DIR)/soft_qw_obj/cl_main.o:	$(CLIENT_DIR)/cl_main.c
	$(DO_CC)

$(BUILD_DIR)/soft_qw_obj/cl_parse.o:	$(CLIENT_DIR)/cl_parse.c
	$(DO_CC)

$(BUILD_DIR)/soft_qw_obj/cl_pred.o:	$(CLIENT_DIR)/cl_pred.c
	$(DO_CC)

$(BUILD_DIR)/soft_qw_obj/cl_tent.o:	$(CLIENT_DIR)/cl_tent.c
	$(DO_CC)

$(BUILD_DIR)/soft_qw_obj/cl_cam.o:	$(CLIENT_DIR)/cl_cam.c
	$(DO_CC)

$(BUILD_DIR)/soft_qw_obj/d_edge.o:	$(CLIENT_DIR)/d_edge.c
	$(DO_CC)

$(BUILD_DIR)/soft_qw_obj/d_fill.o:	$(CLIENT_DIR)/d_fill.c
	$(DO_CC)

$(BUILD_DIR)/soft_qw_obj/d_init.o:	$(CLIENT_DIR)/d_init.c
	$(DO_CC)

$(BUILD_DIR)/soft_qw_obj/d_modech.o:	$(CLIENT_DIR)/d_modech.c
	$(DO_CC)

$(BUILD_DIR)/soft_qw_obj/d_part.o:	$(CLIENT_DIR)/d_part.c
	$(DO_CC)

$(BUILD_DIR)/soft_qw_obj/d_polyse.o:	$(CLIENT_DIR)/d_polyse.c
	$(DO_CC)

$(BUILD_DIR)/soft_qw_obj/d_scan.o:	$(CLIENT_DIR)/d_scan.c
	$(DO_CC)

$(BUILD_DIR)/soft_qw_obj/d_sky.o:	$(CLIENT_DIR)/d_sky.c
	$(DO_CC)

$(BUILD_DIR)/soft_qw_obj/d_sprite.o:	$(CLIENT_DIR)/d_sprite.c
	$(DO_CC)

$(BUILD_DIR)/soft_qw_obj/d_surf.o:	$(CLIENT_DIR)/d_surf.c
	$(DO_CC)

$(BUILD_DIR)/soft_qw_obj/d_vars.o:	$(CLIENT_DIR)/d_vars.c
	$(DO_CC)

$(BUILD_DIR)/soft_qw_obj/d_zpoint.o:	$(CLIENT_DIR)/d_zpoint.c
	$(DO_CC)

$(BUILD_DIR)/soft_qw_obj/draw.o:	$(CLIENT_DIR)/draw.c
	$(DO_CC)

$(BUILD_DIR)/soft_qw_obj/keys.o:	$(CLIENT_DIR)/keys.c
	$(DO_CC)

$(BUILD_DIR)/soft_qw_obj/menu.o:	$(CLIENT_DIR)/menu.c
	$(DO_CC)

$(BUILD_DIR)/soft_qw_obj/model.o:	$(CLIENT_DIR)/model.c
	$(DO_CC)

$(BUILD_DIR)/soft_qw_obj/nonintel.o:	$(CLIENT_DIR)/nonintel.c
	$(DO_CC)

$(BUILD_DIR)/soft_qw_obj/r_aclip.o:	$(CLIENT_DIR)/r_aclip.c
	$(DO_CC)

$(BUILD_DIR)/soft_qw_obj/r_alias.o:	$(CLIENT_DIR)/r_alias.c
	$(DO_CC)

$(BUILD_DIR)/soft_qw_obj/r_bsp.o:	$(CLIENT_DIR)/r_bsp.c
	$(DO_CC)

$(BUILD_DIR)/soft_qw_obj/r_draw.o:	$(CLIENT_DIR)/r_draw.c
	$(DO_CC)

$(BUILD_DIR)/soft_qw_obj/r_edge.o:	$(CLIENT_DIR)/r_edge.c
	$(DO_CC)

$(BUILD_DIR)/soft_qw_obj/r_efrag.o:	$(CLIENT_DIR)/r_efrag.c
	$(DO_CC)

$(BUILD_DIR)/soft_qw_obj/r_light.o:	$(CLIENT_DIR)/r_light.c
	$(DO_CC)

$(BUILD_DIR)/soft_qw_obj/r_main.o:	$(CLIENT_DIR)/r_main.c
	$(DO_CC)

$(BUILD_DIR)/soft_qw_obj/r_misc.o:	$(CLIENT_DIR)/r_misc.c
	$(DO_CC)

$(BUILD_DIR)/soft_qw_obj/r_part.o:	$(CLIENT_DIR)/r_part.c
	$(DO_CC)

$(BUILD_DIR)/soft_qw_obj/r_sky.o:	$(CLIENT_DIR)/r_sky.c
	$(DO_CC)

$(BUILD_DIR)/soft_qw_obj/r_sprite.o:	$(CLIENT_DIR)/r_sprite.c
	$(DO_CC)

$(BUILD_DIR)/soft_qw_obj/r_surf.o:	$(CLIENT_DIR)/r_surf.c
	$(DO_CC)

$(BUILD_DIR)/soft_qw_obj/r_vars.o:	$(CLIENT_DIR)/r_vars.c
	$(DO_CC)

$(BUILD_DIR)/soft_qw_obj/sbar.o:	$(CLIENT_DIR)/sbar.c
	$(DO_CC)

$(BUILD_DIR)/soft_qw_obj/screen.o:	$(CLIENT_DIR)/screen.c
	$(DO_CC)

$(BUILD_DIR)/soft_qw_obj/skin.o:	$(CLIENT_DIR)/skin.c
	$(DO_CC)

$(BUILD_DIR)/soft_qw_obj/view.o:	$(CLIENT_DIR)/view.c
	$(DO_CC)

$(BUILD_DIR)/soft_qw_obj/sys_linux.o: $(CLIENT_DIR)/sys_linux.c
	$(DO_CC)

$(BUILD_DIR)/soft_qw_obj/cmd.o:	$(QW_COMMON_DIR)/cmd.c
	$(DO_CC)

$(BUILD_DIR)/soft_qw_obj/common.o:	$(QW_COMMON_DIR)/common.c
	$(DO_CC)

$(BUILD_DIR)/soft_qw_obj/console.o:	$(QW_COMMON_DIR)/console.c
	$(DO_CC)

$(BUILD_DIR)/soft_qw_obj/cvar.o:	$(QW_COMMON_DIR)/cvar.c
	$(DO_CC)

$(BUILD_DIR)/soft_qw_obj/md4.o:	$(QW_COMMON_DIR)/md4.c
	$(DO_CC)

$(BUILD_DIR)/soft_qw_obj/net_chan.o:	$(QW_COMMON_DIR)/net_chan.c
	$(DO_CC)

$(BUILD_DIR)/soft_qw_obj/net_udp.o:	$(QW_COMMON_DIR)/net_udp.c
	$(DO_CC)

$(BUILD_DIR)/soft_qw_obj/pmove.o:	$(QW_COMMON_DIR)/pmove.c
	$(DO_CC)

$(BUILD_DIR)/soft_qw_obj/pmovetst.o:	$(QW_COMMON_DIR)/pmovetst.c
	$(DO_CC)

$(BUILD_DIR)/soft_qw_obj/crc.o:	$(COMMON_DIR)/crc.c
	$(DO_CC)

$(BUILD_DIR)/soft_qw_obj/mathlib.o:	$(COMMON_DIR)/mathlib.c
	$(DO_CC)

$(BUILD_DIR)/soft_qw_obj/snd_dma.o:	$(COMMON_DIR)/snd_dma.c
	$(DO_CC)

$(BUILD_DIR)/soft_qw_obj/snd_mem.o:	$(COMMON_DIR)/snd_mem.c
	$(DO_CC)

$(BUILD_DIR)/soft_qw_obj/snd_mix.o:	$(COMMON_DIR)/snd_mix.c
	$(DO_CC)

$(BUILD_DIR)/soft_qw_obj/wad.o:	$(COMMON_DIR)/wad.c
	$(DO_CC)

$(BUILD_DIR)/soft_qw_obj/zone.o:	$(COMMON_DIR)/zone.c
	$(DO_CC)

$(BUILD_DIR)/soft_qw_obj/cd_linux.o:	$(COMMON_DIR)/cd_linux.c
	$(DO_CC)

$(BUILD_DIR)/soft_qw_obj/snd_linux.o: $(COMMON_DIR)/snd_linux.c
	$(DO_CC)

# Assembler code

$(BUILD_DIR)/soft_qw_obj/d_copy.o:	$(CLIENT_DIR)/d_copy.s
	$(DO_AS)

$(BUILD_DIR)/soft_qw_obj/d_draw.o:	$(CLIENT_DIR)/d_draw.s
	$(DO_AS)

$(BUILD_DIR)/soft_qw_obj/d_draw16.o:	$(CLIENT_DIR)/d_draw16.s
	$(DO_AS)

$(BUILD_DIR)/soft_qw_obj/d_parta.o:	$(CLIENT_DIR)/d_parta.s
	$(DO_AS)

$(BUILD_DIR)/soft_qw_obj/d_polysa.o:	$(CLIENT_DIR)/d_polysa.s
	$(DO_AS)

$(BUILD_DIR)/soft_qw_obj/d_scana.o:	$(CLIENT_DIR)/d_scana.s
	$(DO_AS)

$(BUILD_DIR)/soft_qw_obj/d_spr8.o:	$(CLIENT_DIR)/d_spr8.s
	$(DO_AS)

$(BUILD_DIR)/soft_qw_obj/d_varsa.o:	$(CLIENT_DIR)/d_varsa.s
	$(DO_AS)

$(BUILD_DIR)/soft_qw_obj/r_aclipa.o:	$(CLIENT_DIR)/r_aclipa.s
	$(DO_AS)

$(BUILD_DIR)/soft_qw_obj/r_aliasa.o:	$(CLIENT_DIR)/r_aliasa.s
	$(DO_AS)

$(BUILD_DIR)/soft_qw_obj/r_drawa.o:	$(CLIENT_DIR)/r_drawa.s
	$(DO_AS)

$(BUILD_DIR)/soft_qw_obj/r_edgea.o:	$(CLIENT_DIR)/r_edgea.s
	$(DO_AS)

$(BUILD_DIR)/soft_qw_obj/r_varsa.o:	$(CLIENT_DIR)/r_varsa.s
	$(DO_AS)

$(BUILD_DIR)/soft_qw_obj/surf16.o:	$(CLIENT_DIR)/surf16.s
	$(DO_AS)

$(BUILD_DIR)/soft_qw_obj/surf8.o:	$(CLIENT_DIR)/surf8.s
	$(DO_AS)

$(BUILD_DIR)/soft_qw_obj/sys_dosa.o:	$(CLIENT_DIR)/sys_dosa.s
	$(DO_AS)

$(BUILD_DIR)/soft_qw_obj/math.o:	$(COMMON_DIR)/math.s
	$(DO_AS)

$(BUILD_DIR)/soft_qw_obj/snd_mixa.o:	$(COMMON_DIR)/snd_mixa.s
	$(DO_AS)

$(BUILD_DIR)/soft_qw_obj/vid_svgalib.o: 	$(COMMON_DIR)/vid_svgalib.c
	$(DO_O_CC)

$(BUILD_DIR)/soft_qw_obj/vid_x.o:	$(COMMON_DIR)/vid_x.c
	$(DO_CC)

#
# OpenGL Client
#

GL_OBJS = \
	$(BUILD_DIR)/gl_qw_obj/cl_demo.o \
	$(BUILD_DIR)/gl_qw_obj/cl_ents.o \
	$(BUILD_DIR)/gl_qw_obj/cl_input.o \
	$(BUILD_DIR)/gl_qw_obj/cl_main.o \
	$(BUILD_DIR)/gl_qw_obj/cl_parse.o \
	$(BUILD_DIR)/gl_qw_obj/cl_pred.o \
	$(BUILD_DIR)/gl_qw_obj/cl_tent.o \
	$(BUILD_DIR)/gl_qw_obj/cl_cam.o \
	$(BUILD_DIR)/gl_qw_obj/cmd.o \
	$(BUILD_DIR)/gl_qw_obj/common.o \
	$(BUILD_DIR)/gl_qw_obj/console.o \
	$(BUILD_DIR)/gl_qw_obj/crc.o \
	$(BUILD_DIR)/gl_qw_obj/cvar.o \
	$(BUILD_DIR)/gl_qw_obj/keys.o \
	$(BUILD_DIR)/gl_qw_obj/mathlib.o \
	$(BUILD_DIR)/gl_qw_obj/md4.o \
	$(BUILD_DIR)/gl_qw_obj/menu.o \
	$(BUILD_DIR)/gl_qw_obj/net_chan.o \
	$(BUILD_DIR)/gl_qw_obj/net_udp.o \
	$(BUILD_DIR)/gl_qw_obj/nonintel.o \
	$(BUILD_DIR)/gl_qw_obj/pmove.o \
	$(BUILD_DIR)/gl_qw_obj/pmovetst.o \
	$(BUILD_DIR)/gl_qw_obj/r_part.o \
	$(BUILD_DIR)/gl_qw_obj/sbar.o \
	$(BUILD_DIR)/gl_qw_obj/skin.o \
	$(BUILD_DIR)/gl_qw_obj/snd_dma.o \
	$(BUILD_DIR)/gl_qw_obj/snd_mem.o \
	$(BUILD_DIR)/gl_qw_obj/snd_mix.o \
	$(BUILD_DIR)/gl_qw_obj/view.o \
	$(BUILD_DIR)/gl_qw_obj/wad.o \
	$(BUILD_DIR)/gl_qw_obj/zone.o \
	$(BUILD_DIR)/gl_qw_obj/cd_linux.o \
	$(BUILD_DIR)/gl_qw_obj/sys_linux.o \
	$(BUILD_DIR)/gl_qw_obj/snd_linux.o \
	\
	$(BUILD_DIR)/gl_qw_obj/gl_draw.o \
	$(BUILD_DIR)/gl_qw_obj/gl_mesh.o \
	$(BUILD_DIR)/gl_qw_obj/gl_model.o \
	$(BUILD_DIR)/gl_qw_obj/gl_ngraph.o \
	$(BUILD_DIR)/gl_qw_obj/gl_refrag.o \
	$(BUILD_DIR)/gl_qw_obj/gl_rlight.o \
	$(BUILD_DIR)/gl_qw_obj/gl_rmain.o \
	$(BUILD_DIR)/gl_qw_obj/gl_rmisc.o \
	$(BUILD_DIR)/gl_qw_obj/gl_rsurf.o \
	$(BUILD_DIR)/gl_qw_obj/gl_screen.o \
	$(BUILD_DIR)/gl_qw_obj/gl_warp.o \
	\
	$(BUILD_DIR)/gl_qw_obj/math.o \
	$(BUILD_DIR)/gl_qw_obj/snd_mixa.o \
	$(BUILD_DIR)/gl_qw_obj/sys_dosa.o
	
GL_SVGA_OBJS = $(BUILD_DIR)/gl_qw_obj/gl_vidlinux.o
GL_X11_OBJS = $(BUILD_DIR)/gl_qw_obj/gl_vidlinuxglx.o

$(BUILD_DIR)/qw-client-glide:	$(GL_OBJS) $(GL_SVGA_OBJS)
	$(CC) $(CFLAGS) -o $@ $(GL_OBJS) $(GL_SVGA_OBJS) $(LDFLAGS) $(GL_SVGA_LDFLAGS)

$(BUILD_DIR)/qw-client-gl:	$(GL_OBJS) $(GL_X11_OBJS)
	$(CC) $(CFLAGS) -o $@ $(GL_OBJS) $(GL_X11_OBJS) $(LDFLAGS) $(GL_X11_LDFLAGS)

$(BUILD_DIR)/gl_qw_obj/cl_demo.o:	$(CLIENT_DIR)/cl_demo.c
	$(DO_GL_CC)

$(BUILD_DIR)/gl_qw_obj/cl_ents.o:	$(CLIENT_DIR)/cl_ents.c
	$(DO_GL_CC)

$(BUILD_DIR)/gl_qw_obj/cl_input.o:	$(CLIENT_DIR)/cl_input.c
	$(DO_GL_CC)

$(BUILD_DIR)/gl_qw_obj/cl_main.o:	$(CLIENT_DIR)/cl_main.c
	$(DO_GL_CC)

$(BUILD_DIR)/gl_qw_obj/cl_parse.o:	$(CLIENT_DIR)/cl_parse.c
	$(DO_GL_CC)

$(BUILD_DIR)/gl_qw_obj/cl_pred.o:	$(CLIENT_DIR)/cl_pred.c
	$(DO_GL_CC)

$(BUILD_DIR)/gl_qw_obj/cl_tent.o:	$(CLIENT_DIR)/cl_tent.c
	$(DO_GL_CC)

$(BUILD_DIR)/gl_qw_obj/cl_cam.o:	$(CLIENT_DIR)/cl_cam.c
	$(DO_GL_CC)

$(BUILD_DIR)/gl_qw_obj/cmd.o:	$(QW_COMMON_DIR)/cmd.c
	$(DO_GL_CC)

$(BUILD_DIR)/gl_qw_obj/common.o:	$(QW_COMMON_DIR)/common.c
	$(DO_GL_CC)

$(BUILD_DIR)/gl_qw_obj/console.o:	$(QW_COMMON_DIR)/console.c
	$(DO_GL_CC)

$(BUILD_DIR)/gl_qw_obj/crc.o:	$(COMMON_DIR)/crc.c
	$(DO_GL_CC)

$(BUILD_DIR)/gl_qw_obj/cvar.o:	$(QW_COMMON_DIR)/cvar.c
	$(DO_GL_CC)

$(BUILD_DIR)/gl_qw_obj/keys.o:	$(CLIENT_DIR)/keys.c
	$(DO_GL_CC)

$(BUILD_DIR)/gl_qw_obj/mathlib.o:	$(COMMON_DIR)/mathlib.c
	$(DO_GL_CC)

$(BUILD_DIR)/gl_qw_obj/md4.o:	$(QW_COMMON_DIR)/md4.c
	$(DO_GL_CC)

$(BUILD_DIR)/gl_qw_obj/menu.o:	$(CLIENT_DIR)/menu.c
	$(DO_GL_CC)

$(BUILD_DIR)/gl_qw_obj/net_chan.o:	$(QW_COMMON_DIR)/net_chan.c
	$(DO_GL_CC)

$(BUILD_DIR)/gl_qw_obj/net_udp.o:	$(QW_COMMON_DIR)/net_udp.c
	$(DO_GL_CC)

$(BUILD_DIR)/gl_qw_obj/nonintel.o:	$(CLIENT_DIR)/nonintel.c
	$(DO_GL_CC)

$(BUILD_DIR)/gl_qw_obj/pmove.o:	$(QW_COMMON_DIR)/pmove.c
	$(DO_GL_CC)

$(BUILD_DIR)/gl_qw_obj/pmovetst.o:	$(QW_COMMON_DIR)/pmovetst.c
	$(DO_GL_CC)

$(BUILD_DIR)/gl_qw_obj/r_part.o:	$(CLIENT_DIR)/r_part.c
	$(DO_GL_CC)

$(BUILD_DIR)/gl_qw_obj/sbar.o:	$(CLIENT_DIR)/sbar.c
	$(DO_GL_CC)

$(BUILD_DIR)/gl_qw_obj/screen.o:	$(CLIENT_DIR)/screen.c
	$(DO_GL_CC)

$(BUILD_DIR)/gl_qw_obj/skin.o:	$(CLIENT_DIR)/skin.c
	$(DO_GL_CC)

$(BUILD_DIR)/gl_qw_obj/snd_dma.o:	$(COMMON_DIR)/snd_dma.c
	$(DO_GL_CC)

$(BUILD_DIR)/gl_qw_obj/snd_mem.o:	$(COMMON_DIR)/snd_mem.c
	$(DO_GL_CC)

$(BUILD_DIR)/gl_qw_obj/snd_mix.o:	$(COMMON_DIR)/snd_mix.c
	$(DO_GL_CC)

$(BUILD_DIR)/gl_qw_obj/view.o:	$(CLIENT_DIR)/view.c
	$(DO_GL_CC)

$(BUILD_DIR)/gl_qw_obj/wad.o:	$(COMMON_DIR)/wad.c
	$(DO_GL_CC)

$(BUILD_DIR)/gl_qw_obj/zone.o:	$(COMMON_DIR)/zone.c
	$(DO_GL_CC)

$(BUILD_DIR)/gl_qw_obj/cd_linux.o:	$(COMMON_DIR)/cd_linux.c
	$(DO_GL_CC)

$(BUILD_DIR)/gl_qw_obj/sys_linux.o:	$(CLIENT_DIR)/sys_linux.c
	$(DO_GL_CC)

$(BUILD_DIR)/gl_qw_obj/snd_linux.o:	$(COMMON_DIR)/snd_linux.c
	$(DO_GL_CC)

$(BUILD_DIR)/gl_qw_obj/gl_draw.o:	$(CLIENT_DIR)/gl_draw.c
	$(DO_GL_CC)

$(BUILD_DIR)/gl_qw_obj/gl_mesh.o:	$(CLIENT_DIR)/gl_mesh.c
	$(DO_GL_CC)

$(BUILD_DIR)/gl_qw_obj/gl_model.o:	$(CLIENT_DIR)/gl_model.c
	$(DO_GL_CC)

$(BUILD_DIR)/gl_qw_obj/gl_ngraph.o:	$(CLIENT_DIR)/gl_ngraph.c
	$(DO_GL_CC)

$(BUILD_DIR)/gl_qw_obj/gl_refrag.o:	$(CLIENT_DIR)/gl_refrag.c
	$(DO_GL_CC)

$(BUILD_DIR)/gl_qw_obj/gl_rlight.o:	$(CLIENT_DIR)/gl_rlight.c
	$(DO_GL_CC)

$(BUILD_DIR)/gl_qw_obj/gl_rmain.o:	$(CLIENT_DIR)/gl_rmain.c
	$(DO_GL_CC)

$(BUILD_DIR)/gl_qw_obj/gl_rmisc.o:	$(CLIENT_DIR)/gl_rmisc.c
	$(DO_GL_CC)

$(BUILD_DIR)/gl_qw_obj/gl_rsurf.o:	$(CLIENT_DIR)/gl_rsurf.c
	$(DO_GL_CC)

$(BUILD_DIR)/gl_qw_obj/gl_screen.o:	$(CLIENT_DIR)/gl_screen.c
	$(DO_GL_CC)

$(BUILD_DIR)/gl_qw_obj/gl_vidlinux.o: $(CLIENT_DIR)/gl_vidlinux.c
	$(DO_GL_CC)

$(BUILD_DIR)/gl_qw_obj/gl_vidlinuxglx.o: $(COMMON_DIR)/gl_vidlinuxglx.c
	$(DO_GL_CC)

$(BUILD_DIR)/gl_qw_obj/gl_warp.o:	$(CLIENT_DIR)/gl_warp.c
	$(DO_GL_CC)

$(BUILD_DIR)/gl_qw_obj/sys_dosa.o:	$(CLIENT_DIR)/sys_dosa.s
	$(DO_GL_AS)

$(BUILD_DIR)/gl_qw_obj/math.o:	$(COMMON_DIR)/math.s
	$(DO_GL_AS)

$(BUILD_DIR)/gl_qw_obj/snd_mixa.o:	$(COMMON_DIR)/snd_mixa.s
	$(DO_GL_AS)

#############################################################################
# MISC
#############################################################################

clean:	clean-soft clean-gl
	@-rmdir $(BUILD_DIR)
	@echo Done.

clean-soft:
	@-rm -f $(SOFT_TARGETS)
	@-rm -f $(SOFT_OBJS)
	@-rmdir $(BUILD_DIR)/soft_qw_obj

clean-gl:
	@-rm -f $(GL_TARGETS)
	@-rm -f $(GL_OBJS)
	@-rmdir $(BUILD_DIR)/gl_qw_obj

