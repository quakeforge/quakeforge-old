#
# UQuake makefile for unified tree
#

VERSION=0.1.0

ifneq (,$(findstring alpha,$(shell uname -m)))
ARCH=axp
else
$(shell uname -m)
endif

PROJECT_DIR=..

COMMON_DIR=$(PROJECT_DIR)/common
UQ_DIR=$(PROJECT_DIR)/uquake

BUILD_DIR=$(PROJECT_DIR)/build
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
RELEASE_CFLAGS=$(BASE_CFLAGS) -O3 -ffast-math -funroll-loops \
	-fomit-frame-pointer -fexpensive-optimizations
endif
GL_CFLAGS=-DGLQUAKE -I$(X11_DIR)/include -I/usr/include/glide

LDFLAGS=-lm
SVGA_LDFLAGS=$(LDFLAGS) -lvga
X11_LDFLAGS=-L$(X11_DIR)/lib $(LDFLAGS) -lX11 -lXext
GL_GLX_LDFLAGS=-L$(X11_DIR)/lib $(LDFLAGS) -lGL -lX11 -lXext
GL_GLIDE_LDFLAGS=-L$(X11_DIR)/lib -lvga -lGL -lglide2x -lX11 -lXext -ldl

DO_CC=$(CC) $(CFLAGS) -o $@ -c $<
DO_AS=$(CC) $(CFLAGS) -DELF -x assembler-with-cpp -o $@ -c $<

DO_GL_CC=$(CC) $(CFLAGS) $(GL_CFLAGS) -o $@ -c $<
DO_GL_AS=$(CC) $(CFLAGS) $(GL_CFLAGS) -DELF -x assembler-with-cpp -o $@ -c $<

DO_O_CC=$(CC) -O $(CFLAGS) -o $@ -c $<

#############################################################################
# SETUP AND BUILD
#############################################################################

SOFT_TARGETS=$(BUILD_DIR)/quake-svga \
	$(BUILD_DIR)/quake-x11
GL_TARGETS=$(BUILD_DIR)/quake-glx \
	$(BUILD_DIR)/quake-glide

debug:		debug-soft debug-gl

release:	release-soft release-gl

all:		debug-soft release-soft debug-gl release-gl

debug-soft:	$(BUILD_DIR) $(BUILD_DIR)/soft_uq_obj $(SOFT_OBJS) $(SOFT_AS_OBJS) $(SOFT_SVGA_OBJS) $(SOFT_X11_OBJS)
		
	$(MAKE) soft-targets CFLAGS="$(DEBUG_CFLAGS)"

debug-gl:	$(BUILD_DIR) $(BUILD_DIR)/gl_uq_obj $(GL_OBJS) $(GL_AS_OBJS) $(GL_SVGA_OBJS) $(GL_X11_OBJS)
		
	$(MAKE) gl-targets CFLAGS="$(DEBUG_CFLAGS)"

release-soft:	$(BUILD_DIR) $(BUILD_DIR)/soft_uq_obj $(SOFT_OBJS) $(SOFT_AS_OBJS) $(SOFT_SVGA_OBJS) $(SOFT_X11_OBJS)
		
	$(MAKE) soft-targets CFLAGS="$(RELEASE_CFLAGS)"

release-gl:	$(BUILD_DIR) $(BUILD_DIR)/gl_uq_obj $(GL_OBJS) $(GL_AS_OBJS) $(GL_SVGA_OBJS) $(GL_X11_OBJS)
		
	$(MAKE) gl-targets CFLAGS="$(RELEASE_CFLAGS)"

soft-targets:	$(SOFT_TARGETS)
gl-targets:	$(GL_TARGETS)

$(BUILD_DIR):
	@mkdir $(BUILD_DIR)
	
$(BUILD_DIR)/soft_uq_obj:
	@mkdir $(BUILD_DIR)/soft_uq_obj
	
$(BUILD_DIR)/gl_uq_obj:
	@mkdir $(BUILD_DIR)/gl_uq_obj

#############################################################################
# Software Renderer
#############################################################################

SOFT_OBJS = \
	$(BUILD_DIR)/soft_uq_obj/cl_demo.o \
	$(BUILD_DIR)/soft_uq_obj/cl_input.o \
	$(BUILD_DIR)/soft_uq_obj/cl_main.o \
	$(BUILD_DIR)/soft_uq_obj/cl_parse.o \
	$(BUILD_DIR)/soft_uq_obj/cl_tent.o \
	$(BUILD_DIR)/soft_uq_obj/chase.o \
	$(BUILD_DIR)/soft_uq_obj/cmd.o \
	$(BUILD_DIR)/soft_uq_obj/common.o \
	$(BUILD_DIR)/soft_uq_obj/console.o \
	$(BUILD_DIR)/soft_uq_obj/crc.o \
	$(BUILD_DIR)/soft_uq_obj/cvar.o \
	$(BUILD_DIR)/soft_uq_obj/draw.o \
	$(BUILD_DIR)/soft_uq_obj/d_edge.o \
	$(BUILD_DIR)/soft_uq_obj/d_fill.o \
	$(BUILD_DIR)/soft_uq_obj/d_init.o \
	$(BUILD_DIR)/soft_uq_obj/d_modech.o \
	$(BUILD_DIR)/soft_uq_obj/d_part.o \
	$(BUILD_DIR)/soft_uq_obj/d_polyse.o \
	$(BUILD_DIR)/soft_uq_obj/d_scan.o \
	$(BUILD_DIR)/soft_uq_obj/d_sky.o \
	$(BUILD_DIR)/soft_uq_obj/d_sprite.o \
	$(BUILD_DIR)/soft_uq_obj/d_surf.o \
	$(BUILD_DIR)/soft_uq_obj/d_vars.o \
	$(BUILD_DIR)/soft_uq_obj/d_zpoint.o \
	$(BUILD_DIR)/soft_uq_obj/host.o \
	$(BUILD_DIR)/soft_uq_obj/host_cmd.o \
	$(BUILD_DIR)/soft_uq_obj/keys.o \
	$(BUILD_DIR)/soft_uq_obj/mathlib.o \
	$(BUILD_DIR)/soft_uq_obj/menu.o \
	$(BUILD_DIR)/soft_uq_obj/model.o \
	$(BUILD_DIR)/soft_uq_obj/net_bsd.o \
	$(BUILD_DIR)/soft_uq_obj/net_dgrm.o \
	$(BUILD_DIR)/soft_uq_obj/net_loop.o \
	$(BUILD_DIR)/soft_uq_obj/net_main.o \
	$(BUILD_DIR)/soft_uq_obj/net_udp.o \
	$(BUILD_DIR)/soft_uq_obj/net_vcr.o \
	$(BUILD_DIR)/soft_uq_obj/nonintel.o \
	$(BUILD_DIR)/soft_uq_obj/pr_cmds.o \
	$(BUILD_DIR)/soft_uq_obj/pr_edict.o \
	$(BUILD_DIR)/soft_uq_obj/pr_exec.o \
	$(BUILD_DIR)/soft_uq_obj/r_aclip.o \
	$(BUILD_DIR)/soft_uq_obj/r_alias.o \
	$(BUILD_DIR)/soft_uq_obj/r_bsp.o \
	$(BUILD_DIR)/soft_uq_obj/r_draw.o \
	$(BUILD_DIR)/soft_uq_obj/r_edge.o \
	$(BUILD_DIR)/soft_uq_obj/r_efrag.o \
	$(BUILD_DIR)/soft_uq_obj/r_light.o \
	$(BUILD_DIR)/soft_uq_obj/r_main.o \
	$(BUILD_DIR)/soft_uq_obj/r_misc.o \
	$(BUILD_DIR)/soft_uq_obj/r_part.o \
	$(BUILD_DIR)/soft_uq_obj/r_sky.o \
	$(BUILD_DIR)/soft_uq_obj/r_sprite.o \
	$(BUILD_DIR)/soft_uq_obj/r_surf.o \
	$(BUILD_DIR)/soft_uq_obj/r_vars.o \
	$(BUILD_DIR)/soft_uq_obj/sbar.o \
	$(BUILD_DIR)/soft_uq_obj/screen.o \
	$(BUILD_DIR)/soft_uq_obj/snd_dma.o \
	$(BUILD_DIR)/soft_uq_obj/snd_mem.o \
	$(BUILD_DIR)/soft_uq_obj/snd_mix.o \
	$(BUILD_DIR)/soft_uq_obj/sv_main.o \
	$(BUILD_DIR)/soft_uq_obj/sv_phys.o \
	$(BUILD_DIR)/soft_uq_obj/sv_move.o \
	$(BUILD_DIR)/soft_uq_obj/sv_user.o \
	$(BUILD_DIR)/soft_uq_obj/view.o \
	$(BUILD_DIR)/soft_uq_obj/wad.o \
	$(BUILD_DIR)/soft_uq_obj/world.o \
	$(BUILD_DIR)/soft_uq_obj/zone.o \
	$(BUILD_DIR)/soft_uq_obj/cd_linux.o \
	$(BUILD_DIR)/soft_uq_obj/snd_linux.o \
	$(BUILD_DIR)/soft_uq_obj/sys_linux.o \

ifeq ($(ARCH),i386)
SOFT_AS_OBJS = \
	$(BUILD_DIR)/soft_uq_obj/d_copy.o \
	$(BUILD_DIR)/soft_uq_obj/d_draw.o \
	$(BUILD_DIR)/soft_uq_obj/d_draw16.o \
	$(BUILD_DIR)/soft_uq_obj/d_parta.o \
	$(BUILD_DIR)/soft_uq_obj/d_polysa.o \
	$(BUILD_DIR)/soft_uq_obj/d_scana.o \
	$(BUILD_DIR)/soft_uq_obj/d_spr8.o \
	$(BUILD_DIR)/soft_uq_obj/d_varsa.o \
	$(BUILD_DIR)/soft_uq_obj/math.o \
	$(BUILD_DIR)/soft_uq_obj/r_aliasa.o \
	$(BUILD_DIR)/soft_uq_obj/r_drawa.o \
	$(BUILD_DIR)/soft_uq_obj/r_edgea.o \
	$(BUILD_DIR)/soft_uq_obj/r_varsa.o \
	$(BUILD_DIR)/soft_uq_obj/surf16.o \
	$(BUILD_DIR)/soft_uq_obj/surf8.o \
	$(BUILD_DIR)/soft_uq_obj/worlda.o \
	$(BUILD_DIR)/soft_uq_obj/r_aclipa.o \
	$(BUILD_DIR)/soft_uq_obj/snd_mixa.o \
	$(BUILD_DIR)/soft_uq_obj/sys_dosa.o
else
SOFT_AS_OBJS=
endif

SOFT_SVGA_OBJS = $(BUILD_DIR)/soft_uq_obj/vid_svgalib.o
SOFT_X11_OBJS = $(BUILD_DIR)/soft_uq_obj/vid_x.o

$(BUILD_DIR)/quake-svga:	$(SOFT_OBJS) $(SOFT_AS_OBJS) $(SOFT_SVGA_OBJS)
	$(CC) $(CFLAGS) -o $@ $(SOFT_OBJS) $(SOFT_AS_OBJS) $(SOFT_SVGA_OBJS) \
		$(SVGA_LDFLAGS)

$(BUILD_DIR)/quake-x11:	$(SOFT_OBJS) $(SOFT_AS_OBJS) $(SOFT_X11_OBJS)
	$(CC) $(CFLAGS) -o $@ $(SOFT_OBJS) $(SOFT_AS_OBJS) $(SOFT_X11_OBJS) \
		$(X11_LDFLAGS)


$(BUILD_DIR)/soft_uq_obj/cl_demo.o:	$(UQ_DIR)/cl_demo.c
	$(DO_CC)

$(BUILD_DIR)/soft_uq_obj/cl_input.o:	$(UQ_DIR)/cl_input.c
	$(DO_CC)

$(BUILD_DIR)/soft_uq_obj/cl_main.o:	$(UQ_DIR)/cl_main.c
	$(DO_CC)

$(BUILD_DIR)/soft_uq_obj/cl_parse.o:	$(UQ_DIR)/cl_parse.c
	$(DO_CC)

$(BUILD_DIR)/soft_uq_obj/cl_tent.o:	$(UQ_DIR)/cl_tent.c
	$(DO_CC)

$(BUILD_DIR)/soft_uq_obj/chase.o:	$(UQ_DIR)/chase.c
	$(DO_CC)

$(BUILD_DIR)/soft_uq_obj/cmd.o:	$(UQ_DIR)/cmd.c
	$(DO_CC)

$(BUILD_DIR)/soft_uq_obj/common.o:	$(UQ_DIR)/common.c
	$(DO_CC)

$(BUILD_DIR)/soft_uq_obj/console.o:	$(UQ_DIR)/console.c
	$(DO_CC)

$(BUILD_DIR)/soft_uq_obj/crc.o:	$(UQ_DIR)/crc.c
	$(DO_CC)

$(BUILD_DIR)/soft_uq_obj/cvar.o:	$(UQ_DIR)/cvar.c
	$(DO_CC)

$(BUILD_DIR)/soft_uq_obj/draw.o:	$(UQ_DIR)/draw.c
	$(DO_CC)

$(BUILD_DIR)/soft_uq_obj/d_edge.o:	$(UQ_DIR)/d_edge.c
	$(DO_CC)

$(BUILD_DIR)/soft_uq_obj/d_fill.o:	$(UQ_DIR)/d_fill.c
	$(DO_CC)

$(BUILD_DIR)/soft_uq_obj/d_init.o:	$(UQ_DIR)/d_init.c
	$(DO_CC)

$(BUILD_DIR)/soft_uq_obj/d_modech.o:	$(UQ_DIR)/d_modech.c
	$(DO_CC)

$(BUILD_DIR)/soft_uq_obj/d_part.o:	$(UQ_DIR)/d_part.c
	$(DO_CC)

$(BUILD_DIR)/soft_uq_obj/d_polyse.o:	$(UQ_DIR)/d_polyse.c
	$(DO_CC)

$(BUILD_DIR)/soft_uq_obj/d_scan.o:	$(UQ_DIR)/d_scan.c
	$(DO_CC)

$(BUILD_DIR)/soft_uq_obj/d_sky.o:	$(UQ_DIR)/d_sky.c
	$(DO_CC)

$(BUILD_DIR)/soft_uq_obj/d_sprite.o:	$(UQ_DIR)/d_sprite.c
	$(DO_CC)

$(BUILD_DIR)/soft_uq_obj/d_surf.o:	$(UQ_DIR)/d_surf.c
	$(DO_CC)

$(BUILD_DIR)/soft_uq_obj/d_vars.o:	$(UQ_DIR)/d_vars.c
	$(DO_CC)

$(BUILD_DIR)/soft_uq_obj/d_zpoint.o:	$(UQ_DIR)/d_zpoint.c
	$(DO_CC)

$(BUILD_DIR)/soft_uq_obj/host.o :     $(UQ_DIR)/host.c
	$(DO_CC)

$(BUILD_DIR)/soft_uq_obj/host_cmd.o : $(UQ_DIR)/host_cmd.c
	$(DO_CC)

$(BUILD_DIR)/soft_uq_obj/keys.o:	$(UQ_DIR)/keys.c
	$(DO_CC)

$(BUILD_DIR)/soft_uq_obj/mathlib.o:	$(COMMON_DIR)/mathlib.c
	$(DO_CC)

$(BUILD_DIR)/soft_uq_obj/menu.o:	$(UQ_DIR)/menu.c
	$(DO_CC)

$(BUILD_DIR)/soft_uq_obj/model.o:	$(UQ_DIR)/model.c
	$(DO_CC)

$(BUILD_DIR)/soft_uq_obj/net_dgrm.o: $(UQ_DIR)/net_dgrm.c
	$(DO_CC)

$(BUILD_DIR)/soft_uq_obj/net_loop.o: $(UQ_DIR)/net_loop.c
	$(DO_CC)

$(BUILD_DIR)/soft_uq_obj/net_main.o: $(UQ_DIR)/net_main.c
	$(DO_CC)

$(BUILD_DIR)/soft_uq_obj/net_vcr.o:	$(UQ_DIR)/net_vcr.c
	$(DO_CC)

$(BUILD_DIR)/soft_uq_obj/net_udp.o:	$(UQ_DIR)/net_udp.c
	$(DO_CC)

$(BUILD_DIR)/soft_uq_obj/net_bsd.o:	$(UQ_DIR)/net_bsd.c
	$(DO_CC)

$(BUILD_DIR)/soft_uq_obj/nonintel.o:	$(UQ_DIR)/nonintel.c
	$(DO_CC)

$(BUILD_DIR)/soft_uq_obj/pr_cmds.o :  $(UQ_DIR)/pr_cmds.c
	$(DO_CC)

$(BUILD_DIR)/soft_uq_obj/pr_edict.o : $(UQ_DIR)/pr_edict.c
	$(DO_CC)

$(BUILD_DIR)/soft_uq_obj/pr_exec.o :  $(UQ_DIR)/pr_exec.c
	$(DO_CC)

$(BUILD_DIR)/soft_uq_obj/r_aclip.o:	$(UQ_DIR)/r_aclip.c
	$(DO_CC)

$(BUILD_DIR)/soft_uq_obj/r_alias.o:	$(UQ_DIR)/r_alias.c
	$(DO_CC)

$(BUILD_DIR)/soft_uq_obj/r_bsp.o:	$(UQ_DIR)/r_bsp.c
	$(DO_CC)

$(BUILD_DIR)/soft_uq_obj/r_draw.o:	$(UQ_DIR)/r_draw.c
	$(DO_CC)

$(BUILD_DIR)/soft_uq_obj/r_edge.o:	$(UQ_DIR)/r_edge.c
	$(DO_CC)

$(BUILD_DIR)/soft_uq_obj/r_efrag.o:	$(UQ_DIR)/r_efrag.c
	$(DO_CC)

$(BUILD_DIR)/soft_uq_obj/r_light.o:	$(UQ_DIR)/r_light.c
	$(DO_CC)

$(BUILD_DIR)/soft_uq_obj/r_main.o:	$(UQ_DIR)/r_main.c
	$(DO_CC)

$(BUILD_DIR)/soft_uq_obj/r_misc.o:	$(UQ_DIR)/r_misc.c
	$(DO_CC)

$(BUILD_DIR)/soft_uq_obj/r_part.o:	$(UQ_DIR)/r_part.c
	$(DO_CC)

$(BUILD_DIR)/soft_uq_obj/r_sky.o:	$(UQ_DIR)/r_sky.c
	$(DO_CC)

$(BUILD_DIR)/soft_uq_obj/r_sprite.o:	$(UQ_DIR)/r_sprite.c
	$(DO_CC)

$(BUILD_DIR)/soft_uq_obj/r_surf.o:	$(UQ_DIR)/r_surf.c
	$(DO_CC)

$(BUILD_DIR)/soft_uq_obj/r_vars.o:	$(UQ_DIR)/r_vars.c
	$(DO_CC)

$(BUILD_DIR)/soft_uq_obj/sbar.o:	$(UQ_DIR)/sbar.c
	$(DO_CC)

$(BUILD_DIR)/soft_uq_obj/screen.o:	$(UQ_DIR)/screen.c
	$(DO_CC)

$(BUILD_DIR)/soft_uq_obj/snd_dma.o:  $(COMMON_DIR)/snd_dma.c
	$(DO_CC)

$(BUILD_DIR)/soft_uq_obj/snd_mem.o:  $(COMMON_DIR)/snd_mem.c
	$(DO_CC)

$(BUILD_DIR)/soft_uq_obj/snd_mix.o:  $(COMMON_DIR)/snd_mix.c
	$(DO_CC)

$(BUILD_DIR)/soft_uq_obj/snd_linux.o: $(COMMON_DIR)/snd_linux.c
	$(DO_CC)

$(BUILD_DIR)/soft_uq_obj/sv_main.o:	$(UQ_DIR)/sv_main.c
	$(DO_CC)

$(BUILD_DIR)/soft_uq_obj/sv_phys.o:	$(UQ_DIR)/sv_phys.c
	$(DO_CC)

$(BUILD_DIR)/soft_uq_obj/sv_move.o:	$(UQ_DIR)/sv_move.c
	$(DO_CC)

$(BUILD_DIR)/soft_uq_obj/sv_user.o:	$(UQ_DIR)/sv_user.c
	$(DO_CC)

$(BUILD_DIR)/soft_uq_obj/view.o:	$(UQ_DIR)/view.c
	$(DO_CC)

$(BUILD_DIR)/soft_uq_obj/wad.o:	$(COMMON_DIR)/wad.c
	$(DO_CC)

$(BUILD_DIR)/soft_uq_obj/world.o:	$(UQ_DIR)/world.c
	$(DO_CC)

$(BUILD_DIR)/soft_uq_obj/zone.o:	$(COMMON_DIR)/zone.c
	$(DO_CC)

$(BUILD_DIR)/soft_uq_obj/cd_linux.o:	$(COMMON_DIR)/cd_linux.c
	$(DO_CC)

$(BUILD_DIR)/soft_uq_obj/sys_linux.o: $(UQ_DIR)/sys_linux.c
	$(DO_CC)

# Assembler code

$(BUILD_DIR)/soft_uq_obj/d_copy.o:	$(UQ_DIR)/d_copy.s
	$(DO_AS)

$(BUILD_DIR)/soft_uq_obj/d_draw.o:	$(UQ_DIR)/d_draw.s
	$(DO_AS)

$(BUILD_DIR)/soft_uq_obj/d_draw16.o:	$(UQ_DIR)/d_draw16.s
	$(DO_AS)

$(BUILD_DIR)/soft_uq_obj/d_parta.o:	$(UQ_DIR)/d_parta.s
	$(DO_AS)

$(BUILD_DIR)/soft_uq_obj/d_polysa.o:	$(UQ_DIR)/d_polysa.s
	$(DO_AS)

$(BUILD_DIR)/soft_uq_obj/d_scana.o:	$(UQ_DIR)/d_scana.s
	$(DO_AS)

$(BUILD_DIR)/soft_uq_obj/d_spr8.o:	$(UQ_DIR)/d_spr8.s
	$(DO_AS)

$(BUILD_DIR)/soft_uq_obj/d_varsa.o:	$(UQ_DIR)/d_varsa.s
	$(DO_AS)

$(BUILD_DIR)/soft_uq_obj/math.o:	$(COMMON_DIR)/math.s
	$(DO_AS)

$(BUILD_DIR)/soft_uq_obj/r_aclipa.o:	$(UQ_DIR)/r_aclipa.s
	$(DO_AS)

$(BUILD_DIR)/soft_uq_obj/r_aliasa.o:	$(UQ_DIR)/r_aliasa.s
	$(DO_AS)

$(BUILD_DIR)/soft_uq_obj/r_drawa.o:	$(UQ_DIR)/r_drawa.s
	$(DO_AS)

$(BUILD_DIR)/soft_uq_obj/r_edgea.o:	$(UQ_DIR)/r_edgea.s
	$(DO_AS)

$(BUILD_DIR)/soft_uq_obj/r_varsa.o:	$(UQ_DIR)/r_varsa.s
	$(DO_AS)

$(BUILD_DIR)/soft_uq_obj/surf16.o:	$(UQ_DIR)/surf16.s
	$(DO_AS)

$(BUILD_DIR)/soft_uq_obj/surf8.o:	$(UQ_DIR)/surf8.s
	$(DO_AS)

$(BUILD_DIR)/soft_uq_obj/snd_mixa.o:	$(COMMON_DIR)/snd_mixa.s
	$(DO_AS)

$(BUILD_DIR)/soft_uq_obj/sys_dosa.o:	$(UQ_DIR)/sys_dosa.s
	$(DO_AS)

$(BUILD_DIR)/soft_uq_obj/worlda.o:	$(UQ_DIR)/worlda.s
	$(DO_AS)

# specific things

$(BUILD_DIR)/soft_uq_obj/vid_svgalib.o: $(COMMON_DIR)/vid_svgalib.c
	$(DO_O_CC)

$(BUILD_DIR)/soft_uq_obj/vid_x.o:	$(COMMON_DIR)/vid_x.c
	$(DO_CC)


# ---------------------------------------------------------------------------
# GL Stuff..
# ---------------------------------------------------------------------------

GL_OBJS =\
	$(BUILD_DIR)/gl_uq_obj/cl_demo.o \
	$(BUILD_DIR)/gl_uq_obj/cl_input.o \
	$(BUILD_DIR)/gl_uq_obj/cl_main.o \
	$(BUILD_DIR)/gl_uq_obj/cl_parse.o \
	$(BUILD_DIR)/gl_uq_obj/cl_tent.o \
	$(BUILD_DIR)/gl_uq_obj/chase.o \
	$(BUILD_DIR)/gl_uq_obj/cmd.o \
	$(BUILD_DIR)/gl_uq_obj/common.o \
	$(BUILD_DIR)/gl_uq_obj/console.o \
	$(BUILD_DIR)/gl_uq_obj/crc.o \
	$(BUILD_DIR)/gl_uq_obj/cvar.o \
	\
	$(BUILD_DIR)/gl_uq_obj/gl_draw.o \
	$(BUILD_DIR)/gl_uq_obj/gl_mesh.o \
	$(BUILD_DIR)/gl_uq_obj/gl_model.o \
	$(BUILD_DIR)/gl_uq_obj/gl_refrag.o \
	$(BUILD_DIR)/gl_uq_obj/gl_rlight.o \
	$(BUILD_DIR)/gl_uq_obj/gl_rmain.o \
	$(BUILD_DIR)/gl_uq_obj/gl_rmisc.o \
	$(BUILD_DIR)/gl_uq_obj/gl_rsurf.o \
	$(BUILD_DIR)/gl_uq_obj/gl_screen.o \
	$(BUILD_DIR)/gl_uq_obj/gl_test.o \
	$(BUILD_DIR)/gl_uq_obj/gl_warp.o \
	\
	$(BUILD_DIR)/gl_uq_obj/host.o \
	$(BUILD_DIR)/gl_uq_obj/host_cmd.o \
	$(BUILD_DIR)/gl_uq_obj/keys.o \
	$(BUILD_DIR)/gl_uq_obj/menu.o \
	$(BUILD_DIR)/gl_uq_obj/mathlib.o \
	$(BUILD_DIR)/gl_uq_obj/net_dgrm.o \
	$(BUILD_DIR)/gl_uq_obj/net_loop.o \
	$(BUILD_DIR)/gl_uq_obj/net_main.o \
	$(BUILD_DIR)/gl_uq_obj/net_vcr.o \
	$(BUILD_DIR)/gl_uq_obj/net_udp.o \
	$(BUILD_DIR)/gl_uq_obj/net_bsd.o \
	$(BUILD_DIR)/gl_uq_obj/pr_cmds.o \
	$(BUILD_DIR)/gl_uq_obj/pr_edict.o \
	$(BUILD_DIR)/gl_uq_obj/pr_exec.o \
	$(BUILD_DIR)/gl_uq_obj/r_part.o \
	$(BUILD_DIR)/gl_uq_obj/sbar.o \
	$(BUILD_DIR)/gl_uq_obj/sv_main.o \
	$(BUILD_DIR)/gl_uq_obj/sv_phys.o \
	$(BUILD_DIR)/gl_uq_obj/sv_move.o \
	$(BUILD_DIR)/gl_uq_obj/sv_user.o \
	$(BUILD_DIR)/gl_uq_obj/zone.o   \
	$(BUILD_DIR)/gl_uq_obj/view.o   \
	$(BUILD_DIR)/gl_uq_obj/wad.o \
	$(BUILD_DIR)/gl_uq_obj/world.o \
	$(BUILD_DIR)/gl_uq_obj/cd_linux.o \
	$(BUILD_DIR)/gl_uq_obj/sys_linux.o \
	$(BUILD_DIR)/gl_uq_obj/snd_dma.o \
	$(BUILD_DIR)/gl_uq_obj/snd_mem.o \
	$(BUILD_DIR)/gl_uq_obj/snd_mix.o \
	$(BUILD_DIR)/gl_uq_obj/snd_linux.o

GL_AS_OBJS =\
	$(BUILD_DIR)/gl_uq_obj/math.o \
	$(BUILD_DIR)/gl_uq_obj/worlda.o \
	$(BUILD_DIR)/gl_uq_obj/snd_mixa.o \
	$(BUILD_DIR)/gl_uq_obj/sys_dosa.o

GL_GLX_OBJS = $(BUILD_DIR)/gl_uq_obj/gl_vidlinuxglx.o
GL_GLIDE_OBJS = $(BUILD_DIR)/gl_uq_obj/gl_vidlinux.o

$(BUILD_DIR)/quake-glx:	$(GL_OBJS) $(GL_AS_OBJS) $(GL_GLX_OBJS)
	$(CC) $(CFLAGS) -o $@ $(GL_OBJS) $(GL_AS_OBJS) $(GL_GLX_OBJS) $(GL_GLX_LDFLAGS)

$(BUILD_DIR)/quake-glide:	$(GL_OBJS) $(GL_AS_OBJS) $(GL_GLIDE_OBJS)
	$(CC) $(CFLAGS) -o $@ $(GL_OBJS) $(GL_AS_OBJS) $(GL_GLIDE_OBJS) $(GL_GLIDE_LDFLAGS)

$(BUILD_DIR)/gl_uq_obj/cl_demo.o :      $(UQ_DIR)/cl_demo.c
	$(DO_GL_CC)

$(BUILD_DIR)/gl_uq_obj/cl_input.o :     $(UQ_DIR)/cl_input.c
	$(DO_GL_CC)

$(BUILD_DIR)/gl_uq_obj/cl_main.o :      $(UQ_DIR)/cl_main.c
	$(DO_GL_CC)

$(BUILD_DIR)/gl_uq_obj/cl_parse.o :     $(UQ_DIR)/cl_parse.c
	$(DO_GL_CC)

$(BUILD_DIR)/gl_uq_obj/cl_tent.o :      $(UQ_DIR)/cl_tent.c
	$(DO_GL_CC)

$(BUILD_DIR)/gl_uq_obj/chase.o :        $(UQ_DIR)/chase.c
	$(DO_GL_CC)

$(BUILD_DIR)/gl_uq_obj/cmd.o :          $(UQ_DIR)/cmd.c
	$(DO_GL_CC)

$(BUILD_DIR)/gl_uq_obj/common.o :       $(UQ_DIR)/common.c
	$(DO_GL_CC)

$(BUILD_DIR)/gl_uq_obj/console.o :      $(UQ_DIR)/console.c
	$(DO_GL_CC)

$(BUILD_DIR)/gl_uq_obj/crc.o :          $(UQ_DIR)/crc.c
	$(DO_GL_CC)

$(BUILD_DIR)/gl_uq_obj/cvar.o :         $(UQ_DIR)/cvar.c
	$(DO_GL_CC)

$(BUILD_DIR)/gl_uq_obj/gl_draw.o :      $(UQ_DIR)/gl_draw.c
	$(DO_GL_CC)

$(BUILD_DIR)/gl_uq_obj/gl_mesh.o :      $(UQ_DIR)/gl_mesh.c
	$(DO_GL_CC)

$(BUILD_DIR)/gl_uq_obj/gl_model.o :     $(UQ_DIR)/gl_model.c
	$(DO_GL_CC)

$(BUILD_DIR)/gl_uq_obj/gl_refrag.o :    $(UQ_DIR)/gl_refrag.c
	$(DO_GL_CC)

$(BUILD_DIR)/gl_uq_obj/gl_rlight.o :    $(UQ_DIR)/gl_rlight.c
	$(DO_GL_CC)

$(BUILD_DIR)/gl_uq_obj/gl_rmain.o :     $(UQ_DIR)/gl_rmain.c
	$(DO_GL_CC)

$(BUILD_DIR)/gl_uq_obj/gl_rmisc.o :     $(UQ_DIR)/gl_rmisc.c
	$(DO_GL_CC)

$(BUILD_DIR)/gl_uq_obj/gl_rsurf.o :     $(UQ_DIR)/gl_rsurf.c
	$(DO_GL_CC)

$(BUILD_DIR)/gl_uq_obj/gl_screen.o :    $(UQ_DIR)/gl_screen.c
	$(DO_GL_CC)

$(BUILD_DIR)/gl_uq_obj/gl_test.o :      $(UQ_DIR)/gl_test.c
	$(DO_GL_CC)

$(BUILD_DIR)/gl_uq_obj/gl_vidlinux.o :  $(UQ_DIR)/gl_vidlinux.c
	$(DO_GL_CC)

$(BUILD_DIR)/gl_uq_obj/gl_vidlinuxglx.o :  $(COMMON_DIR)/gl_vidlinuxglx.c
	$(DO_GL_CC)

$(BUILD_DIR)/gl_uq_obj/gl_warp.o :      $(UQ_DIR)/gl_warp.c
	$(DO_GL_CC)

$(BUILD_DIR)/gl_uq_obj/host.o :         $(UQ_DIR)/host.c
	$(DO_GL_CC)

$(BUILD_DIR)/gl_uq_obj/host_cmd.o :     $(UQ_DIR)/host_cmd.c
	$(DO_GL_CC)

$(BUILD_DIR)/gl_uq_obj/keys.o :         $(UQ_DIR)/keys.c
	$(DO_GL_CC)

$(BUILD_DIR)/gl_uq_obj/menu.o :         $(UQ_DIR)/menu.c
	$(DO_GL_CC)

$(BUILD_DIR)/gl_uq_obj/mathlib.o :      $(COMMON_DIR)/mathlib.c
	$(DO_GL_CC)

$(BUILD_DIR)/gl_uq_obj/net_dgrm.o :     $(UQ_DIR)/net_dgrm.c
	$(DO_GL_CC)

$(BUILD_DIR)/gl_uq_obj/net_loop.o :     $(UQ_DIR)/net_loop.c
	$(DO_GL_CC)

$(BUILD_DIR)/gl_uq_obj/net_main.o :     $(UQ_DIR)/net_main.c
	$(DO_GL_CC)

$(BUILD_DIR)/gl_uq_obj/net_vcr.o :      $(UQ_DIR)/net_vcr.c
	$(DO_GL_CC)

$(BUILD_DIR)/gl_uq_obj/net_udp.o :      $(UQ_DIR)/net_udp.c
	$(DO_GL_CC)

$(BUILD_DIR)/gl_uq_obj/net_bsd.o :      $(UQ_DIR)/net_bsd.c
	$(DO_GL_CC)

$(BUILD_DIR)/gl_uq_obj/pr_cmds.o :      $(UQ_DIR)/pr_cmds.c
	$(DO_GL_CC)

$(BUILD_DIR)/gl_uq_obj/pr_edict.o :     $(UQ_DIR)/pr_edict.c
	$(DO_GL_CC)

$(BUILD_DIR)/gl_uq_obj/pr_exec.o :      $(UQ_DIR)/pr_exec.c
	$(DO_GL_CC)

$(BUILD_DIR)/gl_uq_obj/r_part.o :       $(UQ_DIR)/r_part.c
	$(DO_GL_CC)

$(BUILD_DIR)/gl_uq_obj/sbar.o :         $(UQ_DIR)/sbar.c
	$(DO_GL_CC)

$(BUILD_DIR)/gl_uq_obj/sv_main.o :      $(UQ_DIR)/sv_main.c
	$(DO_GL_CC)

$(BUILD_DIR)/gl_uq_obj/sv_phys.o :      $(UQ_DIR)/sv_phys.c
	$(DO_GL_CC)

$(BUILD_DIR)/gl_uq_obj/sv_move.o :      $(UQ_DIR)/sv_move.c
	$(DO_GL_CC)

$(BUILD_DIR)/gl_uq_obj/sv_user.o :      $(UQ_DIR)/sv_user.c
	$(DO_GL_CC)

$(BUILD_DIR)/gl_uq_obj/zone.o :        $(COMMON_DIR)/zone.c
	$(DO_GL_CC)

$(BUILD_DIR)/gl_uq_obj/view.o :        $(UQ_DIR)/view.c
	$(DO_GL_CC)

$(BUILD_DIR)/gl_uq_obj/wad.o :          $(COMMON_DIR)/wad.c
	$(DO_GL_CC)

$(BUILD_DIR)/gl_uq_obj/world.o :        $(UQ_DIR)/world.c
	$(DO_GL_CC)

$(BUILD_DIR)/gl_uq_obj/cd_linux.o :     $(COMMON_DIR)/cd_linux.c
	$(DO_GL_CC)

$(BUILD_DIR)/gl_uq_obj/sys_linux.o :    $(UQ_DIR)/sys_linux.c
	$(DO_GL_CC)

$(BUILD_DIR)/gl_uq_obj/snd_dma.o :      $(COMMON_DIR)/snd_dma.c
	$(DO_GL_CC)

$(BUILD_DIR)/gl_uq_obj/snd_mem.o :      $(COMMON_DIR)/snd_mem.c
	$(DO_GL_CC)

$(BUILD_DIR)/gl_uq_obj/snd_mix.o :      $(COMMON_DIR)/snd_mix.c
	$(DO_GL_CC)

$(BUILD_DIR)/gl_uq_obj/snd_linux.o :    $(COMMON_DIR)/snd_linux.c
	$(DO_GL_CC)

$(BUILD_DIR)/gl_uq_obj/math.o :         $(COMMON_DIR)/math.s
	$(DO_GL_AS)

$(BUILD_DIR)/gl_uq_obj/worlda.o :       $(UQ_DIR)/worlda.s
	$(DO_GL_AS)

$(BUILD_DIR)/gl_uq_obj/snd_mixa.o :     $(COMMON_DIR)/snd_mixa.s
	$(DO_GL_AS)

$(BUILD_DIR)/gl_uq_obj/sys_dosa.o :     $(UQ_DIR)/sys_dosa.s
	$(DO_GL_AS)

#############################################################################
# MISC
#############################################################################

clean:  clean-soft clean-gl
	@-rmdir $(BUILD_DIR)
	@echo Done.

clean-soft:
	@-rm -f $(SOFT_TARGETS)
	@-rm -f $(SOFT_OBJS)
	@-rmdir $(BUILD_DIR)/soft_uq_obj

clean-gl:
	@-rm -f $(GL_TARGETS)
	@-rm -f $(GL_OBJS)
	@-rmdir $(BUILD_DIR)/gl_uq_obj

