/*
Copyright (C) 1996-1997 Id Software, Inc.
Copyright (C) 1999,2000 contributors of the QuakeForge project
Portions Copyright (C) 1999,2000  Nelson Rush.
Portions Copyright (C) 2000       Marcus Sundberg [mackan@stacken.kth.se]

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

// net_udp6.c

#include "quakedef.h"
#include <qtypes.h>
#include <lib_replace.h>
#include <sys.h>
#include <cvar.h>
#include <net.h>
#include <console.h>

#include <stdio.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <net/if.h>

/* Sun's model_t in sys/model.h conflicts w/ Quake's model_t */
#define model_t quakeforgemodel_t

#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif
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

#ifndef INADDR_LOOPBACK
#define INADDR_LOOPBACK         (u_int32_t)0x7f000001
#endif

#undef model_t

#ifdef NeXT
#include <libc.h>
#endif

#if defined(sgi) && defined(sa_family)
/* Get rid of problematic SGI #def */
# undef sa_family
#endif

#ifndef MAXHOSTNAMELEN
#define MAXHOSTNAMELEN	512
#endif

extern cvar_t *hostname;

static int net_acceptsocket = -1;		// socket for fielding new connections
static int net_controlsocket;
static int net_broadcastsocket = 0;
static struct qsockaddr broadcastaddr;

static unsigned long myAddr;

#include "net_udp.h"

//=============================================================================

/* get any of my non-loopback addr. */
static int
grab_myaddr(family, sa)
	int family;
	struct sockaddr *sa;
{
	int s;
	unsigned int maxif;
	struct ifreq *iflist;
	struct ifconf ifconf;
	struct ifreq *ifr, *ifr_end;
	// struct myaddrs *p;
	struct sockaddr_in *sin;
	struct sockaddr_in6 *sin6;

#if 0
	maxif = if_maxindex() + 1;
#else
	maxif = 10;
#endif
	iflist = (struct ifreq *)malloc(maxif * BUFSIZ);	/* XXX */
	if (!iflist) {
		Sys_Error("grab_myaddr: not enough core\n");
		/*NOTREACHED*/
	}

	if ((s = socket(PF_INET, SOCK_DGRAM, 0)) < 0) {
		Sys_Error("socket(SOCK_DGRAM)\n");
		/*NOTREACHED*/
	}
	memset(&ifconf, 0, sizeof(ifconf));
	ifconf.ifc_req = iflist;
	ifconf.ifc_len = maxif * BUFSIZ;	/* XXX */
	if (ioctl(s, SIOCGIFCONF, &ifconf) < 0) {
		Sys_Error("ioctl(SIOCGIFCONF)\n");
		/*NOTREACHED*/
	}
	close(s);

	/* Look for this interface in the list */
	ifr_end = (struct ifreq *) (ifconf.ifc_buf + ifconf.ifc_len);
	for (ifr = ifconf.ifc_req;
	     ifr < ifr_end;
#ifdef HAVE_SA_LEN
	     /* Hopefully SA_LEN indicates also FreeBSD ioctl() */
	     ifr = (struct ifreq *) ((char *) &ifr->ifr_addr
				    + ifr->ifr_addr.sa_len)) {
#else
	     ifr++) {
#endif
		switch (ifr->ifr_addr.sa_family) {
		case AF_INET:
			sin = (struct sockaddr_in *)&ifr->ifr_addr;
			if (sin->sin_addr.s_addr == htonl(0))
				continue;
			if (sin->sin_addr.s_addr == htonl(INADDR_LOOPBACK))
				continue;
			break;
		case AF_INET6:
			sin6 = (struct sockaddr_in6 *)&ifr->ifr_addr;
			if (IN6_IS_ADDR_UNSPECIFIED(&sin6->sin6_addr))
				continue;
			if (IN6_IS_ADDR_LOOPBACK(&sin6->sin6_addr))
				continue;
			if (IN6_IS_ADDR_LINKLOCAL(&sin6->sin6_addr))
				continue;
			if (IN6_IS_ADDR_SITELOCAL(&sin6->sin6_addr))
				continue;
			break;
		}
		if (ifr->ifr_addr.sa_family != family)
			continue;

#ifdef HAVE_SA_LEN
		memcpy(sa, &ifr->ifr_addr, ifr->ifr_addr.sa_len);
#else
		memcpy(sa, &ifr->ifr_addr, sizeof(struct sockaddr));
#endif
	    {
		char hbuf[NI_MAXHOST];
#ifdef HAVE_SA_LEN
		if (getnameinfo(sa, sa->sa_len, hbuf, sizeof(hbuf), NULL, 0,
		    NI_NUMERICHOST) == 0)
#else
		if (getnameinfo(sa, sizeof(sa), hbuf, sizeof(hbuf), NULL, 0,
		    NI_NUMERICHOST) == 0)
#endif
			printf("got %s as local address\n", hbuf);
		else {
			printf("non-printable local address, family %d\n",
			    family);
		}
	    }
		break;
	}

	free(iflist);
	return 1;
}

int UDP_Init (void)
{
  //struct hostent *local;
	char	buff[MAXHOSTNAMELEN];
	struct qsockaddr addr;
	char *colon;

	if (COM_CheckParm ("-noudp"))
		return -1;

	// determine my name & address
	memset(&myAddr, 0, sizeof(myAddr));
#if 0
	gethostname(buff, MAXHOSTNAMELEN);
	if ((local = gethostbyname(buff)) == NULL)
		myAddr = INADDR_LOOPBACK;
	else
		myAddr = *(int *)local->h_addr_list[0];
#else
    {
	struct sockaddr_in sin;
	if (grab_myaddr(AF_INET, (struct sockaddr *)&sin) == 0)
		memcpy(&myAddr, &sin.sin_addr, sizeof(myAddr));
    }
#endif

	// if the quake hostname isn't set, set it to the machine name
	if (Q_strcmp(hostname->string, "UNNAMED") == 0)
	{
		buff[15] = 0;
		Cvar_Set (hostname, buff);
	}

	if ((net_controlsocket = UDP_OpenSocket (0)) == -1)
		Sys_Error("UDP_Init: Unable to open control socket\n");

	((struct sockaddr_in *)&broadcastaddr)->sin_family = AF_INET;
	((struct sockaddr_in *)&broadcastaddr)->sin_addr.s_addr = INADDR_BROADCAST;
	((struct sockaddr_in *)&broadcastaddr)->sin_port = htons(net_hostport);

	UDP_GetSocketAddr (net_controlsocket, &addr);
	Q_strcpy(my_tcpip_address,  UDP_AddrToString (&addr));
	colon = Q_strrchr (my_tcpip_address, ':');
	if (colon)
		*colon = 0;

	Con_Printf("UDP Initialized\n");
	tcpipAvailable = true;

	return net_controlsocket;
}

//=============================================================================

void UDP_Shutdown (void)
{
	UDP_Listen (false);
	UDP_CloseSocket (net_controlsocket);
}

//=============================================================================

void UDP_Listen (qboolean state)
{
	// enable listening
	if (state)
	{
		if (net_acceptsocket != -1)
			return;
		if ((net_acceptsocket = UDP_OpenSocket (net_hostport)) == -1)
			Sys_Error ("UDP_Listen: Unable to open accept socket\n");
		return;
	}

	// disable listening
	if (net_acceptsocket == -1)
		return;
	UDP_CloseSocket (net_acceptsocket);
	net_acceptsocket = -1;
}

//=============================================================================

int UDP_OpenSocket (int port)
{
	struct addrinfo hints, *res = NULL;
	int error;
	char pbuf[NI_MAXSERV];
	int newsocket;
	//	struct sockaddr_in address;
	int _true = 1;
#ifdef IPV6_BINDV6ONLY
	const int false = 0;
#endif

//	  if ((newsocket = socket (PF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1)
//		  return -1;

	snprintf(pbuf, sizeof(pbuf), "%d", port);
	memset(&hints, 0, sizeof(hints));
	hints.ai_family = PF_INET6;
	hints.ai_socktype = SOCK_DGRAM;
	hints.ai_flags = AI_PASSIVE;
	error = getaddrinfo(NULL, pbuf, &hints, &res);
	if (error)
	        return -1;

//	  if (ioctl (newsocket, FIONBIO, &_true) == -1)
//		  goto ErrorReturn;
	if ((newsocket = socket (res->ai_family, res->ai_socktype,
	    res->ai_protocol)) < 0)
		return -1;

	if (ioctl (newsocket, FIONBIO, (char *)&_true) < 0)
 		goto ErrorReturn;

//	  address.sin_family = AF_INET;
//	  address.sin_addr.s_addr = INADDR_ANY;
//	  address.sin_port = htons(port);
//	  if( bind (newsocket, (void *)&address, sizeof(address)) == -1)
//		  goto ErrorReturn;
//
#ifdef IPV6_BINDV6ONLY
	if (setsockopt(newsocket, IPPROTO_IPV6, IPV6_BINDV6ONLY, &false,
	    sizeof(false)) < 0) {
		/* I don't care */
	}
#endif

	if (bind (newsocket, res->ai_addr, res->ai_addrlen) < 0)
 		goto ErrorReturn;
 
	freeaddrinfo(res);

	return newsocket;

ErrorReturn:
	close (newsocket);
	return -1;
}

//=============================================================================

int UDP_CloseSocket (int socket)
{
	if (socket == net_broadcastsocket)
		net_broadcastsocket = 0;
	return close (socket);
}


//=============================================================================
/*
============
PartialIPAddress

this lets you type only as much of the net address as required, using
the local network components to fill in the rest
============
*/
static int PartialIPAddress (char *in, struct qsockaddr *hostaddr)
{
	char buff[256];
	char *b;
	int addr;
	int num;
	int mask;
	int run;
	int port;

	buff[0] = '.';
	b = buff;
	strcpy(buff+1, in);
	if (buff[1] == '.')
		b++;

	addr = 0;
	mask=-1;
	while (*b == '.')
	{
		b++;
		num = 0;
		run = 0;
		while (!( *b < '0' || *b > '9'))
		{
		  num = num*10 + *b++ - '0';
		  if (++run > 3)
		  	return -1;
		}
		if ((*b < '0' || *b > '9') && *b != '.' && *b != ':' && *b != 0)
			return -1;
		if (num < 0 || num > 255)
			return -1;
		mask<<=8;
		addr = (addr<<8) + num;
	}

	if (*b++ == ':')
		port = Q_atoi(b);
	else
		port = net_hostport;

	hostaddr->sa_family = AF_INET;
	((struct sockaddr_in *)hostaddr)->sin_port = htons((short)port);
	((struct sockaddr_in *)hostaddr)->sin_addr.s_addr = (myAddr & htonl(mask)) | htonl(addr);

	return 0;
}
//=============================================================================

int UDP_Connect (int socket, struct qsockaddr *addr)
{
	return 0;
}

//=============================================================================

int UDP_CheckNewConnections (void)
{
	unsigned long	available;

	if (net_acceptsocket == -1)
		return -1;

	if (ioctl (net_acceptsocket, FIONREAD, &available) == -1)
		Sys_Error ("UDP: ioctlsocket (FIONREAD) failed\n");
	if (available)
		return net_acceptsocket;
	return -1;
}

//=============================================================================

int UDP_Read (int socket, byte *buf, int len, struct qsockaddr *addr)
{
	socklen_t addrlen = sizeof (struct qsockaddr);
	int ret;

	ret = recvfrom (socket, buf, len, 0, (struct sockaddr *)addr, &addrlen);
	if (ret == -1 && (errno == EWOULDBLOCK || errno == ECONNREFUSED))
		return 0;
	return ret;
}

//=============================================================================

int UDP_MakeSocketBroadcastCapable (int socket)
{
	int				i = 1;

	// make this socket broadcast capable
	if (setsockopt(socket, SOL_SOCKET, SO_BROADCAST, (char *)&i, sizeof(i)) < 0)
		return -1;
	net_broadcastsocket = socket;

	return 0;
}

//=============================================================================

int UDP_Broadcast (int socket, byte *buf, int len)
{
	int ret;

	if (socket != net_broadcastsocket)
	{
		if (net_broadcastsocket != 0)
			Sys_Error("Attempted to use multiple broadcasts sockets\n");
		ret = UDP_MakeSocketBroadcastCapable (socket);
		if (ret == -1)
		{
			Con_Printf("Unable to make socket broadcast capable\n");
			return ret;
		}
	}

	return UDP_Write (socket, buf, len, &broadcastaddr);
}

//=============================================================================

int UDP_Write (int socket, byte *buf, int len, struct qsockaddr *addr)
{
	int ret;

	ret = sendto (socket, buf, len, 0, (struct sockaddr *)addr, sizeof(struct qsockaddr));
	if (ret == -1 && errno == EWOULDBLOCK)
		return 0;
	return ret;
}

//=============================================================================

char *UDP_AddrToString (struct qsockaddr *addr)
{
	static char buffer[22];
	int haddr;

	haddr = ntohl(((struct sockaddr_in *)addr)->sin_addr.s_addr);
	snprintf(buffer, sizeof(buffer), "%d.%d.%d.%d:%d", (haddr >> 24) & 0xff, (haddr >> 16) & 0xff, (haddr >> 8) & 0xff, haddr & 0xff, ntohs(((struct sockaddr_in *)addr)->sin_port));
	return buffer;
}

//=============================================================================

int UDP_StringToAddr (char *string, struct qsockaddr *addr)
{
	int ha1, ha2, ha3, ha4, hp;
	int ipaddr;

	sscanf(string, "%d.%d.%d.%d:%d", &ha1, &ha2, &ha3, &ha4, &hp);
	ipaddr = (ha1 << 24) | (ha2 << 16) | (ha3 << 8) | ha4;

	addr->sa_family = AF_INET;
	((struct sockaddr_in *)addr)->sin_addr.s_addr = htonl(ipaddr);
	((struct sockaddr_in *)addr)->sin_port = htons(hp);
	return 0;
}

//=============================================================================

int UDP_GetSocketAddr (int socket, struct qsockaddr *addr)
{
	socklen_t addrlen = sizeof(struct qsockaddr);
	unsigned int a;

	Q_memset(addr, 0, sizeof(struct qsockaddr));
	getsockname(socket, (struct sockaddr *)addr, &addrlen);
	a = ((struct sockaddr_in *)addr)->sin_addr.s_addr;
	if (a == 0 || a == inet_addr("127.0.0.1"))
		((struct sockaddr_in *)addr)->sin_addr.s_addr = myAddr;

	return 0;
}

//=============================================================================

int UDP_GetNameFromAddr (struct qsockaddr *addr, char *name)
{
	struct hostent *hostentry;

	hostentry = gethostbyaddr ((char *)&((struct sockaddr_in *)addr)->sin_addr, sizeof(struct in_addr), AF_INET);
	if (hostentry)
	{
		Q_strncpy (name, (char *)hostentry->h_name, NET_NAMELEN - 1);
		return 0;
	}

	Q_strcpy (name, UDP_AddrToString (addr));
	return 0;
}

//=============================================================================

int UDP_GetAddrFromName(char *name, struct qsockaddr *addr)
{
	struct hostent *hostentry;

	if (name[0] >= '0' && name[0] <= '9')
		return PartialIPAddress (name, addr);

	hostentry = gethostbyname (name);
	if (!hostentry)
		return -1;

	addr->sa_family = AF_INET;
	((struct sockaddr_in *)addr)->sin_port = htons(net_hostport);
	((struct sockaddr_in *)addr)->sin_addr.s_addr = *(int *)hostentry->h_addr_list[0];

	return 0;
}

//=============================================================================

int UDP_AddrCompare (struct qsockaddr *addr1, struct qsockaddr *addr2)
{
	if (addr1->sa_family != addr2->sa_family)
		return -1;

	if (((struct sockaddr_in *)addr1)->sin_addr.s_addr != ((struct sockaddr_in *)addr2)->sin_addr.s_addr)
		return -1;

	if (((struct sockaddr_in *)addr1)->sin_port != ((struct sockaddr_in *)addr2)->sin_port)
		return 1;

	return 0;
}

//=============================================================================

int UDP_GetSocketPort (struct qsockaddr *addr)
{
	return ntohs(((struct sockaddr_in *)addr)->sin_port);
}


int UDP_SetSocketPort (struct qsockaddr *addr, int port)
{
	((struct sockaddr_in *)addr)->sin_port = htons(port);
	return 0;
}

//=============================================================================
