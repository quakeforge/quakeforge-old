#
# QuakeWorld Makefile for unified tree
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

BUILD_DEBUG_DIR=debug-$(ARCH)
BUILD_RELEASE_DIR=release-$(ARCH)

X11_DIR=/usr/X11R6

INCL=-I. -I$(SERVER_DIR) -I$(QW_COMMON_DIR) -I$(COMMON_DIR) -I$(X11_DIR)/include
LIBS=-L$(X11_DIR)/lib

CC=gcc

BASE_CFLAGS=-Wall -DQUAKEWORLD -DSERVERONLY -Dstricmp=strcasecmp $(INCL)
DEBUG_CFLAGS=$(BASE_CFLAGS) -g

ifeq ($(ARCH),i386)
RELEASE_CFLAGS=$(BASE_CFLAGS) -m486 -O6 -ffast-math -funroll-loops \
	-fomit-frame-pointer -fexpensive-optimizations -malign-loops=2 \
	-malign-jumps=2 -malign-functions=2
else
RELEASE_CFLAGS=$(BASE_CFLAGS) -ffast-math -funroll-loops \
	-fomit-frame-pointer -fexpensive-optimizations
endif

LDFLAGS=-lm

DO_CC=$(CC) $(CFLAGS) -o $@ -c $<
DO_O_CC=$(CC) -O $(CFLAGS) -o $@ -c $<
DO_AS=$(CC) $(CFLAGS) -DELF -x assembler-with-cpp -o $@ -c $<

#############################################################################
# SETUP AND BUILD
#############################################################################

TARGETS=$(OBJ_DIR)/qw_server

all:	debug release
.PHONY: all

debug:
	@-mkdir $(BUILD_DEBUG_DIR) \
		$(BUILD_DEBUG_DIR)/obj
	$(MAKE) targets OBJ_DIR=$(BUILD_DEBUG_DIR)/obj CFLAGS="$(DEBUG_CFLAGS)"

release:
	@-mkdir $(BUILD_RELEASE_DIR) \
		$(BUILD_RELEASE_DIR)/obj
	$(MAKE) targets OBJ_DIR=$(BUILD_RELEASE_DIR)/obj CFLAGS="$(RELEASE_CFLAGS)"

targets: $(TARGETS)

#############################################################################
# SERVER
#############################################################################

SERVER_OBJS = \
	 $(OBJ_DIR)/pr_cmds.o \
	 $(OBJ_DIR)/pr_edict.o \
	 $(OBJ_DIR)/pr_exec.o \
	 $(OBJ_DIR)/sv_init.o \
	 $(OBJ_DIR)/sv_main.o \
	 $(OBJ_DIR)/sv_nchan.o \
	 $(OBJ_DIR)/sv_ents.o \
	 $(OBJ_DIR)/sv_send.o \
	 $(OBJ_DIR)/sv_move.o \
	 $(OBJ_DIR)/sv_phys.o \
	 $(OBJ_DIR)/sv_user.o \
	 $(OBJ_DIR)/sv_ccmds.o \
	 $(OBJ_DIR)/world.o \
	 $(OBJ_DIR)/sys_unix.o \
	 $(OBJ_DIR)/model.o \
	 $(OBJ_DIR)/cmd.o \
	 $(OBJ_DIR)/common.o \
	 $(OBJ_DIR)/crc.o \
	 $(OBJ_DIR)/cvar.o \
	 $(OBJ_DIR)/mathlib.o \
	 $(OBJ_DIR)/md4.o \
	 $(OBJ_DIR)/zone.o \
	 $(OBJ_DIR)/pmove.o \
	 $(OBJ_DIR)/pmovetst.o \
	 $(OBJ_DIR)/net_chan.o \
	 $(OBJ_DIR)/net_udp.o 

$(OBJ_DIR)/qw_server:	$(SERVER_OBJS)
	$(CC) $(CFLAGS) -o $@ $(SERVER_OBJS) $(LDFLAGS)

$(OBJ_DIR)/pr_cmds.o:	$(SERVER_DIR)/pr_cmds.c 
	$(DO_CC)

$(OBJ_DIR)/pr_edict.o:	$(SERVER_DIR)/pr_edict.c
	$(DO_CC)

$(OBJ_DIR)/pr_exec.o:	$(SERVER_DIR)/pr_exec.c
	$(DO_CC)

$(OBJ_DIR)/sv_init.o:	$(SERVER_DIR)/sv_init.c
	$(DO_CC)

$(OBJ_DIR)/sv_main.o:	$(SERVER_DIR)/sv_main.c
	$(DO_CC)

$(OBJ_DIR)/sv_nchan.o:	$(SERVER_DIR)/sv_nchan.c
	$(DO_CC)

$(OBJ_DIR)/sv_ents.o:	$(SERVER_DIR)/sv_ents.c
	$(DO_CC)

$(OBJ_DIR)/sv_send.o:	$(SERVER_DIR)/sv_send.c
	$(DO_CC)

$(OBJ_DIR)/sv_move.o:	$(SERVER_DIR)/sv_move.c
	$(DO_CC)

$(OBJ_DIR)/sv_phys.o:	$(SERVER_DIR)/sv_phys.c
	$(DO_CC)

$(OBJ_DIR)/sv_user.o:	$(SERVER_DIR)/sv_user.c
	$(DO_CC)

$(OBJ_DIR)/sv_ccmds.o:	$(SERVER_DIR)/sv_ccmds.c
	$(DO_CC)

$(OBJ_DIR)/world.o:	$(SERVER_DIR)/world.c
	$(DO_CC)

$(OBJ_DIR)/sys_unix.o:	$(SERVER_DIR)/sys_unix.c
	$(DO_CC)

$(OBJ_DIR)/model.o:	$(SERVER_DIR)/model.c
	$(DO_CC)

$(OBJ_DIR)/crc.o:	$(COMMON_DIR)/crc.c
	$(DO_CC)

$(OBJ_DIR)/mathlib.o:	$(COMMON_DIR)/mathlib.c
	$(DO_CC)

$(OBJ_DIR)/zone.o:	$(COMMON_DIR)/zone.c
	$(DO_CC)

$(OBJ_DIR)/cmd.o:	$(QW_COMMON_DIR)/cmd.c
	$(DO_CC)

$(OBJ_DIR)/common.o:	$(QW_COMMON_DIR)/common.c
	$(DO_CC)

$(OBJ_DIR)/cvar.o:	$(QW_COMMON_DIR)/cvar.c
	$(DO_CC)

$(OBJ_DIR)/md4.o:	$(QW_COMMON_DIR)/md4.c
	$(DO_CC)

$(OBJ_DIR)/net_chan.o:	$(QW_COMMON_DIR)/net_chan.c
	$(DO_CC)

$(OBJ_DIR)/net_udp.o:	$(QW_COMMON_DIR)/net_udp.c
	$(DO_CC)

$(OBJ_DIR)/pmove.o:	$(QW_COMMON_DIR)/pmove.c
	$(DO_CC)

$(OBJ_DIR)/pmovetst.o:	$(QW_COMMON_DIR)/pmovetst.c
	$(DO_CC)

#############################################################################
# MISC
#############################################################################

clean: clean-debug clean-release
	@echo Done.

clean-debug:
	-rm -rf $(BUILD_DEBUG_DIR)

clean-release:
	-rm -rf $(BUILD_RELEASE_DIR)
