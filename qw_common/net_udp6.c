/*
Copyright (C) 1996-1997 Id Software, Inc.
Portions Copyright (C) 1999,2000  Nelson Rush.
Portions Copyright (C) 2000      Marcus Sundberg [mackan@stacken.kth.se]

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  

See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

*/
// net_udp.c

#include "quakedef.h"

#include <stdio.h>
#include <errno.h>

/* Sun's model_t in sys/model.h conflicts w/ Quake's model_t */
#define model_t quakeforgemodel_t

#include <unistd.h>
#include <sys/types.h>

#ifdef HAVE_SYS_IOCTL_H
# include <sys/ioctl.h>
#endif
#ifdef HAVE_SYS_SOCKET_H
# include <sys/socket.h>
#endif
#ifdef HAVE_NETINET_IN_H
# include <netinet/in.h>
#endif
#ifdef HAVE_NETDB_H
# include <netdb.h>
#endif
#ifdef HAVE_ARPA_INET_H
# include <arpa/inet.h>
#endif
#ifdef HAVE_SYS_FILIO_H
# include <sys/filio.h>
#endif

#undef model_t

#ifdef _WIN32
# include "winquake.h"
# undef EWOULDBLOCK
# define EWOULDBLOCK	WSAEWOULDBLOCK
#endif

#ifdef NeXT
#include <libc.h>
#endif

#ifndef MAXHOSTNAMELEN
#define MAXHOSTNAMELEN	512
#endif

netadr_t	net_local_adr;

netadr_t	net_from;
sizebuf_t	net_message;
int		net_socket;

#define	MAX_UDP_PACKET	(MAX_MSGLEN*2)
byte		net_message_buffer[MAX_UDP_PACKET];

#ifdef _WIN32
WSADATA		winsockdata;
#endif

//=============================================================================

void NetadrToSockadr (netadr_t *a, struct sockaddr_in6 *s)
{
	memset (s, 0, sizeof(*s));

	s->sin6_family = AF_INET6;

	s->sin6_addr.in6_u.u6_addr32[0] = a->ip[0];
	s->sin6_addr.in6_u.u6_addr32[1] = a->ip[1];
	s->sin6_addr.in6_u.u6_addr32[2] = a->ip[2];
	s->sin6_addr.in6_u.u6_addr32[3] = a->ip[3];
	s->sin6_port = a->port;
}

void SockadrToNetadr (struct sockaddr_in6 *s, netadr_t *a)
{
	a->ip[0] = s->sin6_addr.in6_u.u6_addr32[0];
	a->ip[1] = s->sin6_addr.in6_u.u6_addr32[1];
	a->ip[2] = s->sin6_addr.in6_u.u6_addr32[2];
	a->ip[3] = s->sin6_addr.in6_u.u6_addr32[3];
	a->port = s->sin6_port;
}

qboolean	NET_CompareBaseAdr (netadr_t a, netadr_t b)
{
        if (a.ip[0] == b.ip[0] && a.ip[1] == b.ip[1] && a.ip[2] == b.ip[2] && a.ip[3] == b.ip[3])
		return true;
	return false;
}


qboolean	NET_CompareAdr (netadr_t a, netadr_t b)
{
	if (a.ip[0] == b.ip[0] && a.ip[1] == b.ip[1] && a.ip[2] == b.ip[2] && a.ip[3] == b.ip[3] && a.port == b.port)
		return true;
	return false;
}

char	*NET_AdrToString (netadr_t a)
{
	static	char	s[64];
	unsigned short *addr16=(unsigned short *) &(a.ip);

	snprintf(s, sizeof(s), "%x:%x:%x:%x:%x:%x:%x:%x.%x", ntohs(addr16[0]), ntohs(addr16[1]), ntohs(addr16[2]), ntohs(addr16[3]),ntohs(addr16[4]), ntohs(addr16[5]), ntohs(addr16[6]), ntohs(addr16[7]), ntohs(a.port));

	return s;
}

char	*NET_BaseAdrToString (netadr_t a)
{
	static	char	s[64];
	unsigned short *addr16=(unsigned short *) &(a.ip);
	
	snprintf(s, sizeof(s), "%x:%x:%x:%x:%x:%x:%x:%x", ntohs(addr16[0]), ntohs(addr16[1]), ntohs(addr16[2]), ntohs(addr16[3]),ntohs(addr16[4]), ntohs(addr16[5]), ntohs(addr16[6]), ntohs(addr16[7]));

	return s;
}

/*
=============
NET_StringToAdr

idnewt
idnewt:28000
192.246.40.70
192.246.40.70:28000
=============
*/
qboolean	NET_StringToAdr (char *s, netadr_t *a)
{

	struct addrinfo hints;
	struct addrinfo *resultp;
	char *space;
	char *ports=NULL;
	char copy[128];
	int err;

	bzero(&hints,sizeof(hints));
	hints.ai_socktype=SOCK_DGRAM;
	hints.ai_family=PF_INET6;

	strcpy(copy,s);
	for (space=copy; *space; space++) {
	  if (*space==' ') 
	    {
	      *space=0;
	      ports=space+1;
	    }
	}

	if ((err=getaddrinfo(copy,ports,&hints,&resultp)))
	  {
	    // Error
	    Sys_Error ("NET_StringToAdr: string %s: %s",s, gai_strerror(err));
	    return 0;
	  }
	
	SockadrToNetadr ((struct sockaddr_in6 *) resultp->ai_addr, a);

	return true;
}

// Returns true if we can't bind the address locally--in other words, 
// the IP is NOT one of our interfaces.
qboolean NET_IsClientLegal(netadr_t *adr)
{
#if 0
	struct sockaddr_in sadr;
	int newsocket;

	if (adr->ip[0] == 127)
		return false; // no local connections period

	NetadrToSockadr (adr, &sadr);

	if ((newsocket = socket (PF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1)
		Sys_Error ("NET_IsClientLegal: socket: %s", strerror(errno));

	sadr.sin_port = 0;

	if( bind (newsocket, (void *)&sadr, sizeof(sadr)) == -1) 
	{
		// It is not a local address
		close(newsocket);
		return true;
	}
	close(newsocket);
	return false;
#else
	return true;
#endif
}


//=============================================================================

qboolean NET_GetPacket (void)
{
	int 	ret;
	struct sockaddr_in6	from;
	unsigned int		fromlen;

	fromlen = sizeof(from);
	ret = recvfrom(net_socket, (void*)net_message_buffer, sizeof(net_message_buffer), 0, (struct sockaddr *)&from, &fromlen);
	SockadrToNetadr (&from, &net_from);

	if (ret == -1) {
#ifdef _WIN32
		int err = WSAGetLastError();

		if (err == WSAEMSGSIZE) {
			Con_Printf ("Warning:  Oversize packet from %s\n",
				NET_AdrToString (net_from));
			return false;
		}
#else /* _WIN32 */
		int err = errno;

		if (err == ECONNREFUSED)
			return false;
#endif /* _WIN32 */
		if (err == EWOULDBLOCK)
			return false;
		Sys_Printf ("NET_GetPacket: %s\n", strerror(err));
		return false;
	}

	net_message.cursize = ret;
	if (ret == sizeof(net_message_buffer))	{
		Con_Printf ("Oversize packet from %s\n",
			    NET_AdrToString (net_from));
		return false;
	}

	return ret;
}

//=============================================================================

void NET_SendPacket (int length, void *data, netadr_t to)
{
	int ret;
	struct sockaddr_in6	addr;

	NetadrToSockadr (&to, &addr);

	ret = sendto (net_socket, data, length, 0, (struct sockaddr *)&addr, sizeof(addr) );
	if (ret == -1) {
#ifdef _WIN32
		int err = WSAGetLastError();

#ifndef SERVERONLY
		if (err == WSAEADDRNOTAVAIL)
			Con_DPrintf("NET_SendPacket Warning: %i\n", err);
#endif
#else /* _WIN32 */
		int err = errno;

		if (err == ECONNREFUSED)
			return;
#endif /* _WIN32 */
		if (err == EWOULDBLOCK)
			return;

		Sys_Printf ("NET_SendPacket: %s\n", strerror(err));
	}
}

//=============================================================================

int UDP_OpenSocket (int port)
{
        int err;
	int newsocket;
	struct sockaddr_in6 address;
	struct addrinfo hints;
	struct addrinfo *resultp;
#ifdef _WIN32
#define ioctl ioctlsocket
	unsigned long _true = true;
#else
	int _true = 1;
#endif
	int i;

	if ((newsocket = socket (PF_INET6, SOCK_DGRAM, IPPROTO_UDP)) == -1)
		Sys_Error ("UDP_OpenSocket: socket: %s", strerror(errno));
	if (ioctl (newsocket, FIONBIO, &_true) == -1)
		Sys_Error ("UDP_OpenSocket: ioctl FIONBIO: %s", strerror(errno));
	address.sin6_family = AF_INET6;
//ZOID -- check for interface binding option
	if ((i = COM_CheckParm("-ip")) != 0 && i < com_argc) {

	        bzero(&hints,sizeof(hints));
		hints.ai_socktype=SOCK_DGRAM;
		hints.ai_family=PF_INET6;
		
	        if ((err=getaddrinfo(com_argv[i+1],NULL,&hints,&resultp)))
		  {
		    Sys_Error ("UDP_OpenSocket: addr %s: %s",com_argv[i+1], gai_strerror(err));
		  }
		memcpy((void *) &address,(void *) resultp->ai_addr,sizeof(address));
		// address.sin_addr.s_addr = inet_addr(com_argv[i+1]);

		// Con_Printf("Binding to IP Interface Address of %s\n",
		// inet_ntop(address.sin6_addr));

	} else
		address.sin6_addr = in6addr_any;
	if (port == PORT_ANY)
		address.sin6_port = 0;
	else
		address.sin6_port = htons((unsigned short)port);
	if( bind (newsocket, (void *)&address, sizeof(address)) == -1)
		Sys_Error ("UDP_OpenSocket: bind: %s", strerror(errno));

	return newsocket;
}

void NET_GetLocalAddress (void)
{
	char	buff[MAXHOSTNAMELEN];
	struct sockaddr_in6	address;
	unsigned int		namelen;

	if (gethostname(buff, MAXHOSTNAMELEN) == -1)
	        Sys_Error ("Net_GetLocalAddress: gethostname: %s", strerror(errno));
	buff[MAXHOSTNAMELEN-1] = 0;

	NET_StringToAdr (buff, &net_local_adr);

	namelen = sizeof(address);
	if (getsockname (net_socket, (struct sockaddr *)&address, &namelen) == -1)
		Sys_Error ("NET_GetLocalAddress: getsockname: %s", strerror(errno));
	net_local_adr.port = address.sin6_port;

	Con_Printf("IP address %s\n", NET_AdrToString (net_local_adr) );
}

/*
====================
NET_Init
====================
*/
void NET_Init (int port)
{
#ifdef _WIN32
	WORD	wVersionRequested; 
	int		r;

	wVersionRequested = MAKEWORD(1, 1); 

	r = WSAStartup(MAKEWORD(1, 1), &winsockdata);
	if (r)
		Sys_Error ("Winsock initialization failed.");
#endif /* _WIN32 */

	//
	// open the single socket to be used for all communications
	//
	net_socket = UDP_OpenSocket (port);

	//
	// init the message buffer
	//
	net_message.maxsize = sizeof(net_message_buffer);
	net_message.data = net_message_buffer;

	//
	// determine my name & address
	//
	NET_GetLocalAddress ();

	Con_Printf("UDP Initialized\n");
}

/*
====================
NET_Shutdown
====================
*/
void	NET_Shutdown (void)
{
#ifdef _WIN32
	closesocket(net_socket);
	WSACleanup();
#else
	close(net_socket);
#endif
}
