#
# Quake Makefile for Linux 2.0
#
# Aug '98 by Zoid <zoid@idsoftware.com>
#
# ELF only
#

BASEVERSION=1.09
VERSION=$(BASEVERSION)$(GLIBC)

ifneq (,$(findstring libc6,$(shell if [ -e /lib/libc.so.6 ];then echo libc6;fi)))
GLIBC=-glibc
else
GLIBC=
endif

ifneq (,$(findstring alpha,$(shell uname -m)))
ARCH=axp
else
ARCH=i386
endif
NOARCH=noarch

ROOT_DIR=/home/deek/Source/Quake/WinQuake
BUILD_DEBUG_DIR=debug-$(ARCH)$(GLIBC)
BUILD_RELEASE_DIR=release-$(ARCH)$(GLIBC)

X11_DIR=/usr/X11R6
GL_DIR=/usr/src/mesa-3.0

CC=gcc

BASE_CFLAGS=-Dstricmp=strcasecmp
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
GLIDE_LDFLAGS=-L$(X11_DIR)/lib -L$(MESA_DIR)/lib -L/usr/local/lib -lGL -lX11 -lXext -ldl -lglide2x -lXxf86dga -lXxf86vm -lm

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

TARGETS=$(BUILDDIR)/bin/squake \
	$(BUILDDIR)/bin/xquake \
	$(BUILDDIR)/bin/glquake \
	$(BUILDDIR)/bin/glquake.3dfx
	# $(BUILDDIR)/bin/unixded

build_debug:
	@-mkdir $(BUILD_DEBUG_DIR) \
		$(BUILD_DEBUG_DIR)/bin \
		$(BUILD_DEBUG_DIR)/glquake \
		$(BUILD_DEBUG_DIR)/glquake.3dfx \
		$(BUILD_DEBUG_DIR)/squake \
		$(BUILD_DEBUG_DIR)/unixded \
		$(BUILD_DEBUG_DIR)/x11
	$(MAKE) targets BUILDDIR=$(BUILD_DEBUG_DIR) CFLAGS="$(DEBUG_CFLAGS)"

build_release:
	@-mkdir $(BUILD_RELEASE_DIR) \
		$(BUILD_RELEASE_DIR)/bin \
		$(BUILD_RELEASE_DIR)/glquake \
		$(BUILD_RELEASE_DIR)/glquake.3dfx \
		$(BUILD_RELEASE_DIR)/squake \
		$(BUILD_RELEASE_DIR)/unixded \
		$(BUILD_RELEASE_DIR)/x11
	$(MAKE) targets BUILDDIR=$(BUILD_RELEASE_DIR) CFLAGS="$(RELEASE_CFLAGS)"

all: build_debug build_release

targets: $(TARGETS)

#############################################################################
# SVGALIB Quake
#############################################################################

SQUAKE_OBJS = \
	$(BUILDDIR)/squake/cl_demo.o \
	$(BUILDDIR)/squake/cl_input.o \
	$(BUILDDIR)/squake/cl_main.o \
	$(BUILDDIR)/squake/cl_parse.o \
	$(BUILDDIR)/squake/cl_tent.o \
	$(BUILDDIR)/squake/chase.o \
	$(BUILDDIR)/squake/cmd.o \
	$(BUILDDIR)/squake/common.o \
	$(BUILDDIR)/squake/console.o \
	$(BUILDDIR)/squake/crc.o \
	$(BUILDDIR)/squake/cvar.o \
	$(BUILDDIR)/squake/draw.o \
	$(BUILDDIR)/squake/d_edge.o \
	$(BUILDDIR)/squake/d_fill.o \
	$(BUILDDIR)/squake/d_init.o \
	$(BUILDDIR)/squake/d_modech.o \
	$(BUILDDIR)/squake/d_part.o \
	$(BUILDDIR)/squake/d_polyse.o \
	$(BUILDDIR)/squake/d_scan.o \
	$(BUILDDIR)/squake/d_sky.o \
	$(BUILDDIR)/squake/d_sprite.o \
	$(BUILDDIR)/squake/d_surf.o \
	$(BUILDDIR)/squake/d_vars.o \
	$(BUILDDIR)/squake/d_zpoint.o \
	$(BUILDDIR)/squake/host.o \
	$(BUILDDIR)/squake/host_cmd.o \
	$(BUILDDIR)/squake/keys.o \
	$(BUILDDIR)/squake/menu.o \
	$(BUILDDIR)/squake/mathlib.o \
	$(BUILDDIR)/squake/model.o \
	$(BUILDDIR)/squake/net_dgrm.o \
	$(BUILDDIR)/squake/net_loop.o \
	$(BUILDDIR)/squake/net_main.o \
	$(BUILDDIR)/squake/net_vcr.o \
	$(BUILDDIR)/squake/net_udp.o \
	$(BUILDDIR)/squake/net_bsd.o \
	$(BUILDDIR)/squake/nonintel.o \
	$(BUILDDIR)/squake/pr_cmds.o \
	$(BUILDDIR)/squake/pr_edict.o \
	$(BUILDDIR)/squake/pr_exec.o \
	$(BUILDDIR)/squake/r_aclip.o \
	$(BUILDDIR)/squake/r_alias.o \
	$(BUILDDIR)/squake/r_bsp.o \
	$(BUILDDIR)/squake/r_light.o \
	$(BUILDDIR)/squake/r_draw.o \
	$(BUILDDIR)/squake/r_efrag.o \
	$(BUILDDIR)/squake/r_edge.o \
	$(BUILDDIR)/squake/r_misc.o \
	$(BUILDDIR)/squake/r_main.o \
	$(BUILDDIR)/squake/r_sky.o \
	$(BUILDDIR)/squake/r_sprite.o \
	$(BUILDDIR)/squake/r_surf.o \
	$(BUILDDIR)/squake/r_part.o \
	$(BUILDDIR)/squake/r_vars.o \
	$(BUILDDIR)/squake/screen.o \
	$(BUILDDIR)/squake/sbar.o \
	$(BUILDDIR)/squake/sv_main.o \
	$(BUILDDIR)/squake/sv_phys.o \
	$(BUILDDIR)/squake/sv_move.o \
	$(BUILDDIR)/squake/sv_user.o \
	$(BUILDDIR)/squake/zone.o	\
	$(BUILDDIR)/squake/view.o	\
	$(BUILDDIR)/squake/wad.o \
	$(BUILDDIR)/squake/world.o \
	$(BUILDDIR)/squake/cd_linux.o \
	$(BUILDDIR)/squake/sys_linux.o \
	$(BUILDDIR)/squake/vid_svgalib.o \
	$(BUILDDIR)/squake/snd_dma.o \
	$(BUILDDIR)/squake/snd_mem.o \
	$(BUILDDIR)/squake/snd_mix.o \
	$(BUILDDIR)/squake/snd_linux.o \
	\
	$(BUILDDIR)/squake/d_copy.o \
	$(BUILDDIR)/squake/d_draw.o \
	$(BUILDDIR)/squake/d_draw16.o \
	$(BUILDDIR)/squake/d_parta.o \
	$(BUILDDIR)/squake/d_polysa.o \
	$(BUILDDIR)/squake/d_scana.o \
	$(BUILDDIR)/squake/d_spr8.o \
	$(BUILDDIR)/squake/d_varsa.o \
	$(BUILDDIR)/squake/math.o \
	$(BUILDDIR)/squake/r_aliasa.o \
	$(BUILDDIR)/squake/r_drawa.o \
	$(BUILDDIR)/squake/r_edgea.o \
	$(BUILDDIR)/squake/r_varsa.o \
	$(BUILDDIR)/squake/surf16.o \
	$(BUILDDIR)/squake/surf8.o \
	$(BUILDDIR)/squake/worlda.o \
	$(BUILDDIR)/squake/r_aclipa.o \
	$(BUILDDIR)/squake/snd_mixa.o \
	$(BUILDDIR)/squake/sys_dosa.o

$(BUILDDIR)/bin/squake : $(SQUAKE_OBJS)
	$(CC) $(CFLAGS) -o $@ $(SQUAKE_OBJS) $(SVGA_LDFLAGS) $(LDFLAGS)

####

$(BUILDDIR)/squake/cl_demo.o :  $(ROOT_DIR)/cl_demo.c
	$(DO_CC)

$(BUILDDIR)/squake/cl_input.o : $(ROOT_DIR)/cl_input.c
	$(DO_CC)

$(BUILDDIR)/squake/cl_main.o :  $(ROOT_DIR)/cl_main.c
	$(DO_CC)

$(BUILDDIR)/squake/cl_parse.o : $(ROOT_DIR)/cl_parse.c
	$(DO_CC)

$(BUILDDIR)/squake/cl_tent.o :  $(ROOT_DIR)/cl_tent.c
	$(DO_CC)

$(BUILDDIR)/squake/chase.o :    $(ROOT_DIR)/chase.c
	$(DO_CC)

$(BUILDDIR)/squake/cmd.o :      $(ROOT_DIR)/cmd.c
	$(DO_CC)

$(BUILDDIR)/squake/common.o :   $(ROOT_DIR)/common.c
	$(DO_DEBUG_CC)

$(BUILDDIR)/squake/console.o :  $(ROOT_DIR)/console.c
	$(DO_CC)

$(BUILDDIR)/squake/crc.o :      $(ROOT_DIR)/crc.c
	$(DO_CC)

$(BUILDDIR)/squake/cvar.o :     $(ROOT_DIR)/cvar.c
	$(DO_CC)

$(BUILDDIR)/squake/draw.o :     $(ROOT_DIR)/draw.c
	$(DO_CC)

$(BUILDDIR)/squake/d_edge.o :   $(ROOT_DIR)/d_edge.c
	$(DO_CC)

$(BUILDDIR)/squake/d_fill.o :   $(ROOT_DIR)/d_fill.c
	$(DO_CC)

$(BUILDDIR)/squake/d_init.o :   $(ROOT_DIR)/d_init.c
	$(DO_CC)

$(BUILDDIR)/squake/d_modech.o : $(ROOT_DIR)/d_modech.c
	$(DO_CC)

$(BUILDDIR)/squake/d_part.o :   $(ROOT_DIR)/d_part.c
	$(DO_CC)

$(BUILDDIR)/squake/d_polyse.o : $(ROOT_DIR)/d_polyse.c
	$(DO_CC)

$(BUILDDIR)/squake/d_scan.o :   $(ROOT_DIR)/d_scan.c
	$(DO_CC)

$(BUILDDIR)/squake/d_sky.o :    $(ROOT_DIR)/d_sky.c
	$(DO_CC)

$(BUILDDIR)/squake/d_sprite.o : $(ROOT_DIR)/d_sprite.c
	$(DO_CC)

$(BUILDDIR)/squake/d_surf.o :   $(ROOT_DIR)/d_surf.c
	$(DO_CC)

$(BUILDDIR)/squake/d_vars.o :   $(ROOT_DIR)/d_vars.c
	$(DO_CC)

$(BUILDDIR)/squake/d_zpoint.o : $(ROOT_DIR)/d_zpoint.c
	$(DO_CC)

$(BUILDDIR)/squake/host.o :     $(ROOT_DIR)/host.c
	$(DO_CC)

$(BUILDDIR)/squake/host_cmd.o : $(ROOT_DIR)/host_cmd.c
	$(DO_CC)

$(BUILDDIR)/squake/keys.o :     $(ROOT_DIR)/keys.c
	$(DO_CC)

$(BUILDDIR)/squake/menu.o :     $(ROOT_DIR)/menu.c
	$(DO_CC)

$(BUILDDIR)/squake/mathlib.o :  $(ROOT_DIR)/mathlib.c
	$(DO_CC)

$(BUILDDIR)/squake/model.o :    $(ROOT_DIR)/model.c
	$(DO_CC)

$(BUILDDIR)/squake/net_dgrm.o : $(ROOT_DIR)/net_dgrm.c
	$(DO_CC)

$(BUILDDIR)/squake/net_loop.o : $(ROOT_DIR)/net_loop.c
	$(DO_CC)

$(BUILDDIR)/squake/net_main.o : $(ROOT_DIR)/net_main.c
	$(DO_CC)

$(BUILDDIR)/squake/net_vcr.o :  $(ROOT_DIR)/net_vcr.c
	$(DO_CC)

$(BUILDDIR)/squake/net_udp.o :  $(ROOT_DIR)/net_udp.c
	$(DO_CC)

$(BUILDDIR)/squake/net_bsd.o :  $(ROOT_DIR)/net_bsd.c
	$(DO_CC)

$(BUILDDIR)/squake/nonintel.o : $(ROOT_DIR)/nonintel.c
	$(DO_CC)

$(BUILDDIR)/squake/pr_cmds.o :  $(ROOT_DIR)/pr_cmds.c
	$(DO_CC)

$(BUILDDIR)/squake/pr_edict.o : $(ROOT_DIR)/pr_edict.c
	$(DO_CC)

$(BUILDDIR)/squake/pr_exec.o :  $(ROOT_DIR)/pr_exec.c
	$(DO_CC)

$(BUILDDIR)/squake/r_aclip.o :  $(ROOT_DIR)/r_aclip.c
	$(DO_CC)

$(BUILDDIR)/squake/r_alias.o :  $(ROOT_DIR)/r_alias.c
	$(DO_CC)

$(BUILDDIR)/squake/r_bsp.o :    $(ROOT_DIR)/r_bsp.c
	$(DO_CC)

$(BUILDDIR)/squake/r_light.o :  $(ROOT_DIR)/r_light.c
	$(DO_CC)

$(BUILDDIR)/squake/r_draw.o :   $(ROOT_DIR)/r_draw.c
	$(DO_CC)

$(BUILDDIR)/squake/r_efrag.o :  $(ROOT_DIR)/r_efrag.c
	$(DO_CC)

$(BUILDDIR)/squake/r_edge.o :   $(ROOT_DIR)/r_edge.c
	$(DO_CC)

$(BUILDDIR)/squake/r_misc.o :   $(ROOT_DIR)/r_misc.c
	$(DO_CC)

$(BUILDDIR)/squake/r_main.o :   $(ROOT_DIR)/r_main.c
	$(DO_CC)

$(BUILDDIR)/squake/r_sky.o :    $(ROOT_DIR)/r_sky.c
	$(DO_CC)

$(BUILDDIR)/squake/r_sprite.o : $(ROOT_DIR)/r_sprite.c
	$(DO_CC)

$(BUILDDIR)/squake/r_surf.o :   $(ROOT_DIR)/r_surf.c
	$(DO_CC)

$(BUILDDIR)/squake/r_part.o :   $(ROOT_DIR)/r_part.c
	$(DO_CC)

$(BUILDDIR)/squake/r_vars.o :   $(ROOT_DIR)/r_vars.c
	$(DO_CC)

$(BUILDDIR)/squake/screen.o :   $(ROOT_DIR)/screen.c
	$(DO_CC)

$(BUILDDIR)/squake/sbar.o :     $(ROOT_DIR)/sbar.c
	$(DO_CC)

$(BUILDDIR)/squake/sv_main.o :  $(ROOT_DIR)/sv_main.c
	$(DO_CC)

$(BUILDDIR)/squake/sv_phys.o :  $(ROOT_DIR)/sv_phys.c
	$(DO_CC)

$(BUILDDIR)/squake/sv_move.o :  $(ROOT_DIR)/sv_move.c
	$(DO_CC)

$(BUILDDIR)/squake/sv_user.o :  $(ROOT_DIR)/sv_user.c
	$(DO_CC)

$(BUILDDIR)/squake/zone.o :   $(ROOT_DIR)/zone.c
	$(DO_CC)

$(BUILDDIR)/squake/view.o :   $(ROOT_DIR)/view.c
	$(DO_CC)

$(BUILDDIR)/squake/wad.o :      $(ROOT_DIR)/wad.c
	$(DO_CC)

$(BUILDDIR)/squake/world.o :    $(ROOT_DIR)/world.c
	$(DO_CC)

$(BUILDDIR)/squake/cd_linux.o : $(ROOT_DIR)/cd_linux.c
	$(DO_CC)

$(BUILDDIR)/squake/sys_linux.o :$(ROOT_DIR)/sys_linux.c
	$(DO_CC)

$(BUILDDIR)/squake/vid_svgalib.o:$(ROOT_DIR)/vid_svgalib.c
	$(DO_O_CC)

$(BUILDDIR)/squake/snd_dma.o :  $(ROOT_DIR)/snd_dma.c
	$(DO_CC)

$(BUILDDIR)/squake/snd_mem.o :  $(ROOT_DIR)/snd_mem.c
	$(DO_CC)

$(BUILDDIR)/squake/snd_mix.o :  $(ROOT_DIR)/snd_mix.c
	$(DO_CC)

$(BUILDDIR)/squake/snd_linux.o :$(ROOT_DIR)/snd_linux.c
	$(DO_CC)

#####

$(BUILDDIR)/squake/d_copy.o :   $(ROOT_DIR)/d_copy.s
	$(DO_AS)

$(BUILDDIR)/squake/d_draw.o :   $(ROOT_DIR)/d_draw.s
	$(DO_AS)

$(BUILDDIR)/squake/d_draw16.o : $(ROOT_DIR)/d_draw16.s
	$(DO_AS)

$(BUILDDIR)/squake/d_parta.o :  $(ROOT_DIR)/d_parta.s
	$(DO_AS)

$(BUILDDIR)/squake/d_polysa.o : $(ROOT_DIR)/d_polysa.s
	$(DO_AS)

$(BUILDDIR)/squake/d_scana.o :  $(ROOT_DIR)/d_scana.s
	$(DO_AS)

$(BUILDDIR)/squake/d_spr8.o :   $(ROOT_DIR)/d_spr8.s
	$(DO_AS)

$(BUILDDIR)/squake/d_varsa.o :  $(ROOT_DIR)/d_varsa.s
	$(DO_AS)

$(BUILDDIR)/squake/math.o :     $(ROOT_DIR)/math.s
	$(DO_AS)

$(BUILDDIR)/squake/r_aliasa.o : $(ROOT_DIR)/r_aliasa.s
	$(DO_AS)

$(BUILDDIR)/squake/r_drawa.o :  $(ROOT_DIR)/r_drawa.s
	$(DO_AS)

$(BUILDDIR)/squake/r_edgea.o :  $(ROOT_DIR)/r_edgea.s
	$(DO_AS)

$(BUILDDIR)/squake/r_varsa.o :  $(ROOT_DIR)/r_varsa.s
	$(DO_AS)

$(BUILDDIR)/squake/surf16.o :   $(ROOT_DIR)/surf16.s
	$(DO_AS)

$(BUILDDIR)/squake/surf8.o :    $(ROOT_DIR)/surf8.s
	$(DO_AS)

$(BUILDDIR)/squake/worlda.o :   $(ROOT_DIR)/worlda.s
	$(DO_AS)

$(BUILDDIR)/squake/r_aclipa.o : $(ROOT_DIR)/r_aclipa.s
	$(DO_AS)

$(BUILDDIR)/squake/snd_mixa.o : $(ROOT_DIR)/snd_mixa.s
	$(DO_AS)

$(BUILDDIR)/squake/sys_dosa.o : $(ROOT_DIR)/sys_dosa.s
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

$(BUILDDIR)/bin/xquake : $(X11_OBJS)
	$(CC) $(CFLAGS) -o $@ $(X11_OBJS) $(X11_LDFLAGS) $(LDFLAGS)

####

$(BUILDDIR)/x11/cl_demo.o :  $(ROOT_DIR)/cl_demo.c
	$(DO_X11_CC)

$(BUILDDIR)/x11/cl_input.o : $(ROOT_DIR)/cl_input.c
	$(DO_X11_CC)

$(BUILDDIR)/x11/cl_main.o :  $(ROOT_DIR)/cl_main.c
	$(DO_X11_CC)

$(BUILDDIR)/x11/cl_parse.o : $(ROOT_DIR)/cl_parse.c
	$(DO_X11_CC)

$(BUILDDIR)/x11/cl_tent.o :  $(ROOT_DIR)/cl_tent.c
	$(DO_X11_CC)

$(BUILDDIR)/x11/chase.o :    $(ROOT_DIR)/chase.c
	$(DO_X11_CC)

$(BUILDDIR)/x11/cmd.o :      $(ROOT_DIR)/cmd.c
	$(DO_X11_CC)

$(BUILDDIR)/x11/common.o :   $(ROOT_DIR)/common.c
	$(DO_X11_DEBUG_CC)

$(BUILDDIR)/x11/console.o :  $(ROOT_DIR)/console.c
	$(DO_X11_CC)

$(BUILDDIR)/x11/crc.o :      $(ROOT_DIR)/crc.c
	$(DO_X11_CC)

$(BUILDDIR)/x11/cvar.o :     $(ROOT_DIR)/cvar.c
	$(DO_X11_CC)

$(BUILDDIR)/x11/draw.o :     $(ROOT_DIR)/draw.c
	$(DO_X11_CC)

$(BUILDDIR)/x11/d_edge.o :   $(ROOT_DIR)/d_edge.c
	$(DO_X11_CC)

$(BUILDDIR)/x11/d_fill.o :   $(ROOT_DIR)/d_fill.c
	$(DO_X11_CC)

$(BUILDDIR)/x11/d_init.o :   $(ROOT_DIR)/d_init.c
	$(DO_X11_CC)

$(BUILDDIR)/x11/d_modech.o : $(ROOT_DIR)/d_modech.c
	$(DO_X11_CC)

$(BUILDDIR)/x11/d_part.o :   $(ROOT_DIR)/d_part.c
	$(DO_X11_CC)

$(BUILDDIR)/x11/d_polyse.o : $(ROOT_DIR)/d_polyse.c
	$(DO_X11_CC)

$(BUILDDIR)/x11/d_scan.o :   $(ROOT_DIR)/d_scan.c
	$(DO_X11_CC)

$(BUILDDIR)/x11/d_sky.o :    $(ROOT_DIR)/d_sky.c
	$(DO_X11_CC)

$(BUILDDIR)/x11/d_sprite.o : $(ROOT_DIR)/d_sprite.c
	$(DO_X11_CC)

$(BUILDDIR)/x11/d_surf.o :   $(ROOT_DIR)/d_surf.c
	$(DO_X11_CC)

$(BUILDDIR)/x11/d_vars.o :   $(ROOT_DIR)/d_vars.c
	$(DO_X11_CC)

$(BUILDDIR)/x11/d_zpoint.o : $(ROOT_DIR)/d_zpoint.c
	$(DO_X11_CC)

$(BUILDDIR)/x11/host.o :     $(ROOT_DIR)/host.c
	$(DO_X11_CC)

$(BUILDDIR)/x11/host_cmd.o : $(ROOT_DIR)/host_cmd.c
	$(DO_X11_CC)

$(BUILDDIR)/x11/keys.o :     $(ROOT_DIR)/keys.c
	$(DO_X11_CC)

$(BUILDDIR)/x11/menu.o :     $(ROOT_DIR)/menu.c
	$(DO_X11_CC)

$(BUILDDIR)/x11/mathlib.o :  $(ROOT_DIR)/mathlib.c
	$(DO_X11_CC)

$(BUILDDIR)/x11/model.o :    $(ROOT_DIR)/model.c
	$(DO_X11_CC)

$(BUILDDIR)/x11/net_dgrm.o : $(ROOT_DIR)/net_dgrm.c
	$(DO_X11_CC)

$(BUILDDIR)/x11/net_loop.o : $(ROOT_DIR)/net_loop.c
	$(DO_X11_CC)

$(BUILDDIR)/x11/net_main.o : $(ROOT_DIR)/net_main.c
	$(DO_X11_CC)

$(BUILDDIR)/x11/net_vcr.o :  $(ROOT_DIR)/net_vcr.c
	$(DO_X11_CC)

$(BUILDDIR)/x11/net_udp.o :  $(ROOT_DIR)/net_udp.c
	$(DO_X11_CC)

$(BUILDDIR)/x11/net_bsd.o :  $(ROOT_DIR)/net_bsd.c
	$(DO_X11_CC)

$(BUILDDIR)/x11/nonintel.o : $(ROOT_DIR)/nonintel.c
	$(DO_X11_CC)

$(BUILDDIR)/x11/pr_cmds.o :  $(ROOT_DIR)/pr_cmds.c
	$(DO_X11_CC)

$(BUILDDIR)/x11/pr_edict.o : $(ROOT_DIR)/pr_edict.c
	$(DO_X11_CC)

$(BUILDDIR)/x11/pr_exec.o :  $(ROOT_DIR)/pr_exec.c
	$(DO_X11_CC)

$(BUILDDIR)/x11/r_aclip.o :  $(ROOT_DIR)/r_aclip.c
	$(DO_X11_CC)

$(BUILDDIR)/x11/r_alias.o :  $(ROOT_DIR)/r_alias.c
	$(DO_X11_CC)

$(BUILDDIR)/x11/r_bsp.o :    $(ROOT_DIR)/r_bsp.c
	$(DO_X11_CC)

$(BUILDDIR)/x11/r_light.o :  $(ROOT_DIR)/r_light.c
	$(DO_X11_CC)

$(BUILDDIR)/x11/r_draw.o :   $(ROOT_DIR)/r_draw.c
	$(DO_X11_CC)

$(BUILDDIR)/x11/r_efrag.o :  $(ROOT_DIR)/r_efrag.c
	$(DO_X11_CC)

$(BUILDDIR)/x11/r_edge.o :   $(ROOT_DIR)/r_edge.c
	$(DO_X11_CC)

$(BUILDDIR)/x11/r_misc.o :   $(ROOT_DIR)/r_misc.c
	$(DO_X11_CC)

$(BUILDDIR)/x11/r_main.o :   $(ROOT_DIR)/r_main.c
	$(DO_X11_CC)

$(BUILDDIR)/x11/r_sky.o :    $(ROOT_DIR)/r_sky.c
	$(DO_X11_CC)

$(BUILDDIR)/x11/r_sprite.o : $(ROOT_DIR)/r_sprite.c
	$(DO_X11_CC)

$(BUILDDIR)/x11/r_surf.o :   $(ROOT_DIR)/r_surf.c
	$(DO_X11_CC)

$(BUILDDIR)/x11/r_part.o :   $(ROOT_DIR)/r_part.c
	$(DO_X11_CC)

$(BUILDDIR)/x11/r_vars.o :   $(ROOT_DIR)/r_vars.c
	$(DO_X11_CC)

$(BUILDDIR)/x11/screen.o :   $(ROOT_DIR)/screen.c
	$(DO_X11_CC)

$(BUILDDIR)/x11/sbar.o :     $(ROOT_DIR)/sbar.c
	$(DO_X11_CC)

$(BUILDDIR)/x11/sv_main.o :  $(ROOT_DIR)/sv_main.c
	$(DO_X11_CC)

$(BUILDDIR)/x11/sv_phys.o :  $(ROOT_DIR)/sv_phys.c
	$(DO_X11_CC)

$(BUILDDIR)/x11/sv_move.o :  $(ROOT_DIR)/sv_move.c
	$(DO_X11_CC)

$(BUILDDIR)/x11/sv_user.o :  $(ROOT_DIR)/sv_user.c
	$(DO_X11_CC)

$(BUILDDIR)/x11/zone.o :   $(ROOT_DIR)/zone.c
	$(DO_X11_CC)

$(BUILDDIR)/x11/view.o :   $(ROOT_DIR)/view.c
	$(DO_X11_CC)

$(BUILDDIR)/x11/wad.o :      $(ROOT_DIR)/wad.c
	$(DO_X11_CC)

$(BUILDDIR)/x11/world.o :    $(ROOT_DIR)/world.c
	$(DO_X11_CC)

$(BUILDDIR)/x11/cd_linux.o : $(ROOT_DIR)/cd_linux.c
	$(DO_X11_CC)

$(BUILDDIR)/x11/sys_linux.o :$(ROOT_DIR)/sys_linux.c
	$(DO_X11_CC)

$(BUILDDIR)/x11/vid_x.o: $(ROOT_DIR)/vid_x.c
	$(DO_O_CC)

$(BUILDDIR)/x11/snd_dma.o :  $(ROOT_DIR)/snd_dma.c
	$(DO_X11_CC)

$(BUILDDIR)/x11/snd_mem.o :  $(ROOT_DIR)/snd_mem.c
	$(DO_X11_CC)

$(BUILDDIR)/x11/snd_mix.o :  $(ROOT_DIR)/snd_mix.c
	$(DO_X11_CC)

$(BUILDDIR)/x11/snd_linux.o :$(ROOT_DIR)/snd_linux.c
	$(DO_X11_CC)

#####

$(BUILDDIR)/x11/d_copy.o :   $(ROOT_DIR)/d_copy.s
	$(DO_AS)

$(BUILDDIR)/x11/d_draw.o :   $(ROOT_DIR)/d_draw.s
	$(DO_AS)

$(BUILDDIR)/x11/d_draw16.o : $(ROOT_DIR)/d_draw16.s
	$(DO_AS)

$(BUILDDIR)/x11/d_parta.o :  $(ROOT_DIR)/d_parta.s
	$(DO_AS)

$(BUILDDIR)/x11/d_polysa.o : $(ROOT_DIR)/d_polysa.s
	$(DO_AS)

$(BUILDDIR)/x11/d_scana.o :  $(ROOT_DIR)/d_scana.s
	$(DO_AS)

$(BUILDDIR)/x11/d_spr8.o :   $(ROOT_DIR)/d_spr8.s
	$(DO_AS)

$(BUILDDIR)/x11/d_varsa.o :  $(ROOT_DIR)/d_varsa.s
	$(DO_AS)

$(BUILDDIR)/x11/math.o :     $(ROOT_DIR)/math.s
	$(DO_AS)

$(BUILDDIR)/x11/r_aliasa.o : $(ROOT_DIR)/r_aliasa.s
	$(DO_AS)

$(BUILDDIR)/x11/r_drawa.o :  $(ROOT_DIR)/r_drawa.s
	$(DO_AS)

$(BUILDDIR)/x11/r_edgea.o :  $(ROOT_DIR)/r_edgea.s
	$(DO_AS)

$(BUILDDIR)/x11/r_varsa.o :  $(ROOT_DIR)/r_varsa.s
	$(DO_AS)

$(BUILDDIR)/x11/surf16.o :   $(ROOT_DIR)/surf16.s
	$(DO_AS)

$(BUILDDIR)/x11/surf8.o :    $(ROOT_DIR)/surf8.s
	$(DO_AS)

$(BUILDDIR)/x11/worlda.o :   $(ROOT_DIR)/worlda.s
	$(DO_AS)

$(BUILDDIR)/x11/r_aclipa.o : $(ROOT_DIR)/r_aclipa.s
	$(DO_AS)

$(BUILDDIR)/x11/snd_mixa.o : $(ROOT_DIR)/snd_mixa.s
	$(DO_AS)

$(BUILDDIR)/x11/sys_dosa.o : $(ROOT_DIR)/sys_dosa.s
	$(DO_AS)

#############################################################################
# GLQuake
#############################################################################

GLQUAKE_OBJS= \
	$(BUILDDIR)/glquake/cl_demo.o \
	$(BUILDDIR)/glquake/cl_input.o \
	$(BUILDDIR)/glquake/cl_main.o \
	$(BUILDDIR)/glquake/cl_parse.o \
	$(BUILDDIR)/glquake/cl_tent.o \
	$(BUILDDIR)/glquake/chase.o \
	$(BUILDDIR)/glquake/cmd.o \
	$(BUILDDIR)/glquake/common.o \
	$(BUILDDIR)/glquake/console.o \
	$(BUILDDIR)/glquake/crc.o \
	$(BUILDDIR)/glquake/cvar.o \
	\
	$(BUILDDIR)/glquake/gl_draw.o \
	$(BUILDDIR)/glquake/gl_mesh.o \
	$(BUILDDIR)/glquake/gl_model.o \
	$(BUILDDIR)/glquake/gl_refrag.o \
	$(BUILDDIR)/glquake/gl_rlight.o \
	$(BUILDDIR)/glquake/gl_rmain.o \
	$(BUILDDIR)/glquake/gl_rmisc.o \
	$(BUILDDIR)/glquake/gl_rsurf.o \
	$(BUILDDIR)/glquake/gl_screen.o \
	$(BUILDDIR)/glquake/gl_test.o \
	$(BUILDDIR)/glquake/gl_warp.o \
	\
	$(BUILDDIR)/glquake/host.o \
	$(BUILDDIR)/glquake/host_cmd.o \
	$(BUILDDIR)/glquake/keys.o \
	$(BUILDDIR)/glquake/menu.o \
	$(BUILDDIR)/glquake/mathlib.o \
	$(BUILDDIR)/glquake/net_dgrm.o \
	$(BUILDDIR)/glquake/net_loop.o \
	$(BUILDDIR)/glquake/net_main.o \
	$(BUILDDIR)/glquake/net_vcr.o \
	$(BUILDDIR)/glquake/net_udp.o \
	$(BUILDDIR)/glquake/net_bsd.o \
	$(BUILDDIR)/glquake/pr_cmds.o \
	$(BUILDDIR)/glquake/pr_edict.o \
	$(BUILDDIR)/glquake/pr_exec.o \
	$(BUILDDIR)/glquake/r_part.o \
	$(BUILDDIR)/glquake/sbar.o \
	$(BUILDDIR)/glquake/sv_main.o \
	$(BUILDDIR)/glquake/sv_phys.o \
	$(BUILDDIR)/glquake/sv_move.o \
	$(BUILDDIR)/glquake/sv_user.o \
	$(BUILDDIR)/glquake/zone.o	\
	$(BUILDDIR)/glquake/view.o	\
	$(BUILDDIR)/glquake/wad.o \
	$(BUILDDIR)/glquake/world.o \
	$(BUILDDIR)/glquake/cd_linux.o \
	$(BUILDDIR)/glquake/sys_linux.o \
	$(BUILDDIR)/glquake/snd_dma.o \
	$(BUILDDIR)/glquake/snd_mem.o \
	$(BUILDDIR)/glquake/snd_mix.o \
	$(BUILDDIR)/glquake/snd_linux.o \
	\
	$(BUILDDIR)/glquake/math.o \
	$(BUILDDIR)/glquake/worlda.o \
	$(BUILDDIR)/glquake/snd_mixa.o \
	$(BUILDDIR)/glquake/sys_dosa.o

GLSVGA_OBJS=$(BUILDDIR)/glquake/gl_vidlinux.o

GLX_OBJS=$(BUILDDIR)/glquake/gl_vidlinuxglx.o

$(BUILDDIR)/bin/glquake : $(GLQUAKE_OBJS) $(GLX_OBJS)
	$(CC) $(CFLAGS) -o $@ $(GLQUAKE_OBJS) $(GLX_OBJS) $(GL_LDFLAGS) $(SVGA_LDFLAGS) $(LDFLAGS)

$(BUILDDIR)/bin/glquake.3dfx : $(GLQUAKE_OBJS) $(GLSVGA_OBJS)
	$(CC) $(CFLAGS) -o $@ $(GLQUAKE_OBJS) $(GLSVGA_OBJS) $(GL_LDFLAGS) $(LDFLAGS)

$(BUILDDIR)/glquake/cl_demo.o :      $(ROOT_DIR)/cl_demo.c
	$(DO_GL_CC)

$(BUILDDIR)/glquake/cl_input.o :     $(ROOT_DIR)/cl_input.c
	$(DO_GL_CC)

$(BUILDDIR)/glquake/cl_main.o :      $(ROOT_DIR)/cl_main.c
	$(DO_GL_CC)

$(BUILDDIR)/glquake/cl_parse.o :     $(ROOT_DIR)/cl_parse.c
	$(DO_GL_CC)

$(BUILDDIR)/glquake/cl_tent.o :      $(ROOT_DIR)/cl_tent.c
	$(DO_GL_CC)

$(BUILDDIR)/glquake/chase.o :        $(ROOT_DIR)/chase.c
	$(DO_GL_CC)

$(BUILDDIR)/glquake/cmd.o :          $(ROOT_DIR)/cmd.c
	$(DO_GL_CC)

$(BUILDDIR)/glquake/common.o :       $(ROOT_DIR)/common.c
	$(DO_GL_DEBUG_CC)

$(BUILDDIR)/glquake/console.o :      $(ROOT_DIR)/console.c
	$(DO_GL_CC)

$(BUILDDIR)/glquake/crc.o :          $(ROOT_DIR)/crc.c
	$(DO_GL_CC)

$(BUILDDIR)/glquake/cvar.o :         $(ROOT_DIR)/cvar.c
	$(DO_GL_CC)

$(BUILDDIR)/glquake/gl_draw.o :      $(ROOT_DIR)/gl_draw.c
	$(DO_GL_CC)

$(BUILDDIR)/glquake/gl_mesh.o :      $(ROOT_DIR)/gl_mesh.c
	$(DO_GL_CC)

$(BUILDDIR)/glquake/gl_model.o :     $(ROOT_DIR)/gl_model.c
	$(DO_GL_CC)

$(BUILDDIR)/glquake/gl_refrag.o :    $(ROOT_DIR)/gl_refrag.c
	$(DO_GL_CC)

$(BUILDDIR)/glquake/gl_rlight.o :    $(ROOT_DIR)/gl_rlight.c
	$(DO_GL_CC)

$(BUILDDIR)/glquake/gl_rmain.o :     $(ROOT_DIR)/gl_rmain.c
	$(DO_GL_CC)

$(BUILDDIR)/glquake/gl_rmisc.o :     $(ROOT_DIR)/gl_rmisc.c
	$(DO_GL_CC)

$(BUILDDIR)/glquake/gl_rsurf.o :     $(ROOT_DIR)/gl_rsurf.c
	$(DO_GL_CC)

$(BUILDDIR)/glquake/gl_screen.o :    $(ROOT_DIR)/gl_screen.c
	$(DO_GL_CC)

$(BUILDDIR)/glquake/gl_test.o :      $(ROOT_DIR)/gl_test.c
	$(DO_GL_CC)

$(BUILDDIR)/glquake/gl_vidlinux.o :  $(ROOT_DIR)/gl_vidlinux.c
	$(DO_GL_CC)

$(BUILDDIR)/glquake/gl_vidlinuxglx.o :  $(ROOT_DIR)/gl_vidlinuxglx.c
	$(DO_GL_CC)

$(BUILDDIR)/glquake/gl_warp.o :      $(ROOT_DIR)/gl_warp.c
	$(DO_GL_CC)

$(BUILDDIR)/glquake/host.o :         $(ROOT_DIR)/host.c
	$(DO_GL_CC)

$(BUILDDIR)/glquake/host_cmd.o :     $(ROOT_DIR)/host_cmd.c
	$(DO_GL_CC)

$(BUILDDIR)/glquake/keys.o :         $(ROOT_DIR)/keys.c
	$(DO_GL_CC)

$(BUILDDIR)/glquake/menu.o :         $(ROOT_DIR)/menu.c
	$(DO_GL_CC)

$(BUILDDIR)/glquake/mathlib.o :      $(ROOT_DIR)/mathlib.c
	$(DO_GL_CC)

$(BUILDDIR)/glquake/net_dgrm.o :     $(ROOT_DIR)/net_dgrm.c
	$(DO_GL_CC)

$(BUILDDIR)/glquake/net_loop.o :     $(ROOT_DIR)/net_loop.c
	$(DO_GL_CC)

$(BUILDDIR)/glquake/net_main.o :     $(ROOT_DIR)/net_main.c
	$(DO_GL_CC)

$(BUILDDIR)/glquake/net_vcr.o :      $(ROOT_DIR)/net_vcr.c
	$(DO_GL_CC)

$(BUILDDIR)/glquake/net_udp.o :      $(ROOT_DIR)/net_udp.c
	$(DO_GL_CC)

$(BUILDDIR)/glquake/net_bsd.o :      $(ROOT_DIR)/net_bsd.c
	$(DO_GL_CC)

$(BUILDDIR)/glquake/pr_cmds.o :      $(ROOT_DIR)/pr_cmds.c
	$(DO_GL_CC)

$(BUILDDIR)/glquake/pr_edict.o :     $(ROOT_DIR)/pr_edict.c
	$(DO_GL_CC)

$(BUILDDIR)/glquake/pr_exec.o :      $(ROOT_DIR)/pr_exec.c
	$(DO_GL_CC)

$(BUILDDIR)/glquake/r_part.o :       $(ROOT_DIR)/r_part.c
	$(DO_GL_CC)

$(BUILDDIR)/glquake/sbar.o :         $(ROOT_DIR)/sbar.c
	$(DO_GL_CC)

$(BUILDDIR)/glquake/sv_main.o :      $(ROOT_DIR)/sv_main.c
	$(DO_GL_CC)

$(BUILDDIR)/glquake/sv_phys.o :      $(ROOT_DIR)/sv_phys.c
	$(DO_GL_CC)

$(BUILDDIR)/glquake/sv_move.o :      $(ROOT_DIR)/sv_move.c
	$(DO_GL_CC)

$(BUILDDIR)/glquake/sv_user.o :      $(ROOT_DIR)/sv_user.c
	$(DO_GL_CC)

$(BUILDDIR)/glquake/zone.o :        $(ROOT_DIR)/zone.c
	$(DO_GL_CC)

$(BUILDDIR)/glquake/view.o :        $(ROOT_DIR)/view.c
	$(DO_GL_CC)

$(BUILDDIR)/glquake/wad.o :          $(ROOT_DIR)/wad.c
	$(DO_GL_CC)

$(BUILDDIR)/glquake/world.o :        $(ROOT_DIR)/world.c
	$(DO_GL_CC)

$(BUILDDIR)/glquake/cd_linux.o :     $(ROOT_DIR)/cd_linux.c
	$(DO_GL_CC)

$(BUILDDIR)/glquake/sys_linux.o :    $(ROOT_DIR)/sys_linux.c
	$(DO_GL_CC)

$(BUILDDIR)/glquake/snd_dma.o :      $(ROOT_DIR)/snd_dma.c
	$(DO_GL_CC)

$(BUILDDIR)/glquake/snd_mem.o :      $(ROOT_DIR)/snd_mem.c
	$(DO_GL_CC)

$(BUILDDIR)/glquake/snd_mix.o :      $(ROOT_DIR)/snd_mix.c
	$(DO_GL_CC)

$(BUILDDIR)/glquake/snd_linux.o :    $(ROOT_DIR)/snd_linux.c
	$(DO_GL_CC)

$(BUILDDIR)/glquake/math.o :         $(ROOT_DIR)/math.s
	$(DO_GL_AS)

$(BUILDDIR)/glquake/worlda.o :       $(ROOT_DIR)/worlda.s
	$(DO_GL_AS)

$(BUILDDIR)/glquake/snd_mixa.o :     $(ROOT_DIR)/snd_mixa.s
	$(DO_GL_AS)

$(BUILDDIR)/glquake/sys_dosa.o :     $(ROOT_DIR)/sys_dosa.s
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
