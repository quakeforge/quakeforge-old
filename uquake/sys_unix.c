/*
Copyright (C) 1996-1997 Id Software, Inc.

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
// sys_unix.c -- Unix system driver

#include "quakedef.h"
#include "sys.h"
#include "qargs.h"

#include <stdio.h>
#include <errno.h>
#include <fcntl.h>
#include <stddef.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <sys/param.h>

#ifndef MAP_FAILED
# define MAP_FAILED ((void*)-1)
#endif

#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif

#ifdef HAVE_STRINGS_H
#include <strings.h>
#endif

qboolean			isDedicated;

/*
===============================================================================

FILE IO

===============================================================================
*/

#define MAX_HANDLES             10

typedef struct 
{
    QFile	    *hFile;
    char	    *pMap;
    int		    nLen;
    int		    nPos;
} MEMFILE;

MEMFILE		    sys_handles[MAX_HANDLES];

int findhandle (void)
{
    int             i;
    
    for (i=1 ; i<MAX_HANDLES ; i++)
	    if (!sys_handles[i].hFile)
		    return i;
    Sys_Error ("out of handles");
    return -1;
}

/*
================
filelength
================
*/
int filelength (QFile *f)
{
    int             pos;
    int             end;

    pos = Qtell (f);
    Qseek (f, 0, SEEK_END);
    end = Qtell (f);
    Qseek (f, pos, SEEK_SET);

    return end;
}

/* from sys_linux.c */
int Sys_FileOpenRead (char *path, int *handle)
{
	int	h;
	struct stat	fileinfo;

	h = open (path, O_RDONLY, 0666);
	*handle = h;
	if (h == -1)
		return -1;

	if (fstat (h,&fileinfo) == -1)
		Sys_Error ("Error fstating %s", path);

	return fileinfo.st_size;
}

int Sys_FileOpenWrite (char *path)
{
    QFile    *f;
    int             i;
    
    i = findhandle ();

    f = Qopen(path, "wb");
    if (!f)
	Sys_Error ("Error opening %s: %s", path,strerror(errno));
    sys_handles[i].hFile = f;
    sys_handles[i].nLen = 0;
    sys_handles[i].nPos = 0;
    sys_handles[i].pMap = NULL;
    
    return i;
}

void Sys_FileClose (int handle)
{
    if (sys_handles[handle].pMap)
	if (munmap( sys_handles[handle].pMap, sys_handles[handle].nLen ) != 0)
	    printf( "failed to unmap handle %d\n", handle );

    Qclose (sys_handles[handle].hFile);
    sys_handles[handle].hFile = NULL;
}

void Sys_FileSeek (int handle, int position)
{
    if (sys_handles[handle].pMap)
    {
	sys_handles[handle].nPos = position;
    }
    else Qseek (sys_handles[handle].hFile, position, SEEK_SET);
}

int Sys_FileRead (int handle, void *dest, int count)
{
    if (sys_handles[handle].pMap)
    {
	int nPos = sys_handles[handle].nPos;
	if (count < 0) count = 0;
	if (nPos + count > sys_handles[handle].nLen)
	    count = sys_handles[handle].nLen - nPos;
	memcpy( dest, &sys_handles[handle].pMap[nPos], count );
	sys_handles[handle].nPos = nPos + count;
	return( count );
    }
    else return Qread (sys_handles[handle].hFile, dest, count);
}

int Sys_FileWrite (int handle, void *data, int count)
{
    if (sys_handles[handle].pMap)
	Sys_Error( "Attempted to write to read-only file %d!\n", handle );
    return Qwrite (sys_handles[handle].hFile, data, count);
}

void Sys_DebugLog(char *file, char *fmt, ...) {

	va_list argptr; 
	static char data[1024];
	QFile *stream;
	unsigned char *p;
	//int fd;
    
	va_start(argptr, fmt);
	vsnprintf(data, sizeof(data), fmt, argptr);
	va_end(argptr);
// fd = open(file, O_WRONLY | O_BINARY | O_CREAT | O_APPEND, 0666);
	stream = Qopen(file, "a");
	for (p = (unsigned char *) data; *p; p++) {
	    Qputc(stream, trans_table[*p]);
	}
	Qclose(stream);
	/*
	fd = open(file, O_WRONLY | O_CREAT | O_APPEND, 0666);
	write(fd, data, strlen(data));
	close(fd);
	*/
}


/*
===============================================================================

SYSTEM IO

===============================================================================
*/

void Sys_MakeCodeWriteable (unsigned long startaddr, unsigned long length)
{

	int r;
	unsigned long addr;
	int psize = getpagesize();

	addr = (startaddr & ~(psize-1)) - psize;

//	fprintf(stderr, "writable code %lx(%lx)-%lx, length=%lx\n", startaddr,
//			addr, startaddr+length, length);

	r = mprotect((char*)addr, length + startaddr - addr + psize, 7);

	if (r < 0)
    		Sys_Error("Protection change failed\n");

}


void Sys_Error (char *error, ...)
{
    va_list         argptr;

    printf ("Sys_Error: ");   
    va_start (argptr,error);
    vprintf (error,argptr);
    va_end (argptr);
    printf ("\n");
    Host_Shutdown();
    exit (1);
}

void Sys_Quit (void)
{
    Host_Shutdown();
    exit (0);
}

char *Sys_ConsoleInput (void)
{
    static char	text[256];
    int		len;
    fd_set	readfds;
    int		ready;
    struct timeval timeout;
    
    timeout.tv_sec = 0;
    timeout.tv_usec = 0;
    FD_ZERO(&readfds);
    FD_SET(0, &readfds);
    ready = select(1, &readfds, 0, 0, &timeout);
    
    if (ready>0)
    {
	len = read (0, text, sizeof(text));
	if (len >= 1)
	{
		text[len-1] = 0;	// rip off the /n and terminate
		return text;
	}
    }
    
    return 0;
}

void Sys_Sleep (void)
{
}

#if !id386
void Sys_HighFPPrecision (void)
{
}

void Sys_LowFPPrecision (void)
{
}
#endif

void Sys_Init(void)
{
#if id386
	Sys_SetFPCW();
#endif
}

//=============================================================================

int main (int argc, char **argv)
{
    static quakeparms_t    parms;
    float oldtime, newtime;
    
    parms.memsize = 16*1024*1024;
    parms.membase = malloc (parms.memsize);
    parms.basedir = ".";
    parms.cachedir = NULL;

    signal(SIGFPE, SIG_IGN);

    COM_InitArgv (argc, argv);

    parms.argc = com_argc;
    parms.argv = com_argv;

    printf ("Host_Init\n");
    Host_Init (&parms);

	Sys_Init();

    // unroll the simulation loop to give the video side a chance to see _vid_default_mode
    Host_Frame( 0.1 );
    // FIXME - probably should be deleted - this is just an empty function
//    VID_SetDefaultMode();

    oldtime = Sys_DoubleTime();
    while (1)
    {
		newtime = Sys_DoubleTime();
		Host_Frame (newtime - oldtime);
		oldtime = newtime;
    }
	return 0;
}




