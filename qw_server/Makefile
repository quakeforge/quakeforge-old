#
# QuakeWorld Makefile for unified tree
#

VERSION=2.40

ifneq (,$(findstring alpha,$(shell uname -m)))
ARCH=axp
else
ARCH=$(shell uname -m)
endif

PROJECT_DIR=..

COMMON_DIR=$(PROJECT_DIR)/common
QW_COMMON_DIR=$(PROJECT_DIR)/qw_common
CLIENT_DIR=$(PROJECT_DIR)/qw_client
SERVER_DIR=$(PROJECT_DIR)/qw_server

BUILD_DIR=$(PROJECT_DIR)/build

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
RELEASE_CFLAGS=$(BASE_CFLAGS) -O3 -ffast-math -funroll-loops \
	-fomit-frame-pointer -fexpensive-optimizations
endif

LDFLAGS=-lm

DO_CC=$(CC) $(CFLAGS) -o $@ -c $<
DO_O_CC=$(CC) -O $(CFLAGS) -o $@ -c $<
DO_AS=$(CC) $(CFLAGS) -DELF -x assembler-with-cpp -o $@ -c $<

#############################################################################
# SETUP AND BUILD
#############################################################################

TARGETS=$(BUILD_DIR)/qw_server

all:	debug release
.PHONY: all

debug: $(BUILD_DIR) $(BUILD_DIR)/server_obj
	$(MAKE) targets CFLAGS="$(DEBUG_CFLAGS)"

release: $(BUILD_DIR) $(BUILD_DIR)/server_obj
	$(MAKE) targets CFLAGS="$(RELEASE_CFLAGS)"

targets: $(TARGETS)

$(BUILD_DIR):
	@-mkdir $(BUILD_DIR)

$(BUILD_DIR)/server_obj:
	@-mkdir $(BUILD_DIR)/server_obj

#############################################################################
# SERVER
#############################################################################

SERVER_OBJS = \
	 $(BUILD_DIR)/server_obj/pr_cmds.o \
	 $(BUILD_DIR)/server_obj/pr_edict.o \
	 $(BUILD_DIR)/server_obj/pr_exec.o \
	 $(BUILD_DIR)/server_obj/sv_init.o \
	 $(BUILD_DIR)/server_obj/sv_main.o \
	 $(BUILD_DIR)/server_obj/sv_nchan.o \
	 $(BUILD_DIR)/server_obj/sv_ents.o \
	 $(BUILD_DIR)/server_obj/sv_send.o \
	 $(BUILD_DIR)/server_obj/sv_move.o \
	 $(BUILD_DIR)/server_obj/sv_phys.o \
	 $(BUILD_DIR)/server_obj/sv_user.o \
	 $(BUILD_DIR)/server_obj/sv_ccmds.o \
	 $(BUILD_DIR)/server_obj/world.o \
	 $(BUILD_DIR)/server_obj/sys_unix.o \
	 $(BUILD_DIR)/server_obj/model.o \
	 $(BUILD_DIR)/server_obj/cmd.o \
	 $(BUILD_DIR)/server_obj/common.o \
	 $(BUILD_DIR)/server_obj/crc.o \
	 $(BUILD_DIR)/server_obj/cvar.o \
	 $(BUILD_DIR)/server_obj/mathlib.o \
	 $(BUILD_DIR)/server_obj/md4.o \
	 $(BUILD_DIR)/server_obj/zone.o \
	 $(BUILD_DIR)/server_obj/pmove.o \
	 $(BUILD_DIR)/server_obj/pmovetst.o \
	 $(BUILD_DIR)/server_obj/net_chan.o \
	 $(BUILD_DIR)/server_obj/net_udp.o 

$(BUILD_DIR)/qw_server:	$(SERVER_OBJS)
	$(CC) $(CFLAGS) -o $@ $(SERVER_OBJS) $(LDFLAGS)

$(BUILD_DIR)/server_obj/pr_cmds.o:	$(SERVER_DIR)/pr_cmds.c 
	$(DO_CC)

$(BUILD_DIR)/server_obj/pr_edict.o:	$(SERVER_DIR)/pr_edict.c
	$(DO_CC)

$(BUILD_DIR)/server_obj/pr_exec.o:	$(SERVER_DIR)/pr_exec.c
	$(DO_CC)

$(BUILD_DIR)/server_obj/sv_init.o:	$(SERVER_DIR)/sv_init.c
	$(DO_CC)

$(BUILD_DIR)/server_obj/sv_main.o:	$(SERVER_DIR)/sv_main.c
	$(DO_CC)

$(BUILD_DIR)/server_obj/sv_nchan.o:	$(SERVER_DIR)/sv_nchan.c
	$(DO_CC)

$(BUILD_DIR)/server_obj/sv_ents.o:	$(SERVER_DIR)/sv_ents.c
	$(DO_CC)

$(BUILD_DIR)/server_obj/sv_send.o:	$(SERVER_DIR)/sv_send.c
	$(DO_CC)

$(BUILD_DIR)/server_obj/sv_move.o:	$(SERVER_DIR)/sv_move.c
	$(DO_CC)

$(BUILD_DIR)/server_obj/sv_phys.o:	$(SERVER_DIR)/sv_phys.c
	$(DO_CC)

$(BUILD_DIR)/server_obj/sv_user.o:	$(SERVER_DIR)/sv_user.c
	$(DO_CC)

$(BUILD_DIR)/server_obj/sv_ccmds.o:	$(SERVER_DIR)/sv_ccmds.c
	$(DO_CC)

$(BUILD_DIR)/server_obj/world.o:	$(SERVER_DIR)/world.c
	$(DO_CC)

$(BUILD_DIR)/server_obj/sys_unix.o:	$(SERVER_DIR)/sys_unix.c
	$(DO_CC)

$(BUILD_DIR)/server_obj/model.o:	$(SERVER_DIR)/model.c
	$(DO_CC)

$(BUILD_DIR)/server_obj/crc.o:	$(COMMON_DIR)/crc.c
	$(DO_CC)

$(BUILD_DIR)/server_obj/mathlib.o:	$(COMMON_DIR)/mathlib.c
	$(DO_CC)

$(BUILD_DIR)/server_obj/zone.o:	$(COMMON_DIR)/zone.c
	$(DO_CC)

$(BUILD_DIR)/server_obj/cmd.o:	$(QW_COMMON_DIR)/cmd.c
	$(DO_CC)

$(BUILD_DIR)/server_obj/common.o:	$(QW_COMMON_DIR)/common.c
	$(DO_CC)

$(BUILD_DIR)/server_obj/cvar.o:	$(QW_COMMON_DIR)/cvar.c
	$(DO_CC)

$(BUILD_DIR)/server_obj/md4.o:	$(QW_COMMON_DIR)/md4.c
	$(DO_CC)

$(BUILD_DIR)/server_obj/net_chan.o:	$(QW_COMMON_DIR)/net_chan.c
	$(DO_CC)

$(BUILD_DIR)/server_obj/net_udp.o:	$(QW_COMMON_DIR)/net_udp.c
	$(DO_CC)

$(BUILD_DIR)/server_obj/pmove.o:	$(QW_COMMON_DIR)/pmove.c
	$(DO_CC)

$(BUILD_DIR)/server_obj/pmovetst.o:	$(QW_COMMON_DIR)/pmovetst.c
	$(DO_CC)

#############################################################################
# MISC
#############################################################################

clean:
	-rm -rf $(TARGETS) $(BUILD_DIR)/server_obj
	-rmdir $(BUILD_DIR)
	@echo Done.

