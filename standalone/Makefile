#
# Quake Makefile for Linux 2.0
#
# Aug '98 by Zoid <zoid@idsoftware.com>
#
# ELF only
#

VERSION=1.09

ifneq (,$(findstring alpha,$(shell uname -m)))
ARCH=axp
else
ARCH=i386
endif
NOARCH=noarch

ROOT_DIR=$(shell pwd)
BUILD_DEBUG_DIR=debug-$(ARCH)$(GLIBC)
BUILD_RELEASE_DIR=release-$(ARCH)$(GLIBC)

X11_DIR=/usr/X11R6
GL_DIR=/usr/src/mesa-3.0

CC=gcc

PROJECT_DIR=..

COMMON_DIR=$(PROJECT_DIR)/common
POQ_DIR=$(PROJECT_DIR)/standalone


INCL=-I. -I$(COMMON_DIR) -I$(POQ_DIR)/include

BASE_CFLAGS=$(INCL) -Dstricmp=strcasecmp
DEBUG_CFLAGS=$(BASE_CFLAGS) -g

ifeq ($(ARCH),axp)
RELEASE_CFLAGS=$(BASE_CFLAGS) -ffast-math -funroll-loops \
	-fomit-frame-pointer -fexpensive-optimizations
else
RELEASE_CFLAGS=$(BASE_CFLAGS) -m486 -O6 -ffast-math -funroll-loops \
	-fomit-frame-pointer -fexpensive-optimizations -malign-loops=2 \
	-malign-jumps=2 -malign-functions=2
endif

GL_CFLAGS=-DGLQUAKE -I$(X11_DIR)/include -I$(GL_DIR)/include -I/usr/include/glide
X11_CFLAGS=-DX11 -I$(X11_DIR)/include

LDFLAGS=-lm
SVGA_LDFLAGS=-lvga
X11_LDFLAGS=-L$(X11_DIR)/lib -lX11 -lXext -lXxf86dga
GL_LDFLAGS=-L$(X11_DIR)/lib -L$(MESA_DIR)/lib -L/usr/local/lib -lGL -lX11 -lXext -ldl -lglide2x -lXxf86dga -lXxf86vm
GLIDE_LDFLAGS=-L$(X11_DIR)/lib -L$(MESA_DIR)/lib -L/usr/local/lib -lGL -lX11 -lXext -ldl -lglide2x -lXxf86dga -lXxf86vm -lm -lvga

#GLLDFLAGS=-L/usr/X11R6/lib -L/usr/local/lib -lGL -lX11 -lXext -ldl -lXxf86dga -lXxf86vm -lm

DO_CC=$(CC) $(CFLAGS) -o $@ -c $<
DO_DEBUG_CC=$(CC) $(DEBUG_CFLAGS) -o $@ -c $<
DO_GL_CC=$(CC) $(CFLAGS) $(GL_CFLAGS) -o $@ -c $<
DO_GL_DEBUG_CC=$(CC) $(DEBUG_CFLAGS) $(GL_CFLAGS) -o $@ -c $<
DO_X11_CC=$(CC) $(CFLAGS) $(X11_CFLAGS) -o $@ -c $<
DO_X11_DEBUG_CC=$(CC) $(DEBUG_CFLAGS) $(X11_CFLAGS) -o $@ -c $<
DO_O_CC=$(CC) -O $(CFLAGS) -o $@ -c $<
DO_AS=$(CC) $(CFLAGS) -DELF -x assembler-with-cpp -o $@ -c $<
DO_GL_AS=$(CC) $(CFLAGS) $(GL_CFLAGS) -DELF -x assembler-with-cpp -o $@ -c $<

#############################################################################
# SETUP AND BUILD
#############################################################################

TARGETS=$(BUILDDIR)/bin/quake-svga \
	$(BUILDDIR)/bin/quake-x11 \
	$(BUILDDIR)/bin/quake-gl \
	$(BUILDDIR)/bin/quake-glide

build_debug:
	@-mkdir $(BUILD_DEBUG_DIR) \
		$(BUILD_DEBUG_DIR)/bin \
		$(BUILD_DEBUG_DIR)/gl \
		$(BUILD_DEBUG_DIR)/svga \
		$(BUILD_DEBUG_DIR)/x11
	$(MAKE) targets BUILDDIR=$(BUILD_DEBUG_DIR) CFLAGS="$(DEBUG_CFLAGS)"

build_release:
	@-mkdir $(BUILD_RELEASE_DIR) \
		$(BUILD_RELEASE_DIR)/bin \
		$(BUILD_RELEASE_DIR)/gl \
		$(BUILD_RELEASE_DIR)/svga \
		$(BUILD_RELEASE_DIR)/x11
	$(MAKE) targets BUILDDIR=$(BUILD_RELEASE_DIR) CFLAGS="$(RELEASE_CFLAGS)"

all: build_debug build_release

targets: $(TARGETS)

#############################################################################
# SVGALIB Quake
#############################################################################

SQUAKE_OBJS = \
	$(BUILDDIR)/svga/cl_demo.o \
	$(BUILDDIR)/svga/cl_input.o \
	$(BUILDDIR)/svga/cl_main.o \
	$(BUILDDIR)/svga/cl_parse.o \
	$(BUILDDIR)/svga/cl_tent.o \
	$(BUILDDIR)/svga/chase.o \
	$(BUILDDIR)/svga/cmd.o \
	$(BUILDDIR)/svga/common.o \
	$(BUILDDIR)/svga/console.o \
	$(BUILDDIR)/svga/crc.o \
	$(BUILDDIR)/svga/cvar.o \
	$(BUILDDIR)/svga/draw.o \
	$(BUILDDIR)/svga/d_edge.o \
	$(BUILDDIR)/svga/d_fill.o \
	$(BUILDDIR)/svga/d_init.o \
	$(BUILDDIR)/svga/d_modech.o \
	$(BUILDDIR)/svga/d_part.o \
	$(BUILDDIR)/svga/d_polyse.o \
	$(BUILDDIR)/svga/d_scan.o \
	$(BUILDDIR)/svga/d_sky.o \
	$(BUILDDIR)/svga/d_sprite.o \
	$(BUILDDIR)/svga/d_surf.o \
	$(BUILDDIR)/svga/d_vars.o \
	$(BUILDDIR)/svga/d_zpoint.o \
	$(BUILDDIR)/svga/host.o \
	$(BUILDDIR)/svga/host_cmd.o \
	$(BUILDDIR)/svga/keys.o \
	$(BUILDDIR)/svga/menu.o \
	$(BUILDDIR)/svga/mathlib.o \
	$(BUILDDIR)/svga/model.o \
	$(BUILDDIR)/svga/net_dgrm.o \
	$(BUILDDIR)/svga/net_loop.o \
	$(BUILDDIR)/svga/net_main.o \
	$(BUILDDIR)/svga/net_vcr.o \
	$(BUILDDIR)/svga/net_udp.o \
	$(BUILDDIR)/svga/net_bsd.o \
	$(BUILDDIR)/svga/nonintel.o \
	$(BUILDDIR)/svga/pr_cmds.o \
	$(BUILDDIR)/svga/pr_edict.o \
	$(BUILDDIR)/svga/pr_exec.o \
	$(BUILDDIR)/svga/r_aclip.o \
	$(BUILDDIR)/svga/r_alias.o \
	$(BUILDDIR)/svga/r_bsp.o \
	$(BUILDDIR)/svga/r_light.o \
	$(BUILDDIR)/svga/r_draw.o \
	$(BUILDDIR)/svga/r_efrag.o \
	$(BUILDDIR)/svga/r_edge.o \
	$(BUILDDIR)/svga/r_misc.o \
	$(BUILDDIR)/svga/r_main.o \
	$(BUILDDIR)/svga/r_sky.o \
	$(BUILDDIR)/svga/r_sprite.o \
	$(BUILDDIR)/svga/r_surf.o \
	$(BUILDDIR)/svga/r_part.o \
	$(BUILDDIR)/svga/r_vars.o \
	$(BUILDDIR)/svga/screen.o \
	$(BUILDDIR)/svga/sbar.o \
	$(BUILDDIR)/svga/sv_main.o \
	$(BUILDDIR)/svga/sv_phys.o \
	$(BUILDDIR)/svga/sv_move.o \
	$(BUILDDIR)/svga/sv_user.o \
	$(BUILDDIR)/svga/zone.o	\
	$(BUILDDIR)/svga/view.o	\
	$(BUILDDIR)/svga/wad.o \
	$(BUILDDIR)/svga/world.o \
	$(BUILDDIR)/svga/cd_linux.o \
	$(BUILDDIR)/svga/sys_linux.o \
	$(BUILDDIR)/svga/vid_svgalib.o \
	$(BUILDDIR)/svga/snd_dma.o \
	$(BUILDDIR)/svga/snd_mem.o \
	$(BUILDDIR)/svga/snd_mix.o \
	$(BUILDDIR)/svga/snd_linux.o \
	\
	$(BUILDDIR)/svga/d_copy.o \
	$(BUILDDIR)/svga/d_draw.o \
	$(BUILDDIR)/svga/d_draw16.o \
	$(BUILDDIR)/svga/d_parta.o \
	$(BUILDDIR)/svga/d_polysa.o \
	$(BUILDDIR)/svga/d_scana.o \
	$(BUILDDIR)/svga/d_spr8.o \
	$(BUILDDIR)/svga/d_varsa.o \
	$(BUILDDIR)/svga/math.o \
	$(BUILDDIR)/svga/r_aliasa.o \
	$(BUILDDIR)/svga/r_drawa.o \
	$(BUILDDIR)/svga/r_edgea.o \
	$(BUILDDIR)/svga/r_varsa.o \
	$(BUILDDIR)/svga/surf16.o \
	$(BUILDDIR)/svga/surf8.o \
	$(BUILDDIR)/svga/worlda.o \
	$(BUILDDIR)/svga/r_aclipa.o \
	$(BUILDDIR)/svga/snd_mixa.o \
	$(BUILDDIR)/svga/sys_dosa.o

$(BUILDDIR)/bin/quake-svga : $(SQUAKE_OBJS)
	$(CC) $(CFLAGS) -o $@ $(SQUAKE_OBJS) $(SVGA_LDFLAGS) $(LDFLAGS)

####

$(BUILDDIR)/svga/cl_demo.o :  $(PROJECT_DIR)/standalone/cl_demo.c
	$(DO_CC)

$(BUILDDIR)/svga/cl_input.o : $(PROJECT_DIR)/standalone/cl_input.c
	$(DO_CC)

$(BUILDDIR)/svga/cl_main.o :  $(PROJECT_DIR)/standalone/cl_main.c
	$(DO_CC)

$(BUILDDIR)/svga/cl_parse.o : $(PROJECT_DIR)/standalone/cl_parse.c
	$(DO_CC)

$(BUILDDIR)/svga/cl_tent.o :  $(PROJECT_DIR)/standalone/cl_tent.c
	$(DO_CC)

$(BUILDDIR)/svga/chase.o :    $(PROJECT_DIR)/standalone/chase.c
	$(DO_CC)

$(BUILDDIR)/svga/cmd.o :      $(PROJECT_DIR)/standalone/cmd.c
	$(DO_CC)

$(BUILDDIR)/svga/common.o :   $(PROJECT_DIR)/standalone/common.c
	$(DO_DEBUG_CC)

$(BUILDDIR)/svga/console.o :  $(PROJECT_DIR)/standalone/console.c
	$(DO_CC)

$(BUILDDIR)/svga/crc.o :      $(PROJECT_DIR)/standalone/crc.c
	$(DO_CC)

$(BUILDDIR)/svga/cvar.o :     $(PROJECT_DIR)/standalone/cvar.c
	$(DO_CC)

$(BUILDDIR)/svga/draw.o :     $(PROJECT_DIR)/standalone/draw.c
	$(DO_CC)

$(BUILDDIR)/svga/d_edge.o :   $(PROJECT_DIR)/standalone/d_edge.c
	$(DO_CC)

$(BUILDDIR)/svga/d_fill.o :   $(PROJECT_DIR)/standalone/d_fill.c
	$(DO_CC)

$(BUILDDIR)/svga/d_init.o :   $(PROJECT_DIR)/standalone/d_init.c
	$(DO_CC)

$(BUILDDIR)/svga/d_modech.o : $(PROJECT_DIR)/standalone/d_modech.c
	$(DO_CC)

$(BUILDDIR)/svga/d_part.o :   $(PROJECT_DIR)/standalone/d_part.c
	$(DO_CC)

$(BUILDDIR)/svga/d_polyse.o : $(PROJECT_DIR)/standalone/d_polyse.c
	$(DO_CC)

$(BUILDDIR)/svga/d_scan.o :   $(PROJECT_DIR)/standalone/d_scan.c
	$(DO_CC)

$(BUILDDIR)/svga/d_sky.o :    $(PROJECT_DIR)/standalone/d_sky.c
	$(DO_CC)

$(BUILDDIR)/svga/d_sprite.o : $(PROJECT_DIR)/standalone/d_sprite.c
	$(DO_CC)

$(BUILDDIR)/svga/d_surf.o :   $(PROJECT_DIR)/standalone/d_surf.c
	$(DO_CC)

$(BUILDDIR)/svga/d_vars.o :   $(PROJECT_DIR)/standalone/d_vars.c
	$(DO_CC)

$(BUILDDIR)/svga/d_zpoint.o : $(PROJECT_DIR)/standalone/d_zpoint.c
	$(DO_CC)

$(BUILDDIR)/svga/host.o :     $(PROJECT_DIR)/standalone/host.c
	$(DO_CC)

$(BUILDDIR)/svga/host_cmd.o : $(PROJECT_DIR)/standalone/host_cmd.c
	$(DO_CC)

$(BUILDDIR)/svga/keys.o :     $(PROJECT_DIR)/standalone/keys.c
	$(DO_CC)

$(BUILDDIR)/svga/menu.o :     $(PROJECT_DIR)/standalone/menu.c
	$(DO_CC)

$(BUILDDIR)/svga/mathlib.o :  $(PROJECT_DIR)/common/mathlib.c
	$(DO_CC)

$(BUILDDIR)/svga/model.o :    $(PROJECT_DIR)/standalone/model.c
	$(DO_CC)

$(BUILDDIR)/svga/net_dgrm.o : $(PROJECT_DIR)/standalone/net_dgrm.c
	$(DO_CC)

$(BUILDDIR)/svga/net_loop.o : $(PROJECT_DIR)/standalone/net_loop.c
	$(DO_CC)

$(BUILDDIR)/svga/net_main.o : $(PROJECT_DIR)/standalone/net_main.c
	$(DO_CC)

$(BUILDDIR)/svga/net_vcr.o :  $(PROJECT_DIR)/standalone/net_vcr.c
	$(DO_CC)

$(BUILDDIR)/svga/net_udp.o :  $(PROJECT_DIR)/standalone/net_udp.c
	$(DO_CC)

$(BUILDDIR)/svga/net_bsd.o :  $(PROJECT_DIR)/standalone/net_bsd.c
	$(DO_CC)

$(BUILDDIR)/svga/nonintel.o : $(PROJECT_DIR)/standalone/nonintel.c
	$(DO_CC)

$(BUILDDIR)/svga/pr_cmds.o :  $(PROJECT_DIR)/standalone/pr_cmds.c
	$(DO_CC)

$(BUILDDIR)/svga/pr_edict.o : $(PROJECT_DIR)/standalone/pr_edict.c
	$(DO_CC)

$(BUILDDIR)/svga/pr_exec.o :  $(PROJECT_DIR)/standalone/pr_exec.c
	$(DO_CC)

$(BUILDDIR)/svga/r_aclip.o :  $(PROJECT_DIR)/standalone/r_aclip.c
	$(DO_CC)

$(BUILDDIR)/svga/r_alias.o :  $(PROJECT_DIR)/standalone/r_alias.c
	$(DO_CC)

$(BUILDDIR)/svga/r_bsp.o :    $(PROJECT_DIR)/standalone/r_bsp.c
	$(DO_CC)

$(BUILDDIR)/svga/r_light.o :  $(PROJECT_DIR)/standalone/r_light.c
	$(DO_CC)

$(BUILDDIR)/svga/r_draw.o :   $(PROJECT_DIR)/standalone/r_draw.c
	$(DO_CC)

$(BUILDDIR)/svga/r_efrag.o :  $(PROJECT_DIR)/standalone/r_efrag.c
	$(DO_CC)

$(BUILDDIR)/svga/r_edge.o :   $(PROJECT_DIR)/standalone/r_edge.c
	$(DO_CC)

$(BUILDDIR)/svga/r_misc.o :   $(PROJECT_DIR)/standalone/r_misc.c
	$(DO_CC)

$(BUILDDIR)/svga/r_main.o :   $(PROJECT_DIR)/standalone/r_main.c
	$(DO_CC)

$(BUILDDIR)/svga/r_sky.o :    $(PROJECT_DIR)/standalone/r_sky.c
	$(DO_CC)

$(BUILDDIR)/svga/r_sprite.o : $(PROJECT_DIR)/standalone/r_sprite.c
	$(DO_CC)

$(BUILDDIR)/svga/r_surf.o :   $(PROJECT_DIR)/standalone/r_surf.c
	$(DO_CC)

$(BUILDDIR)/svga/r_part.o :   $(PROJECT_DIR)/standalone/r_part.c
	$(DO_CC)

$(BUILDDIR)/svga/r_vars.o :   $(PROJECT_DIR)/standalone/r_vars.c
	$(DO_CC)

$(BUILDDIR)/svga/screen.o :   $(PROJECT_DIR)/standalone/screen.c
	$(DO_CC)

$(BUILDDIR)/svga/sbar.o :     $(PROJECT_DIR)/standalone/sbar.c
	$(DO_CC)

$(BUILDDIR)/svga/sv_main.o :  $(PROJECT_DIR)/standalone/sv_main.c
	$(DO_CC)

$(BUILDDIR)/svga/sv_phys.o :  $(PROJECT_DIR)/standalone/sv_phys.c
	$(DO_CC)

$(BUILDDIR)/svga/sv_move.o :  $(PROJECT_DIR)/standalone/sv_move.c
	$(DO_CC)

$(BUILDDIR)/svga/sv_user.o :  $(PROJECT_DIR)/standalone/sv_user.c
	$(DO_CC)

$(BUILDDIR)/svga/zone.o :   $(PROJECT_DIR)/common/zone.c
	$(DO_CC)

$(BUILDDIR)/svga/view.o :   $(PROJECT_DIR)/standalone/view.c
	$(DO_CC)

$(BUILDDIR)/svga/wad.o :      $(PROJECT_DIR)/common/wad.c
	$(DO_CC)

$(BUILDDIR)/svga/world.o :    $(PROJECT_DIR)/standalone/world.c
	$(DO_CC)

$(BUILDDIR)/svga/cd_linux.o : $(PROJECT_DIR)/common/cd_linux.c
	$(DO_CC)

$(BUILDDIR)/svga/sys_linux.o :$(PROJECT_DIR)/standalone/sys_linux.c
	$(DO_CC)

$(BUILDDIR)/svga/vid_svgalib.o:$(PROJECT_DIR)/common/vid_svgalib.c
	$(DO_O_CC)

$(BUILDDIR)/svga/snd_dma.o :  $(PROJECT_DIR)/common/snd_dma.c
	$(DO_CC)

$(BUILDDIR)/svga/snd_mem.o :  $(PROJECT_DIR)/common/snd_mem.c
	$(DO_CC)

$(BUILDDIR)/svga/snd_mix.o :  $(PROJECT_DIR)/common/snd_mix.c
	$(DO_CC)

$(BUILDDIR)/svga/snd_linux.o :$(PROJECT_DIR)/common/snd_linux.c
	$(DO_CC)

#####

$(BUILDDIR)/svga/d_copy.o :   $(PROJECT_DIR)/standalone/d_copy.s
	$(DO_AS)

$(BUILDDIR)/svga/d_draw.o :   $(PROJECT_DIR)/standalone/d_draw.s
	$(DO_AS)

$(BUILDDIR)/svga/d_draw16.o : $(PROJECT_DIR)/standalone/d_draw16.s
	$(DO_AS)

$(BUILDDIR)/svga/d_parta.o :  $(PROJECT_DIR)/standalone/d_parta.s
	$(DO_AS)

$(BUILDDIR)/svga/d_polysa.o : $(PROJECT_DIR)/standalone/d_polysa.s
	$(DO_AS)

$(BUILDDIR)/svga/d_scana.o :  $(PROJECT_DIR)/standalone/d_scana.s
	$(DO_AS)

$(BUILDDIR)/svga/d_spr8.o :   $(PROJECT_DIR)/standalone/d_spr8.s
	$(DO_AS)

$(BUILDDIR)/svga/d_varsa.o :  $(PROJECT_DIR)/standalone/d_varsa.s
	$(DO_AS)

$(BUILDDIR)/svga/math.o :     $(PROJECT_DIR)/common/math.s
	$(DO_AS)

$(BUILDDIR)/svga/r_aliasa.o : $(PROJECT_DIR)/standalone/r_aliasa.s
	$(DO_AS)

$(BUILDDIR)/svga/r_drawa.o :  $(PROJECT_DIR)/standalone/r_drawa.s
	$(DO_AS)

$(BUILDDIR)/svga/r_edgea.o :  $(PROJECT_DIR)/standalone/r_edgea.s
	$(DO_AS)

$(BUILDDIR)/svga/r_varsa.o :  $(PROJECT_DIR)/standalone/r_varsa.s
	$(DO_AS)

$(BUILDDIR)/svga/surf16.o :   $(PROJECT_DIR)/standalone/surf16.s
	$(DO_AS)

$(BUILDDIR)/svga/surf8.o :    $(PROJECT_DIR)/standalone/surf8.s
	$(DO_AS)

$(BUILDDIR)/svga/worlda.o :   $(PROJECT_DIR)/standalone/worlda.s
	$(DO_AS)

$(BUILDDIR)/svga/r_aclipa.o : $(PROJECT_DIR)/standalone/r_aclipa.s
	$(DO_AS)

$(BUILDDIR)/svga/snd_mixa.o : $(PROJECT_DIR)/common/snd_mixa.s
	$(DO_AS)

$(BUILDDIR)/svga/sys_dosa.o : $(PROJECT_DIR)/standalone/sys_dosa.s
	$(DO_AS)

#############################################################################
# X11 Quake
#############################################################################

X11_OBJS = \
	$(BUILDDIR)/x11/cl_demo.o \
	$(BUILDDIR)/x11/cl_input.o \
	$(BUILDDIR)/x11/cl_main.o \
	$(BUILDDIR)/x11/cl_parse.o \
	$(BUILDDIR)/x11/cl_tent.o \
	$(BUILDDIR)/x11/chase.o \
	$(BUILDDIR)/x11/cmd.o \
	$(BUILDDIR)/x11/common.o \
	$(BUILDDIR)/x11/console.o \
	$(BUILDDIR)/x11/crc.o \
	$(BUILDDIR)/x11/cvar.o \
	$(BUILDDIR)/x11/draw.o \
	$(BUILDDIR)/x11/d_edge.o \
	$(BUILDDIR)/x11/d_fill.o \
	$(BUILDDIR)/x11/d_init.o \
	$(BUILDDIR)/x11/d_modech.o \
	$(BUILDDIR)/x11/d_part.o \
	$(BUILDDIR)/x11/d_polyse.o \
	$(BUILDDIR)/x11/d_scan.o \
	$(BUILDDIR)/x11/d_sky.o \
	$(BUILDDIR)/x11/d_sprite.o \
	$(BUILDDIR)/x11/d_surf.o \
	$(BUILDDIR)/x11/d_vars.o \
	$(BUILDDIR)/x11/d_zpoint.o \
	$(BUILDDIR)/x11/host.o \
	$(BUILDDIR)/x11/host_cmd.o \
	$(BUILDDIR)/x11/keys.o \
	$(BUILDDIR)/x11/menu.o \
	$(BUILDDIR)/x11/mathlib.o \
	$(BUILDDIR)/x11/model.o \
	$(BUILDDIR)/x11/net_dgrm.o \
	$(BUILDDIR)/x11/net_loop.o \
	$(BUILDDIR)/x11/net_main.o \
	$(BUILDDIR)/x11/net_vcr.o \
	$(BUILDDIR)/x11/net_udp.o \
	$(BUILDDIR)/x11/net_bsd.o \
	$(BUILDDIR)/x11/nonintel.o \
	$(BUILDDIR)/x11/pr_cmds.o \
	$(BUILDDIR)/x11/pr_edict.o \
	$(BUILDDIR)/x11/pr_exec.o \
	$(BUILDDIR)/x11/r_aclip.o \
	$(BUILDDIR)/x11/r_alias.o \
	$(BUILDDIR)/x11/r_bsp.o \
	$(BUILDDIR)/x11/r_light.o \
	$(BUILDDIR)/x11/r_draw.o \
	$(BUILDDIR)/x11/r_efrag.o \
	$(BUILDDIR)/x11/r_edge.o \
	$(BUILDDIR)/x11/r_misc.o \
	$(BUILDDIR)/x11/r_main.o \
	$(BUILDDIR)/x11/r_sky.o \
	$(BUILDDIR)/x11/r_sprite.o \
	$(BUILDDIR)/x11/r_surf.o \
	$(BUILDDIR)/x11/r_part.o \
	$(BUILDDIR)/x11/r_vars.o \
	$(BUILDDIR)/x11/screen.o \
	$(BUILDDIR)/x11/sbar.o \
	$(BUILDDIR)/x11/sv_main.o \
	$(BUILDDIR)/x11/sv_phys.o \
	$(BUILDDIR)/x11/sv_move.o \
	$(BUILDDIR)/x11/sv_user.o \
	$(BUILDDIR)/x11/zone.o	\
	$(BUILDDIR)/x11/view.o	\
	$(BUILDDIR)/x11/wad.o \
	$(BUILDDIR)/x11/world.o \
	$(BUILDDIR)/x11/cd_linux.o \
	$(BUILDDIR)/x11/sys_linux.o \
	$(BUILDDIR)/x11/vid_x.o \
	$(BUILDDIR)/x11/snd_dma.o \
	$(BUILDDIR)/x11/snd_mem.o \
	$(BUILDDIR)/x11/snd_mix.o \
	$(BUILDDIR)/x11/snd_linux.o \
	\
	$(BUILDDIR)/x11/d_draw.o \
	$(BUILDDIR)/x11/d_draw16.o \
	$(BUILDDIR)/x11/d_parta.o \
	$(BUILDDIR)/x11/d_polysa.o \
	$(BUILDDIR)/x11/d_scana.o \
	$(BUILDDIR)/x11/d_spr8.o \
	$(BUILDDIR)/x11/d_varsa.o \
	$(BUILDDIR)/x11/math.o \
	$(BUILDDIR)/x11/r_aliasa.o \
	$(BUILDDIR)/x11/r_drawa.o \
	$(BUILDDIR)/x11/r_edgea.o \
	$(BUILDDIR)/x11/r_varsa.o \
	$(BUILDDIR)/x11/surf16.o \
	$(BUILDDIR)/x11/surf8.o \
	$(BUILDDIR)/x11/worlda.o \
	$(BUILDDIR)/x11/r_aclipa.o \
	$(BUILDDIR)/x11/snd_mixa.o \
	$(BUILDDIR)/x11/sys_dosa.o

$(BUILDDIR)/bin/quake-x11 : $(X11_OBJS)
	$(CC) $(CFLAGS) -o $@ $(X11_OBJS) $(X11_LDFLAGS) $(LDFLAGS)

####

$(BUILDDIR)/x11/cl_demo.o :  $(PROJECT_DIR)/standalone/cl_demo.c
	$(DO_X11_CC)

$(BUILDDIR)/x11/cl_input.o : $(PROJECT_DIR)/standalone/cl_input.c
	$(DO_X11_CC)

$(BUILDDIR)/x11/cl_main.o :  $(PROJECT_DIR)/standalone/cl_main.c
	$(DO_X11_CC)

$(BUILDDIR)/x11/cl_parse.o : $(PROJECT_DIR)/standalone/cl_parse.c
	$(DO_X11_CC)

$(BUILDDIR)/x11/cl_tent.o :  $(PROJECT_DIR)/standalone/cl_tent.c
	$(DO_X11_CC)

$(BUILDDIR)/x11/chase.o :    $(PROJECT_DIR)/standalone/chase.c
	$(DO_X11_CC)

$(BUILDDIR)/x11/cmd.o :      $(PROJECT_DIR)/standalone/cmd.c
	$(DO_X11_CC)

$(BUILDDIR)/x11/common.o :   $(PROJECT_DIR)/standalone/common.c
	$(DO_X11_DEBUG_CC)

$(BUILDDIR)/x11/console.o :  $(PROJECT_DIR)/standalone/console.c
	$(DO_X11_CC)

$(BUILDDIR)/x11/crc.o :      $(PROJECT_DIR)/standalone/crc.c
	$(DO_X11_CC)

$(BUILDDIR)/x11/cvar.o :     $(PROJECT_DIR)/standalone/cvar.c
	$(DO_X11_CC)

$(BUILDDIR)/x11/draw.o :     $(PROJECT_DIR)/standalone/draw.c
	$(DO_X11_CC)

$(BUILDDIR)/x11/d_edge.o :   $(PROJECT_DIR)/standalone/d_edge.c
	$(DO_X11_CC)

$(BUILDDIR)/x11/d_fill.o :   $(PROJECT_DIR)/standalone/d_fill.c
	$(DO_X11_CC)

$(BUILDDIR)/x11/d_init.o :   $(PROJECT_DIR)/standalone/d_init.c
	$(DO_X11_CC)

$(BUILDDIR)/x11/d_modech.o : $(PROJECT_DIR)/standalone/d_modech.c
	$(DO_X11_CC)

$(BUILDDIR)/x11/d_part.o :   $(PROJECT_DIR)/standalone/d_part.c
	$(DO_X11_CC)

$(BUILDDIR)/x11/d_polyse.o : $(PROJECT_DIR)/standalone/d_polyse.c
	$(DO_X11_CC)

$(BUILDDIR)/x11/d_scan.o :   $(PROJECT_DIR)/standalone/d_scan.c
	$(DO_X11_CC)

$(BUILDDIR)/x11/d_sky.o :    $(PROJECT_DIR)/standalone/d_sky.c
	$(DO_X11_CC)

$(BUILDDIR)/x11/d_sprite.o : $(PROJECT_DIR)/standalone/d_sprite.c
	$(DO_X11_CC)

$(BUILDDIR)/x11/d_surf.o :   $(PROJECT_DIR)/standalone/d_surf.c
	$(DO_X11_CC)

$(BUILDDIR)/x11/d_vars.o :   $(PROJECT_DIR)/standalone/d_vars.c
	$(DO_X11_CC)

$(BUILDDIR)/x11/d_zpoint.o : $(PROJECT_DIR)/standalone/d_zpoint.c
	$(DO_X11_CC)

$(BUILDDIR)/x11/host.o :     $(PROJECT_DIR)/standalone/host.c
	$(DO_X11_CC)

$(BUILDDIR)/x11/host_cmd.o : $(PROJECT_DIR)/standalone/host_cmd.c
	$(DO_X11_CC)

$(BUILDDIR)/x11/keys.o :     $(PROJECT_DIR)/standalone/keys.c
	$(DO_X11_CC)

$(BUILDDIR)/x11/menu.o :     $(PROJECT_DIR)/standalone/menu.c
	$(DO_X11_CC)

$(BUILDDIR)/x11/mathlib.o :  $(PROJECT_DIR)/common/mathlib.c
	$(DO_X11_CC)

$(BUILDDIR)/x11/model.o :    $(PROJECT_DIR)/standalone/model.c
	$(DO_X11_CC)

$(BUILDDIR)/x11/net_dgrm.o : $(PROJECT_DIR)/standalone/net_dgrm.c
	$(DO_X11_CC)

$(BUILDDIR)/x11/net_loop.o : $(PROJECT_DIR)/standalone/net_loop.c
	$(DO_X11_CC)

$(BUILDDIR)/x11/net_main.o : $(PROJECT_DIR)/standalone/net_main.c
	$(DO_X11_CC)

$(BUILDDIR)/x11/net_vcr.o :  $(PROJECT_DIR)/standalone/net_vcr.c
	$(DO_X11_CC)

$(BUILDDIR)/x11/net_udp.o :  $(PROJECT_DIR)/standalone/net_udp.c
	$(DO_X11_CC)

$(BUILDDIR)/x11/net_bsd.o :  $(PROJECT_DIR)/standalone/net_bsd.c
	$(DO_X11_CC)

$(BUILDDIR)/x11/nonintel.o : $(PROJECT_DIR)/standalone/nonintel.c
	$(DO_X11_CC)

$(BUILDDIR)/x11/pr_cmds.o :  $(PROJECT_DIR)/standalone/pr_cmds.c
	$(DO_X11_CC)

$(BUILDDIR)/x11/pr_edict.o : $(PROJECT_DIR)/standalone/pr_edict.c
	$(DO_X11_CC)

$(BUILDDIR)/x11/pr_exec.o :  $(PROJECT_DIR)/standalone/pr_exec.c
	$(DO_X11_CC)

$(BUILDDIR)/x11/r_aclip.o :  $(PROJECT_DIR)/standalone/r_aclip.c
	$(DO_X11_CC)

$(BUILDDIR)/x11/r_alias.o :  $(PROJECT_DIR)/standalone/r_alias.c
	$(DO_X11_CC)

$(BUILDDIR)/x11/r_bsp.o :    $(PROJECT_DIR)/standalone/r_bsp.c
	$(DO_X11_CC)

$(BUILDDIR)/x11/r_light.o :  $(PROJECT_DIR)/standalone/r_light.c
	$(DO_X11_CC)

$(BUILDDIR)/x11/r_draw.o :   $(PROJECT_DIR)/standalone/r_draw.c
	$(DO_X11_CC)

$(BUILDDIR)/x11/r_efrag.o :  $(PROJECT_DIR)/standalone/r_efrag.c
	$(DO_X11_CC)

$(BUILDDIR)/x11/r_edge.o :   $(PROJECT_DIR)/standalone/r_edge.c
	$(DO_X11_CC)

$(BUILDDIR)/x11/r_misc.o :   $(PROJECT_DIR)/standalone/r_misc.c
	$(DO_X11_CC)

$(BUILDDIR)/x11/r_main.o :   $(PROJECT_DIR)/standalone/r_main.c
	$(DO_X11_CC)

$(BUILDDIR)/x11/r_sky.o :    $(PROJECT_DIR)/standalone/r_sky.c
	$(DO_X11_CC)

$(BUILDDIR)/x11/r_sprite.o : $(PROJECT_DIR)/standalone/r_sprite.c
	$(DO_X11_CC)

$(BUILDDIR)/x11/r_surf.o :   $(PROJECT_DIR)/standalone/r_surf.c
	$(DO_X11_CC)

$(BUILDDIR)/x11/r_part.o :   $(PROJECT_DIR)/standalone/r_part.c
	$(DO_X11_CC)

$(BUILDDIR)/x11/r_vars.o :   $(PROJECT_DIR)/standalone/r_vars.c
	$(DO_X11_CC)

$(BUILDDIR)/x11/screen.o :   $(PROJECT_DIR)/standalone/screen.c
	$(DO_X11_CC)

$(BUILDDIR)/x11/sbar.o :     $(PROJECT_DIR)/standalone/sbar.c
	$(DO_X11_CC)

$(BUILDDIR)/x11/sv_main.o :  $(PROJECT_DIR)/standalone/sv_main.c
	$(DO_X11_CC)

$(BUILDDIR)/x11/sv_phys.o :  $(PROJECT_DIR)/standalone/sv_phys.c
	$(DO_X11_CC)

$(BUILDDIR)/x11/sv_move.o :  $(PROJECT_DIR)/standalone/sv_move.c
	$(DO_X11_CC)

$(BUILDDIR)/x11/sv_user.o :  $(PROJECT_DIR)/standalone/sv_user.c
	$(DO_X11_CC)

$(BUILDDIR)/x11/zone.o :   $(PROJECT_DIR)/common/zone.c
	$(DO_X11_CC)

$(BUILDDIR)/x11/view.o :   $(PROJECT_DIR)/standalone/view.c
	$(DO_X11_CC)

$(BUILDDIR)/x11/wad.o :      $(PROJECT_DIR)/common/wad.c
	$(DO_X11_CC)

$(BUILDDIR)/x11/world.o :    $(PROJECT_DIR)/standalone/world.c
	$(DO_X11_CC)

$(BUILDDIR)/x11/cd_linux.o : $(PROJECT_DIR)/common/cd_linux.c
	$(DO_X11_CC)

$(BUILDDIR)/x11/sys_linux.o :$(PROJECT_DIR)/standalone/sys_linux.c
	$(DO_X11_CC)

$(BUILDDIR)/x11/vid_x.o: $(PROJECT_DIR)/common/vid_x.c
	$(DO_O_CC)

$(BUILDDIR)/x11/snd_dma.o :  $(PROJECT_DIR)/common/snd_dma.c
	$(DO_X11_CC)

$(BUILDDIR)/x11/snd_mem.o :  $(PROJECT_DIR)/common/snd_mem.c
	$(DO_X11_CC)

$(BUILDDIR)/x11/snd_mix.o :  $(PROJECT_DIR)/common/snd_mix.c
	$(DO_X11_CC)

$(BUILDDIR)/x11/snd_linux.o :$(PROJECT_DIR)/common/snd_linux.c
	$(DO_X11_CC)

#####

$(BUILDDIR)/x11/d_copy.o :   $(PROJECT_DIR)/standalone/d_copy.s
	$(DO_AS)

$(BUILDDIR)/x11/d_draw.o :   $(PROJECT_DIR)/standalone/d_draw.s
	$(DO_AS)

$(BUILDDIR)/x11/d_draw16.o : $(PROJECT_DIR)/standalone/d_draw16.s
	$(DO_AS)

$(BUILDDIR)/x11/d_parta.o :  $(PROJECT_DIR)/standalone/d_parta.s
	$(DO_AS)

$(BUILDDIR)/x11/d_polysa.o : $(PROJECT_DIR)/standalone/d_polysa.s
	$(DO_AS)

$(BUILDDIR)/x11/d_scana.o :  $(PROJECT_DIR)/standalone/d_scana.s
	$(DO_AS)

$(BUILDDIR)/x11/d_spr8.o :   $(PROJECT_DIR)/standalone/d_spr8.s
	$(DO_AS)

$(BUILDDIR)/x11/d_varsa.o :  $(PROJECT_DIR)/standalone/d_varsa.s
	$(DO_AS)

$(BUILDDIR)/x11/math.o :     $(PROJECT_DIR)/common/math.s
	$(DO_AS)

$(BUILDDIR)/x11/r_aliasa.o : $(PROJECT_DIR)/standalone/r_aliasa.s
	$(DO_AS)

$(BUILDDIR)/x11/r_drawa.o :  $(PROJECT_DIR)/standalone/r_drawa.s
	$(DO_AS)

$(BUILDDIR)/x11/r_edgea.o :  $(PROJECT_DIR)/standalone/r_edgea.s
	$(DO_AS)

$(BUILDDIR)/x11/r_varsa.o :  $(PROJECT_DIR)/standalone/r_varsa.s
	$(DO_AS)

$(BUILDDIR)/x11/surf16.o :   $(PROJECT_DIR)/standalone/surf16.s
	$(DO_AS)

$(BUILDDIR)/x11/surf8.o :    $(PROJECT_DIR)/standalone/surf8.s
	$(DO_AS)

$(BUILDDIR)/x11/worlda.o :   $(PROJECT_DIR)/standalone/worlda.s
	$(DO_AS)

$(BUILDDIR)/x11/r_aclipa.o : $(PROJECT_DIR)/standalone/r_aclipa.s
	$(DO_AS)

$(BUILDDIR)/x11/snd_mixa.o : $(PROJECT_DIR)/common/snd_mixa.s
	$(DO_AS)

$(BUILDDIR)/x11/sys_dosa.o : $(PROJECT_DIR)/standalone/sys_dosa.s
	$(DO_AS)

#############################################################################
# GLQuake
#############################################################################

GLQUAKE_OBJS= \
	$(BUILDDIR)/gl/cl_demo.o \
	$(BUILDDIR)/gl/cl_input.o \
	$(BUILDDIR)/gl/cl_main.o \
	$(BUILDDIR)/gl/cl_parse.o \
	$(BUILDDIR)/gl/cl_tent.o \
	$(BUILDDIR)/gl/chase.o \
	$(BUILDDIR)/gl/cmd.o \
	$(BUILDDIR)/gl/common.o \
	$(BUILDDIR)/gl/console.o \
	$(BUILDDIR)/gl/crc.o \
	$(BUILDDIR)/gl/cvar.o \
	\
	$(BUILDDIR)/gl/gl_draw.o \
	$(BUILDDIR)/gl/gl_mesh.o \
	$(BUILDDIR)/gl/gl_model.o \
	$(BUILDDIR)/gl/gl_refrag.o \
	$(BUILDDIR)/gl/gl_rlight.o \
	$(BUILDDIR)/gl/gl_rmain.o \
	$(BUILDDIR)/gl/gl_rmisc.o \
	$(BUILDDIR)/gl/gl_rsurf.o \
	$(BUILDDIR)/gl/gl_screen.o \
	$(BUILDDIR)/gl/gl_test.o \
	$(BUILDDIR)/gl/gl_warp.o \
	\
	$(BUILDDIR)/gl/host.o \
	$(BUILDDIR)/gl/host_cmd.o \
	$(BUILDDIR)/gl/keys.o \
	$(BUILDDIR)/gl/menu.o \
	$(BUILDDIR)/gl/mathlib.o \
	$(BUILDDIR)/gl/net_dgrm.o \
	$(BUILDDIR)/gl/net_loop.o \
	$(BUILDDIR)/gl/net_main.o \
	$(BUILDDIR)/gl/net_vcr.o \
	$(BUILDDIR)/gl/net_udp.o \
	$(BUILDDIR)/gl/net_bsd.o \
	$(BUILDDIR)/gl/pr_cmds.o \
	$(BUILDDIR)/gl/pr_edict.o \
	$(BUILDDIR)/gl/pr_exec.o \
	$(BUILDDIR)/gl/r_part.o \
	$(BUILDDIR)/gl/sbar.o \
	$(BUILDDIR)/gl/sv_main.o \
	$(BUILDDIR)/gl/sv_phys.o \
	$(BUILDDIR)/gl/sv_move.o \
	$(BUILDDIR)/gl/sv_user.o \
	$(BUILDDIR)/gl/zone.o	\
	$(BUILDDIR)/gl/view.o	\
	$(BUILDDIR)/gl/wad.o \
	$(BUILDDIR)/gl/world.o \
	$(BUILDDIR)/gl/cd_linux.o \
	$(BUILDDIR)/gl/sys_linux.o \
	$(BUILDDIR)/gl/snd_dma.o \
	$(BUILDDIR)/gl/snd_mem.o \
	$(BUILDDIR)/gl/snd_mix.o \
	$(BUILDDIR)/gl/snd_linux.o \
	\
	$(BUILDDIR)/gl/math.o \
	$(BUILDDIR)/gl/worlda.o \
	$(BUILDDIR)/gl/snd_mixa.o \
	$(BUILDDIR)/gl/sys_dosa.o

GLSVGA_OBJS=$(BUILDDIR)/gl/gl_vidlinux.o

GLX_OBJS=$(BUILDDIR)/gl/gl_vidlinuxglx.o

$(BUILDDIR)/bin/quake-gl : $(GLQUAKE_OBJS) $(GLX_OBJS)
	$(CC) $(CFLAGS) -o $@ $(GLQUAKE_OBJS) $(GLX_OBJS) $(GL_LDFLAGS) $(SVGA_LDFLAGS) $(LDFLAGS)

$(BUILDDIR)/bin/quake-glide : $(GLQUAKE_OBJS) $(GLSVGA_OBJS)
	$(CC) $(CFLAGS) -o $@ $(GLQUAKE_OBJS) $(GLSVGA_OBJS) $(GLIDE_LDFLAGS) $(LDFLAGS)

$(BUILDDIR)/gl/cl_demo.o :      $(PROJECT_DIR)/standalone/cl_demo.c
	$(DO_GL_CC)

$(BUILDDIR)/gl/cl_input.o :     $(PROJECT_DIR)/standalone/cl_input.c
	$(DO_GL_CC)

$(BUILDDIR)/gl/cl_main.o :      $(PROJECT_DIR)/standalone/cl_main.c
	$(DO_GL_CC)

$(BUILDDIR)/gl/cl_parse.o :     $(PROJECT_DIR)/standalone/cl_parse.c
	$(DO_GL_CC)

$(BUILDDIR)/gl/cl_tent.o :      $(PROJECT_DIR)/standalone/cl_tent.c
	$(DO_GL_CC)

$(BUILDDIR)/gl/chase.o :        $(PROJECT_DIR)/standalone/chase.c
	$(DO_GL_CC)

$(BUILDDIR)/gl/cmd.o :          $(PROJECT_DIR)/standalone/cmd.c
	$(DO_GL_CC)

$(BUILDDIR)/gl/common.o :       $(PROJECT_DIR)/standalone/common.c
	$(DO_GL_DEBUG_CC)

$(BUILDDIR)/gl/console.o :      $(PROJECT_DIR)/standalone/console.c
	$(DO_GL_CC)

$(BUILDDIR)/gl/crc.o :          $(PROJECT_DIR)/standalone/crc.c
	$(DO_GL_CC)

$(BUILDDIR)/gl/cvar.o :         $(PROJECT_DIR)/standalone/cvar.c
	$(DO_GL_CC)

$(BUILDDIR)/gl/gl_draw.o :      $(PROJECT_DIR)/standalone/gl_draw.c
	$(DO_GL_CC)

$(BUILDDIR)/gl/gl_mesh.o :      $(PROJECT_DIR)/standalone/gl_mesh.c
	$(DO_GL_CC)

$(BUILDDIR)/gl/gl_model.o :     $(PROJECT_DIR)/standalone/gl_model.c
	$(DO_GL_CC)

$(BUILDDIR)/gl/gl_refrag.o :    $(PROJECT_DIR)/standalone/gl_refrag.c
	$(DO_GL_CC)

$(BUILDDIR)/gl/gl_rlight.o :    $(PROJECT_DIR)/standalone/gl_rlight.c
	$(DO_GL_CC)

$(BUILDDIR)/gl/gl_rmain.o :     $(PROJECT_DIR)/standalone/gl_rmain.c
	$(DO_GL_CC)

$(BUILDDIR)/gl/gl_rmisc.o :     $(PROJECT_DIR)/standalone/gl_rmisc.c
	$(DO_GL_CC)

$(BUILDDIR)/gl/gl_rsurf.o :     $(PROJECT_DIR)/standalone/gl_rsurf.c
	$(DO_GL_CC)

$(BUILDDIR)/gl/gl_screen.o :    $(PROJECT_DIR)/standalone/gl_screen.c
	$(DO_GL_CC)

$(BUILDDIR)/gl/gl_test.o :      $(PROJECT_DIR)/standalone/gl_test.c
	$(DO_GL_CC)

$(BUILDDIR)/gl/gl_vidlinux.o :  $(PROJECT_DIR)/standalone/gl_vidlinux.c
	$(DO_GL_CC)

$(BUILDDIR)/gl/gl_vidlinuxglx.o :  $(PROJECT_DIR)/common/gl_vidlinuxglx.c
	$(DO_GL_CC)

$(BUILDDIR)/gl/gl_warp.o :      $(PROJECT_DIR)/standalone/gl_warp.c
	$(DO_GL_CC)

$(BUILDDIR)/gl/host.o :         $(PROJECT_DIR)/standalone/host.c
	$(DO_GL_CC)

$(BUILDDIR)/gl/host_cmd.o :     $(PROJECT_DIR)/standalone/host_cmd.c
	$(DO_GL_CC)

$(BUILDDIR)/gl/keys.o :         $(PROJECT_DIR)/standalone/keys.c
	$(DO_GL_CC)

$(BUILDDIR)/gl/menu.o :         $(PROJECT_DIR)/standalone/menu.c
	$(DO_GL_CC)

$(BUILDDIR)/gl/mathlib.o :      $(PROJECT_DIR)/common/mathlib.c
	$(DO_GL_CC)

$(BUILDDIR)/gl/net_dgrm.o :     $(PROJECT_DIR)/standalone/net_dgrm.c
	$(DO_GL_CC)

$(BUILDDIR)/gl/net_loop.o :     $(PROJECT_DIR)/standalone/net_loop.c
	$(DO_GL_CC)

$(BUILDDIR)/gl/net_main.o :     $(PROJECT_DIR)/standalone/net_main.c
	$(DO_GL_CC)

$(BUILDDIR)/gl/net_vcr.o :      $(PROJECT_DIR)/standalone/net_vcr.c
	$(DO_GL_CC)

$(BUILDDIR)/gl/net_udp.o :      $(PROJECT_DIR)/standalone/net_udp.c
	$(DO_GL_CC)

$(BUILDDIR)/gl/net_bsd.o :      $(PROJECT_DIR)/standalone/net_bsd.c
	$(DO_GL_CC)

$(BUILDDIR)/gl/pr_cmds.o :      $(PROJECT_DIR)/standalone/pr_cmds.c
	$(DO_GL_CC)

$(BUILDDIR)/gl/pr_edict.o :     $(PROJECT_DIR)/standalone/pr_edict.c
	$(DO_GL_CC)

$(BUILDDIR)/gl/pr_exec.o :      $(PROJECT_DIR)/standalone/pr_exec.c
	$(DO_GL_CC)

$(BUILDDIR)/gl/r_part.o :       $(PROJECT_DIR)/standalone/r_part.c
	$(DO_GL_CC)

$(BUILDDIR)/gl/sbar.o :         $(PROJECT_DIR)/standalone/sbar.c
	$(DO_GL_CC)

$(BUILDDIR)/gl/sv_main.o :      $(PROJECT_DIR)/standalone/sv_main.c
	$(DO_GL_CC)

$(BUILDDIR)/gl/sv_phys.o :      $(PROJECT_DIR)/standalone/sv_phys.c
	$(DO_GL_CC)

$(BUILDDIR)/gl/sv_move.o :      $(PROJECT_DIR)/standalone/sv_move.c
	$(DO_GL_CC)

$(BUILDDIR)/gl/sv_user.o :      $(PROJECT_DIR)/standalone/sv_user.c
	$(DO_GL_CC)

$(BUILDDIR)/gl/zone.o :        $(PROJECT_DIR)/common/zone.c
	$(DO_GL_CC)

$(BUILDDIR)/gl/view.o :        $(PROJECT_DIR)/standalone/view.c
	$(DO_GL_CC)

$(BUILDDIR)/gl/wad.o :          $(PROJECT_DIR)/common/wad.c
	$(DO_GL_CC)

$(BUILDDIR)/gl/world.o :        $(PROJECT_DIR)/standalone/world.c
	$(DO_GL_CC)

$(BUILDDIR)/gl/cd_linux.o :     $(PROJECT_DIR)/common/cd_linux.c
	$(DO_GL_CC)

$(BUILDDIR)/gl/sys_linux.o :    $(PROJECT_DIR)/standalone/sys_linux.c
	$(DO_GL_CC)

$(BUILDDIR)/gl/snd_dma.o :      $(PROJECT_DIR)/common/snd_dma.c
	$(DO_GL_CC)

$(BUILDDIR)/gl/snd_mem.o :      $(PROJECT_DIR)/common/snd_mem.c
	$(DO_GL_CC)

$(BUILDDIR)/gl/snd_mix.o :      $(PROJECT_DIR)/common/snd_mix.c
	$(DO_GL_CC)

$(BUILDDIR)/gl/snd_linux.o :    $(PROJECT_DIR)/common/snd_linux.c
	$(DO_GL_CC)

$(BUILDDIR)/gl/math.o :         $(PROJECT_DIR)/common/math.s
	$(DO_GL_AS)

$(BUILDDIR)/gl/worlda.o :       $(PROJECT_DIR)/standalone/worlda.s
	$(DO_GL_AS)

$(BUILDDIR)/gl/snd_mixa.o :     $(PROJECT_DIR)/common/snd_mixa.s
	$(DO_GL_AS)

$(BUILDDIR)/gl/sys_dosa.o :     $(PROJECT_DIR)/standalone/sys_dosa.s
	$(DO_GL_AS)

#############################################################################
# MISC
#############################################################################

clean: clean-debug clean-release
	rm -f squake.spec glquake.spec quake.x11.spec

clean-debug:
	$(MAKE) clean2 BUILDDIR=$(BUILD_DEBUG_DIR) CFLAGS="$(DEBUG_CFLAGS)"

clean-release:
	$(MAKE) clean2 BUILDDIR=$(BUILD_RELEASE_DIR) CFLAGS="$(DEBUG_CFLAGS)"

clean2:
	-rm -f $(SQUAKE_OBJS) $(X11_OBJS) $(GLQUAKE_OBJS) $(GLSVGA_OBJS) \
		$(GLX_OBJS)
