#
# UQuake makefile for unified tree
#

VERSION=0.1.0

ifneq (,$(findstring alpha,$(shell uname -m)))
ARCH=axp
else
ARCH=i386
endif

PROJECT_DIR=..

COMMON_DIR=$(PROJECT_DIR)/common
UQ_DIR=$(PROJECT_DIR)/uquake

BUILD_DEBUG_DIR=debug-$(ARCH)
BUILD_RELEASE_DIR=release-$(ARCH)

X11_DIR=/usr/X11R6

INCL=-I. -I$(UQ_DIR) -I$(COMMON_DIR) -I$(X11_DIR)/include

CC=gcc

BASE_CFLAGS=-Wall -Dstricmp=strcasecmp $(INCL)
DEBUG_CFLAGS=$(BASE_CFLAGS) -g

ifeq ($(ARCH),i386)
RELEASE_CFLAGS=$(BASE_CFLAGS) -m486 -O6 -ffast-math -funroll-loops \
	-fomit-frame-pointer -fexpensive-optimizations -malign-loops=2 \
	-malign-jumps=2 -malign-functions=2
else
RELEASE_CFLAGS=$(BASE_CFLAGS) -ffast-math -funroll-loops \
	-fomit-frame-pointer -fexpensive-optimizations
endif
GL_CFLAGS=-DGLQUAKE -I$(X11_DIR)/include

LDFLAGS=-lm
SVGA_LDFLAGS=$(LDFLAGS) -lvga
X11_LDFLAGS=-L$(X11_DIR)/lib $(LDFLAGS) -lX11 -lXext
GL_X11_LDFLAGS=-L$(X11_DIR)/lib $(LDFLAGS) -lGL -lX11 -lXext
GL_SVGA_LDFLAGS=-L$(X11_DIR)/lib -lvga -lMesaGL -lglide2x -lX11 -lXext

DO_CC=$(CC) $(CFLAGS) -o $@ -c $<
DO_AS=$(CC) $(CFLAGS) -DELF -x assembler-with-cpp -o $@ -c $<

DO_GL_CC=$(CC) $(CFLAGS) $(GL_CFLAGS) -o $@ -c $<
DO_GL_AS=$(CC) $(CFLAGS) $(GL_CFLAGS) -DELF -x assembler-with-cpp -o $@ -c $<

DO_O_CC=$(CC) -O $(CFLAGS) -o $@ -c $<

#############################################################################
# SETUP AND BUILD
#############################################################################

SOFT_TARGETS=$(OBJ_DIR)/quake-svga \
	$(OBJ_DIR)/quake-x
GL_TARGETS=$(OBJ_DIR)/quake-gl \
	$(OBJ_DIR)/quake-glide

debug:		debug-soft #debug-gl

release:	release-soft #release-gl

all:		debug-soft release-soft #debug-gl release-gl

debug-soft:	$(BUILD_DEBUG_DIR) $(BUILD_DEBUG_DIR)/soft_obj $(SOFT_OBJS) $(SOFT_AS_OBJS) $(SOFT_SVGA_OBJS) $(SOFT_X11_OBJS)
		
	$(MAKE) soft-targets OBJ_DIR=$(BUILD_DEBUG_DIR) CFLAGS="$(DEBUG_CFLAGS)"

debug-gl:	$(BUILD_DEBUG_DIR) $(BUILD_DEBUG_DIR)/gl_obj $(GL_OBJS) $(GL_AS_OBJS) $(GL_SVGA_OBJS) $(GL_X11_OBJS)
		
	$(MAKE) gl-targets OBJ_DIR=$(BUILD_DEBUG_DIR) CFLAGS="$(DEBUG_CFLAGS)"

release-soft:	$(BUILD_RELEASE_DIR) $(BUILD_RELEASE_DIR)/soft_obj $(SOFT_OBJS) $(SOFT_AS_OBJS) $(SOFT_SVGA_OBJS) $(SOFT_X11_OBJS)
		
	$(MAKE) soft-targets OBJ_DIR=$(BUILD_RELEASE_DIR) CFLAGS="$(RELEASE_CFLAGS)"

release-gl:	$(BUILD_RELEASE_DIR) $(BUILD_RELEASE_DIR)/gl_obj $(GL_OBJS) $(GL_AS_OBJS) $(GL_SVGA_OBJS) $(GL_X11_OBJS)
		
	$(MAKE) gl-targets OBJ_DIR=$(BUILD_RELEASE_DIR) CFLAGS="$(RELEASE_CFLAGS)"

soft-targets:	$(SOFT_TARGETS)
gl-targets:	$(GL_TARGETS)

$(BUILD_DEBUG_DIR):
	@mkdir $(BUILD_DEBUG_DIR)
	
$(BUILD_DEBUG_DIR)/soft_obj:
	@mkdir $(BUILD_DEBUG_DIR)/soft_obj
	
$(BUILD_DEBUG_DIR)/gl_obj:
	@mkdir $(BUILD_DEBUG_DIR)/gl_obj

$(BUILD_RELEASE_DIR):
	@mkdir $(BUILD_RELEASE_DIR)
	
$(BUILD_RELEASE_DIR)/soft_obj:
	@mkdir $(BUILD_RELEASE_DIR)/soft_obj
	
$(BUILD_RELEASE_DIR)/gl_obj:
	@mkdir $(BUILD_RELEASE_DIR)/gl_obj

#############################################################################
# Software Renderer
#############################################################################

SOFT_OBJS = \
	$(OBJ_DIR)/soft_obj/cl_demo.o \
	$(OBJ_DIR)/soft_obj/cl_input.o \
	$(OBJ_DIR)/soft_obj/cl_main.o \
	$(OBJ_DIR)/soft_obj/cl_parse.o \
	$(OBJ_DIR)/soft_obj/cl_tent.o \
	$(OBJ_DIR)/soft_obj/chase.o \
	$(OBJ_DIR)/soft_obj/cmd.o \
	$(OBJ_DIR)/soft_obj/common.o \
	$(OBJ_DIR)/soft_obj/console.o \
	$(OBJ_DIR)/soft_obj/crc.o \
	$(OBJ_DIR)/soft_obj/cvar.o \
	$(OBJ_DIR)/soft_obj/draw.o \
	$(OBJ_DIR)/soft_obj/d_edge.o \
	$(OBJ_DIR)/soft_obj/d_fill.o \
	$(OBJ_DIR)/soft_obj/d_init.o \
	$(OBJ_DIR)/soft_obj/d_modech.o \
	$(OBJ_DIR)/soft_obj/d_part.o \
	$(OBJ_DIR)/soft_obj/d_polyse.o \
	$(OBJ_DIR)/soft_obj/d_scan.o \
	$(OBJ_DIR)/soft_obj/d_sky.o \
	$(OBJ_DIR)/soft_obj/d_sprite.o \
	$(OBJ_DIR)/soft_obj/d_surf.o \
	$(OBJ_DIR)/soft_obj/d_vars.o \
	$(OBJ_DIR)/soft_obj/d_zpoint.o \
	$(OBJ_DIR)/soft_obj/host.o \
	$(OBJ_DIR)/soft_obj/host_cmd.o \
	$(OBJ_DIR)/soft_obj/keys.o \
	$(OBJ_DIR)/soft_obj/mathlib.o \
	$(OBJ_DIR)/soft_obj/menu.o \
	$(OBJ_DIR)/soft_obj/model.o \
	$(OBJ_DIR)/soft_obj/net_bsd.o \
	$(OBJ_DIR)/soft_obj/net_dgrm.o \
	$(OBJ_DIR)/soft_obj/net_loop.o \
	$(OBJ_DIR)/soft_obj/net_main.o \
	$(OBJ_DIR)/soft_obj/net_udp.o \
	$(OBJ_DIR)/soft_obj/net_vcr.o \
	$(OBJ_DIR)/soft_obj/nonintel.o \
	$(OBJ_DIR)/soft_obj/pr_cmds.o \
	$(OBJ_DIR)/soft_obj/pr_edict.o \
	$(OBJ_DIR)/soft_obj/pr_exec.o \
	$(OBJ_DIR)/soft_obj/r_aclip.o \
	$(OBJ_DIR)/soft_obj/r_alias.o \
	$(OBJ_DIR)/soft_obj/r_bsp.o \
	$(OBJ_DIR)/soft_obj/r_draw.o \
	$(OBJ_DIR)/soft_obj/r_edge.o \
	$(OBJ_DIR)/soft_obj/r_efrag.o \
	$(OBJ_DIR)/soft_obj/r_light.o \
	$(OBJ_DIR)/soft_obj/r_main.o \
	$(OBJ_DIR)/soft_obj/r_misc.o \
	$(OBJ_DIR)/soft_obj/r_part.o \
	$(OBJ_DIR)/soft_obj/r_sky.o \
	$(OBJ_DIR)/soft_obj/r_sprite.o \
	$(OBJ_DIR)/soft_obj/r_surf.o \
	$(OBJ_DIR)/soft_obj/r_vars.o \
	$(OBJ_DIR)/soft_obj/sbar.o \
	$(OBJ_DIR)/soft_obj/screen.o \
	$(OBJ_DIR)/soft_obj/snd_dma.o \
	$(OBJ_DIR)/soft_obj/snd_mem.o \
	$(OBJ_DIR)/soft_obj/snd_mix.o \
	$(OBJ_DIR)/soft_obj/sv_main.o \
	$(OBJ_DIR)/soft_obj/sv_phys.o \
	$(OBJ_DIR)/soft_obj/sv_move.o \
	$(OBJ_DIR)/soft_obj/sv_user.o \
	$(OBJ_DIR)/soft_obj/view.o \
	$(OBJ_DIR)/soft_obj/wad.o \
	$(OBJ_DIR)/soft_obj/world.o \
	$(OBJ_DIR)/soft_obj/zone.o \
	$(OBJ_DIR)/soft_obj/cd_linux.o \
	$(OBJ_DIR)/soft_obj/snd_linux.o \
	$(OBJ_DIR)/soft_obj/sys_linux.o \

ifeq ($(ARCH),i386)
SOFT_AS_OBJS = \
	$(OBJ_DIR)/soft_obj/d_copy.o \
	$(OBJ_DIR)/soft_obj/d_draw.o \
	$(OBJ_DIR)/soft_obj/d_draw16.o \
	$(OBJ_DIR)/soft_obj/d_parta.o \
	$(OBJ_DIR)/soft_obj/d_polysa.o \
	$(OBJ_DIR)/soft_obj/d_scana.o \
	$(OBJ_DIR)/soft_obj/d_spr8.o \
	$(OBJ_DIR)/soft_obj/d_varsa.o \
	$(OBJ_DIR)/soft_obj/math.o \
	$(OBJ_DIR)/soft_obj/r_aliasa.o \
	$(OBJ_DIR)/soft_obj/r_drawa.o \
	$(OBJ_DIR)/soft_obj/r_edgea.o \
	$(OBJ_DIR)/soft_obj/r_varsa.o \
	$(OBJ_DIR)/soft_obj/surf16.o \
	$(OBJ_DIR)/soft_obj/surf8.o \
	$(OBJ_DIR)/soft_obj/worlda.o \
	$(OBJ_DIR)/soft_obj/r_aclipa.o \
	$(OBJ_DIR)/soft_obj/snd_mixa.o \
	$(OBJ_DIR)/soft_obj/sys_dosa.o
else
SOFT_AS_OBJS=
endif

SOFT_SVGA_OBJS = $(OBJ_DIR)/soft_obj/vid_svgalib.o
SOFT_X11_OBJS = $(OBJ_DIR)/soft_obj/vid_x.o

$(OBJ_DIR)/quake-svga:	$(SOFT_OBJS) $(SOFT_AS_OBJS) $(SOFT_SVGA_OBJS)
	$(CC) $(CFLAGS) -o $@ $(SOFT_OBJS) $(SOFT_AS_OBJS) $(SOFT_SVGA_OBJS) \
		$(SVGA_LDFLAGS)

$(OBJ_DIR)/quake-x:	$(SOFT_OBJS) $(SOFT_AS_OBJS) $(SOFT_X11_OBJS)
	$(CC) $(CFLAGS) -o $@ $(SOFT_OBJS) $(SOFT_AS_OBJS) $(SOFT_X11_OBJS) \
		$(X11_LDFLAGS)


$(OBJ_DIR)/soft_obj/cl_demo.o:	$(UQ_DIR)/cl_demo.c
	$(DO_CC)

$(OBJ_DIR)/soft_obj/cl_input.o:	$(UQ_DIR)/cl_input.c
	$(DO_CC)

$(OBJ_DIR)/soft_obj/cl_main.o:	$(UQ_DIR)/cl_main.c
	$(DO_CC)

$(OBJ_DIR)/soft_obj/cl_parse.o:	$(UQ_DIR)/cl_parse.c
	$(DO_CC)

$(OBJ_DIR)/soft_obj/cl_tent.o:	$(UQ_DIR)/cl_tent.c
	$(DO_CC)

$(OBJ_DIR)/soft_obj/chase.o:	$(UQ_DIR)/chase.c
	$(DO_CC)

$(OBJ_DIR)/soft_obj/cmd.o:	$(UQ_DIR)/cmd.c
	$(DO_CC)

$(OBJ_DIR)/soft_obj/common.o:	$(UQ_DIR)/common.c
	$(DO_CC)

$(OBJ_DIR)/soft_obj/console.o:	$(UQ_DIR)/console.c
	$(DO_CC)

$(OBJ_DIR)/soft_obj/crc.o:	$(UQ_DIR)/crc.c
	$(DO_CC)

$(OBJ_DIR)/soft_obj/cvar.o:	$(UQ_DIR)/cvar.c
	$(DO_CC)

$(OBJ_DIR)/soft_obj/draw.o:	$(UQ_DIR)/draw.c
	$(DO_CC)

$(OBJ_DIR)/soft_obj/d_edge.o:	$(UQ_DIR)/d_edge.c
	$(DO_CC)

$(OBJ_DIR)/soft_obj/d_fill.o:	$(UQ_DIR)/d_fill.c
	$(DO_CC)

$(OBJ_DIR)/soft_obj/d_init.o:	$(UQ_DIR)/d_init.c
	$(DO_CC)

$(OBJ_DIR)/soft_obj/d_modech.o:	$(UQ_DIR)/d_modech.c
	$(DO_CC)

$(OBJ_DIR)/soft_obj/d_part.o:	$(UQ_DIR)/d_part.c
	$(DO_CC)

$(OBJ_DIR)/soft_obj/d_polyse.o:	$(UQ_DIR)/d_polyse.c
	$(DO_CC)

$(OBJ_DIR)/soft_obj/d_scan.o:	$(UQ_DIR)/d_scan.c
	$(DO_CC)

$(OBJ_DIR)/soft_obj/d_sky.o:	$(UQ_DIR)/d_sky.c
	$(DO_CC)

$(OBJ_DIR)/soft_obj/d_sprite.o:	$(UQ_DIR)/d_sprite.c
	$(DO_CC)

$(OBJ_DIR)/soft_obj/d_surf.o:	$(UQ_DIR)/d_surf.c
	$(DO_CC)

$(OBJ_DIR)/soft_obj/d_vars.o:	$(UQ_DIR)/d_vars.c
	$(DO_CC)

$(OBJ_DIR)/soft_obj/d_zpoint.o:	$(UQ_DIR)/d_zpoint.c
	$(DO_CC)

$(OBJ_DIR)/soft_obj/host.o :     $(UQ_DIR)/host.c
	$(DO_CC)

$(OBJ_DIR)/soft_obj/host_cmd.o : $(UQ_DIR)/host_cmd.c
	$(DO_CC)

$(OBJ_DIR)/soft_obj/keys.o:	$(UQ_DIR)/keys.c
	$(DO_CC)

$(OBJ_DIR)/soft_obj/mathlib.o:	$(COMMON_DIR)/mathlib.c
	$(DO_CC)

$(OBJ_DIR)/soft_obj/menu.o:	$(UQ_DIR)/menu.c
	$(DO_CC)

$(OBJ_DIR)/soft_obj/model.o:	$(UQ_DIR)/model.c
	$(DO_CC)

$(OBJ_DIR)/soft_obj/net_dgrm.o: $(UQ_DIR)/net_dgrm.c
	$(DO_CC)

$(OBJ_DIR)/soft_obj/net_loop.o: $(UQ_DIR)/net_loop.c
	$(DO_CC)

$(OBJ_DIR)/soft_obj/net_main.o: $(UQ_DIR)/net_main.c
	$(DO_CC)

$(OBJ_DIR)/soft_obj/net_vcr.o:	$(UQ_DIR)/net_vcr.c
	$(DO_CC)

$(OBJ_DIR)/soft_obj/net_udp.o:	$(UQ_DIR)/net_udp.c
	$(DO_CC)

$(OBJ_DIR)/soft_obj/net_bsd.o:	$(UQ_DIR)/net_bsd.c
	$(DO_CC)

$(OBJ_DIR)/soft_obj/nonintel.o:	$(UQ_DIR)/nonintel.c
	$(DO_CC)

$(OBJ_DIR)/soft_obj/pr_cmds.o :  $(UQ_DIR)/pr_cmds.c
	$(DO_CC)

$(OBJ_DIR)/soft_obj/pr_edict.o : $(UQ_DIR)/pr_edict.c
	$(DO_CC)

$(OBJ_DIR)/soft_obj/pr_exec.o :  $(UQ_DIR)/pr_exec.c
	$(DO_CC)

$(OBJ_DIR)/soft_obj/r_aclip.o:	$(UQ_DIR)/r_aclip.c
	$(DO_CC)

$(OBJ_DIR)/soft_obj/r_alias.o:	$(UQ_DIR)/r_alias.c
	$(DO_CC)

$(OBJ_DIR)/soft_obj/r_bsp.o:	$(UQ_DIR)/r_bsp.c
	$(DO_CC)

$(OBJ_DIR)/soft_obj/r_draw.o:	$(UQ_DIR)/r_draw.c
	$(DO_CC)

$(OBJ_DIR)/soft_obj/r_edge.o:	$(UQ_DIR)/r_edge.c
	$(DO_CC)

$(OBJ_DIR)/soft_obj/r_efrag.o:	$(UQ_DIR)/r_efrag.c
	$(DO_CC)

$(OBJ_DIR)/soft_obj/r_light.o:	$(UQ_DIR)/r_light.c
	$(DO_CC)

$(OBJ_DIR)/soft_obj/r_main.o:	$(UQ_DIR)/r_main.c
	$(DO_CC)

$(OBJ_DIR)/soft_obj/r_misc.o:	$(UQ_DIR)/r_misc.c
	$(DO_CC)

$(OBJ_DIR)/soft_obj/r_part.o:	$(UQ_DIR)/r_part.c
	$(DO_CC)

$(OBJ_DIR)/soft_obj/r_sky.o:	$(UQ_DIR)/r_sky.c
	$(DO_CC)

$(OBJ_DIR)/soft_obj/r_sprite.o:	$(UQ_DIR)/r_sprite.c
	$(DO_CC)

$(OBJ_DIR)/soft_obj/r_surf.o:	$(UQ_DIR)/r_surf.c
	$(DO_CC)

$(OBJ_DIR)/soft_obj/r_vars.o:	$(UQ_DIR)/r_vars.c
	$(DO_CC)

$(OBJ_DIR)/soft_obj/sbar.o:	$(UQ_DIR)/sbar.c
	$(DO_CC)

$(OBJ_DIR)/soft_obj/screen.o:	$(UQ_DIR)/screen.c
	$(DO_CC)

$(OBJ_DIR)/soft_obj/snd_dma.o:  $(COMMON_DIR)/snd_dma.c
	$(DO_CC)

$(OBJ_DIR)/soft_obj/snd_mem.o:  $(COMMON_DIR)/snd_mem.c
	$(DO_CC)

$(OBJ_DIR)/soft_obj/snd_mix.o:  $(COMMON_DIR)/snd_mix.c
	$(DO_CC)

$(OBJ_DIR)/soft_obj/snd_linux.o: $(COMMON_DIR)/snd_linux.c
	$(DO_CC)

$(OBJ_DIR)/soft_obj/sv_main.o:	$(UQ_DIR)/sv_main.c
	$(DO_CC)

$(OBJ_DIR)/soft_obj/sv_phys.o:	$(UQ_DIR)/sv_phys.c
	$(DO_CC)

$(OBJ_DIR)/soft_obj/sv_move.o:	$(UQ_DIR)/sv_move.c
	$(DO_CC)

$(OBJ_DIR)/soft_obj/sv_user.o:	$(UQ_DIR)/sv_user.c
	$(DO_CC)

$(OBJ_DIR)/soft_obj/view.o:	$(UQ_DIR)/view.c
	$(DO_CC)

$(OBJ_DIR)/soft_obj/wad.o:	$(COMMON_DIR)/wad.c
	$(DO_CC)

$(OBJ_DIR)/soft_obj/world.o:	$(UQ_DIR)/world.c
	$(DO_CC)

$(OBJ_DIR)/soft_obj/zone.o:	$(COMMON_DIR)/zone.c
	$(DO_CC)

$(OBJ_DIR)/soft_obj/cd_linux.o:	$(COMMON_DIR)/cd_linux.c
	$(DO_CC)

$(OBJ_DIR)/soft_obj/sys_linux.o: $(UQ_DIR)/sys_linux.c
	$(DO_CC)

# Assembler code

$(OBJ_DIR)/soft_obj/d_copy.o:	$(UQ_DIR)/d_copy.s
	$(DO_AS)

$(OBJ_DIR)/soft_obj/d_draw.o:	$(UQ_DIR)/d_draw.s
	$(DO_AS)

$(OBJ_DIR)/soft_obj/d_draw16.o:	$(UQ_DIR)/d_draw16.s
	$(DO_AS)

$(OBJ_DIR)/soft_obj/d_parta.o:	$(UQ_DIR)/d_parta.s
	$(DO_AS)

$(OBJ_DIR)/soft_obj/d_polysa.o:	$(UQ_DIR)/d_polysa.s
	$(DO_AS)

$(OBJ_DIR)/soft_obj/d_scana.o:	$(UQ_DIR)/d_scana.s
	$(DO_AS)

$(OBJ_DIR)/soft_obj/d_spr8.o:	$(UQ_DIR)/d_spr8.s
	$(DO_AS)

$(OBJ_DIR)/soft_obj/d_varsa.o:	$(UQ_DIR)/d_varsa.s
	$(DO_AS)

$(OBJ_DIR)/soft_obj/math.o:	$(COMMON_DIR)/math.s
	$(DO_AS)

$(OBJ_DIR)/soft_obj/r_aclipa.o:	$(UQ_DIR)/r_aclipa.s
	$(DO_AS)

$(OBJ_DIR)/soft_obj/r_aliasa.o:	$(UQ_DIR)/r_aliasa.s
	$(DO_AS)

$(OBJ_DIR)/soft_obj/r_drawa.o:	$(UQ_DIR)/r_drawa.s
	$(DO_AS)

$(OBJ_DIR)/soft_obj/r_edgea.o:	$(UQ_DIR)/r_edgea.s
	$(DO_AS)

$(OBJ_DIR)/soft_obj/r_varsa.o:	$(UQ_DIR)/r_varsa.s
	$(DO_AS)

$(OBJ_DIR)/soft_obj/surf16.o:	$(UQ_DIR)/surf16.s
	$(DO_AS)

$(OBJ_DIR)/soft_obj/surf8.o:	$(UQ_DIR)/surf8.s
	$(DO_AS)

$(OBJ_DIR)/soft_obj/snd_mixa.o:	$(COMMON_DIR)/snd_mixa.s
	$(DO_AS)

$(OBJ_DIR)/soft_obj/sys_dosa.o:	$(UQ_DIR)/sys_dosa.s
	$(DO_AS)

$(OBJ_DIR)/soft_obj/worlda.o:	$(UQ_DIR)/worlda.s
	$(DO_AS)

# specific things

$(OBJ_DIR)/soft_obj/vid_svgalib.o: $(COMMON_DIR)/vid_svgalib.c
	$(DO_O_CC)

$(OBJ_DIR)/soft_obj/vid_x.o:	$(COMMON_DIR)/vid_x.c
	$(DO_CC)
