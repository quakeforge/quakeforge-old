/*
	cl_slist.h

	serverlist addressbook interface

	Copyright (C) 1999,2000  contributors of the QuakeForge project
	Please see the file "AUTHORS" for a list of contributors

	Author: Brain Koropoff
	Date: 03 May 2000

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

#ifndef _REGISTER_CHECK_H
#define _REGISTER_CHECK_H

extern struct cvar_s *registered;
void register_check ();

#endif // _REGISTER_CHECK_H

#include <quakeio.h>
#define MAX_SERVER_LIST 256

typedef struct {
	char *server;
	char *description;
	int ping;
} server_entry_t;

extern server_entry_t	slist[MAX_SERVER_LIST];

void Server_List_Init(void);
void Server_List_Shutdown(void);
int Server_List_Set(int i,char *addr,char *desc);
int Server_List_Load(QFile *f);
char *gettokstart (char *str, int req, char delim);
int gettoklen(char *str, int req, char delim);
