/*
sys_win.c - Win32 system interface code
Copyright (C) 1996-1997 Id Software, Inc.
Copyright (C) 1999,2000  contributors of the QuakeForge project
Please see the file "AUTHORS" for a list of contributors

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

#include <quakedef.h>
#include <winquake.h>
#include <resource.h>
#include <sys.h>
#include <screen.h>

#include <errno.h>
#include <fcntl.h>
#include <limits.h>
#include <io.h>

#define MINIMUM_WIN_MEMORY	0x0c00000
#define MAXIMUM_WIN_MEMORY	0x1000000

#define PAUSE_SLEEP		50	// sleep time on pause or minimization
#define NOT_FOCUS_SLEEP		20	// sleep time when not focus

int		starttime;
qboolean	ActiveApp, Minimized;
qboolean	WinNT;

HWND		hwnd_dialog;		// startup dialog box

static double		pfreq;
static double		curtime = 0.0;
static double		lastcurtime = 0.0;
static int		lowshift;
static HANDLE		hinput, houtput;

HANDLE			qwclsemaphore;

static HANDLE		tevent;

void Sys_InitFloatTime (void);

void MaskExceptions (void);
void Sys_PopFPCW (void);
void Sys_PushFPCW_SetHigh (void);

void
Sys_DebugLog (char *file, char *fmt, ...)
{
	va_list argptr; 
	static char data[1024];
	int fd;

	va_start(argptr, fmt);
	vsnprintf(data, sizeof(data), fmt, argptr);
	va_end(argptr);
	fd = open(file, O_WRONLY | O_CREAT | O_APPEND, 0666);
	write(fd, data, strlen(data));
	close(fd);
};

/*
===============================================================================

FILE IO

===============================================================================
*/

#define	MAX_HANDLES		10
FILE		*sys_handles[MAX_HANDLES];

/*
	findhandle
*/
int
findhandle (void)
{
	int		i;
	
	for (i=1 ; i<MAX_HANDLES ; i++)
		if (!sys_handles[i])
			return i;
	Sys_Error ("out of handles");
	return -1;
}


/*
	filelength
*/
int
wfilelength (FILE *f)
{
	int		pos;
	int		end;
	int		t;

	t = VID_ForceUnlockedAndReturnState ();

	pos = ftell (f);
	fseek (f, 0, SEEK_END);
	end = ftell (f);
	fseek (f, pos, SEEK_SET);

	VID_ForceLockState (t);

	return end;
}


/*
	Sys_FileOpenRead
*/
int
Sys_FileOpenRead (char *path, int *hndl)
{
	FILE	*f;
	int		i, retval;
	int		t;

	t = VID_ForceUnlockedAndReturnState ();

	i = findhandle ();

	f = fopen(path, "rb");

	if (!f)
	{
		*hndl = -1;
		retval = -1;
	}
	else
	{
		sys_handles[i] = f;
		*hndl = i;
		retval = wfilelength(f);
	}

	VID_ForceLockState (t);

	return retval;
}


/*
	Sys_FileOpenWrite
*/
int
Sys_FileOpenWrite (char *path)
{
	FILE		*f;
	int		i;
	int		t;

	t = VID_ForceUnlockedAndReturnState ();
	
	i = findhandle ();

	f = fopen(path, "wb");
	if (!f)
		Sys_Error ("Error opening %s: %s", path,strerror(errno));
	sys_handles[i] = f;
	
	VID_ForceLockState (t);

	return i;
}


/*
	Sys_FileClose
*/
void
Sys_FileClose (int handle)
{
	int		t;

	t = VID_ForceUnlockedAndReturnState ();
	fclose (sys_handles[handle]);
	sys_handles[handle] = NULL;
	VID_ForceLockState (t);
}


/*
	Sys_FileSeek
*/
void
Sys_FileSeek (int handle, int position)
{
	int		t;

	t = VID_ForceUnlockedAndReturnState ();
	fseek (sys_handles[handle], position, SEEK_SET);
	VID_ForceLockState (t);
}


/*
	Sys_FileRead
*/
int
Sys_FileRead (int handle, void *dest, int count)
{
	int		t, x;

	t = VID_ForceUnlockedAndReturnState ();
	x = fread (dest, 1, count, sys_handles[handle]);
	VID_ForceLockState (t);
	return x;
}

/*
	Sys_FileWrite
*/
int
Sys_FileWrite (int handle, void *data, int count)
{
	int		t, x;

	t = VID_ForceUnlockedAndReturnState ();
	x = fwrite (data, 1, count, sys_handles[handle]);
	VID_ForceLockState (t);
	return x;
}


/*
===============================================================================
SYSTEM IO
===============================================================================

/*
	Sys_MakeCodeWriteable
*/
void
Sys_MakeCodeWriteable (unsigned long startaddr, unsigned long length)
{
	DWORD  flOldProtect;

//@@@ copy on write or just read-write?
	if (!VirtualProtect((LPVOID)startaddr, length, PAGE_READWRITE,
				&flOldProtect))
   		Sys_Error("Protection change failed\n");
}


/*
	Sys_Init
*/
void
Sys_Init (void)
{
	LARGE_INTEGER	PerformanceFreq;
	unsigned int	lowpart, highpart;
	OSVERSIONINFO	vinfo;

#ifndef SERVERONLY
	// allocate a named semaphore on the client so the
	// front end can tell if it is alive

	// mutex will fail if semephore allready exists
	qwclsemaphore = CreateMutex(
			NULL,		// Security attributes
			0,		// owner
			"qwcl");	// Semaphore name
	if (!qwclsemaphore)
		Sys_Error ("QWCL is already running on this system");
	CloseHandle (qwclsemaphore);

	qwclsemaphore = CreateSemaphore(
			NULL,		// Security attributes
			0,		// Initial count
			1,		// Maximum count
			"qwcl");	// Semaphore name
#endif

	MaskExceptions ();
	Sys_SetFPCW ();

#if 0
	if (!QueryPerformanceFrequency (&PerformanceFreq))
		Sys_Error ("No hardware timer available");

// get 32 out of the 64 time bits such that we have around
// 1 microsecond resolution
	lowpart = (unsigned int)PerformanceFreq.LowPart;
	highpart = (unsigned int)PerformanceFreq.HighPart;
	lowshift = 0;

	while (highpart || (lowpart > 2000000.0))
	{
		lowshift++;
		lowpart >>= 1;
		lowpart |= (highpart & 1) << 31;
		highpart >>= 1;
	}

	pfreq = 1.0 / (double)lowpart;

	Sys_InitFloatTime ();
#endif

	// make sure the timer is high precision, otherwise
	// NT gets 18ms resolution
	timeBeginPeriod( 1 );

	vinfo.dwOSVersionInfoSize = sizeof(vinfo);

	if (!GetVersionEx (&vinfo))
		Sys_Error ("Couldn't get OS info");

	if ((vinfo.dwMajorVersion < 4) ||
		(vinfo.dwPlatformId == VER_PLATFORM_WIN32s))
	{
		Sys_Error ("This program requires at least Win95 or NT 4.0");
	}
	
	if (vinfo.dwPlatformId == VER_PLATFORM_WIN32_NT)
		WinNT = true;
	else
		WinNT = false;
}


void Sys_Error (char *error, ...)
{
	va_list		argptr;
	char		text[1024], text2[1024];
	DWORD		dummy;

	Host_Shutdown ();

	va_start (argptr, error);
	vsnprintf (text, sizeof(text), error, argptr);
	va_end (argptr);

	MessageBox(NULL, text, "Error", 0 /* MB_OK */ );

#ifndef SERVERONLY
	CloseHandle (qwclsemaphore);
#endif

	exit (1);
}

void Sys_Quit (void)
{
	VID_ForceUnlockedAndReturnState ();

	Host_Shutdown();
#ifndef SERVERONLY
	if (tevent)
		CloseHandle (tevent);

	if (qwclsemaphore)
		CloseHandle (qwclsemaphore);
#endif

	exit (0);
}


#if 0
/*
================
Sys_DoubleTime
================
*/
double Sys_DoubleTime (void)
{
	static int			sametimecount;
	static unsigned int	oldtime;
	static int			first = 1;
	LARGE_INTEGER		PerformanceCount;
	unsigned int		temp, t2;
	double				time;

	Sys_PushFPCW_SetHigh ();

	QueryPerformanceCounter (&PerformanceCount);

	temp = ((unsigned int)PerformanceCount.LowPart >> lowshift) |
		   ((unsigned int)PerformanceCount.HighPart << (32 - lowshift));

	if (first)
	{
		oldtime = temp;
		first = 0;
	}
	else
	{
	// check for turnover or backward time
		if ((temp <= oldtime) && ((oldtime - temp) < 0x10000000))
		{
			oldtime = temp;	// so we can't get stuck
		}
		else
		{
			t2 = temp - oldtime;

			time = (double)t2 * pfreq;
			oldtime = temp;

			curtime += time;

			if (curtime == lastcurtime)
			{
				sametimecount++;

				if (sametimecount > 100000)
				{
					curtime += 1.0;
					sametimecount = 0;
				}
			}
			else
			{
				sametimecount = 0;
			}

			lastcurtime = curtime;
		}
	}

	Sys_PopFPCW ();

    return curtime;
}

/*
================
Sys_InitFloatTime
================
*/
void Sys_InitFloatTime (void)
{
	int		j;

	Sys_DoubleTime ();

	j = COM_CheckParm("-starttime");

	if (j)
	{
		curtime = (double) (Q_atof(com_argv[j+1]));
	}
	else
	{
		curtime = 0.0;
	}

	lastcurtime = curtime;
}

#endif

/*
	Sys_ConsoleInput
*/
char *
Sys_ConsoleInput (void)
{
	static char	text[256];
	static int	len;
	INPUT_RECORD	recs[1024];
	int		count;
	int		i, dummy;
	int		ch, numread, numevents;
	HANDLE		th;
	char		*clipText, *textCopied;

	for ( ;; )
	{
		if (!GetNumberOfConsoleInputEvents (hinput, &numevents))
			Sys_Error ("Error getting # of console events");

		if (numevents <= 0)
			break;

		if (!ReadConsoleInput(hinput, recs, 1, &numread))
			Sys_Error ("Error reading console input");

		if (numread != 1)
			Sys_Error ("Couldn't read console input");

		if (recs[0].EventType == KEY_EVENT)
		{
			if (!recs[0].Event.KeyEvent.bKeyDown)
			{
				ch = recs[0].Event.KeyEvent.uChar.AsciiChar;

				switch (ch)
				{
					case '\r':
						WriteFile(houtput, "\r\n", 2, 
							&dummy, NULL);	

						if (len)
						{
							text[len] = 0;
							len = 0;
							return text;
						}
						break;

					case '\b':
						WriteFile(houtput, "\b \b", 
								3, &dummy,
								NULL);
						if (len)
						{
							len--;
							putch('\b');
						}
						break;

					default:
						Con_Printf("Stupid: %d\n", recs[0].Event.KeyEvent.dwControlKeyState);
						if (((ch=='V' || ch=='v') && (recs[0].Event.KeyEvent.dwControlKeyState & 
							(LEFT_CTRL_PRESSED | RIGHT_CTRL_PRESSED))) || ((recs[0].Event.KeyEvent.dwControlKeyState 
							& SHIFT_PRESSED) && (recs[0].Event.KeyEvent.wVirtualKeyCode
							==VK_INSERT))) {
							if (OpenClipboard(NULL)) {
								th = GetClipboardData(CF_TEXT);
								if (th) {
									clipText = GlobalLock(th);
									if (clipText) {
										textCopied = malloc(GlobalSize(th)+1);
										strcpy(textCopied, clipText);
/* Substitutes a NULL for every token */strtok(textCopied, "\n\r\b");
										i = strlen(textCopied);
										if (i+len>=256)
											i=256-len;
										if (i>0) {
											textCopied[i]=0;
											text[len]=0;
											strcat(text, textCopied);
											len+=dummy;
											WriteFile(houtput, textCopied, i, &dummy, NULL);
										}
										free(textCopied);
									}
									GlobalUnlock(th);
								}
								CloseClipboard();
							}
						} else if (ch >= ' ')
						{
							WriteFile(houtput, &ch, 1, &dummy, NULL);	
							text[len] = ch;
							len = (len + 1) & 0xff;
						}

						break;

				}
			}
		}
	}

	return NULL;
}

void Sys_Sleep (void)
{
}


void IN_SendKeyEvents (void)
{
    MSG        msg;

	while (PeekMessage (&msg, NULL, 0, 0, PM_NOREMOVE))
	{
	// we always update if there are any event, even if we're paused
		scr_skipupdate = 0;

		if (!GetMessage (&msg, NULL, 0, 0))
			Sys_Quit ();
      	TranslateMessage (&msg);
      	DispatchMessage (&msg);
	}
}



/*
==============================================================================
 WINDOWS CRAP
==============================================================================
*/

/*
	SleepUntilInput
*/
void
SleepUntilInput (int time)
{

	MsgWaitForMultipleObjects(1, &tevent, FALSE, time, QS_ALLINPUT);
}


/*
	WinMain
*/
HINSTANCE	global_hInstance;
int			global_nCmdShow;
char		*argv[MAX_NUM_ARGVS];
static char	*empty_string = "";
HWND		hwnd_dialog;


int WINAPI
WinMain (HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, 
		int nCmdShow)
{
	MSG		msg;
	quakeparms_t	parms;
	double		time, oldtime, newtime;
	MEMORYSTATUS	lpBuffer;
	static	char	cwd[1024];
	int		t;
	RECT		rect;

	/* previous instances do not exist in Win32 */
	if (hPrevInstance)
		return 0;

	global_hInstance = hInstance;
	global_nCmdShow = nCmdShow;

	lpBuffer.dwLength = sizeof(MEMORYSTATUS);
	GlobalMemoryStatus (&lpBuffer);

	if (!GetCurrentDirectory (sizeof(cwd), cwd))
		Sys_Error ("Couldn't determine current directory");

	if (cwd[Q_strlen(cwd)-1] == '/')
		cwd[Q_strlen(cwd)-1] = 0;

	parms.basedir = cwd;
	parms.cachedir = NULL;

	parms.argc = 1;
	argv[0] = empty_string;

	while (*lpCmdLine && (parms.argc < MAX_NUM_ARGVS))
	{
		while (*lpCmdLine && ((*lpCmdLine <= 32) || (*lpCmdLine > 126)))
			lpCmdLine++;

		if (*lpCmdLine)
		{
			argv[parms.argc] = lpCmdLine;
			parms.argc++;

			while (*lpCmdLine && ((*lpCmdLine > 32)
						&& (*lpCmdLine <= 126)))
				lpCmdLine++;

			if (*lpCmdLine)
			{
				*lpCmdLine = 0;
				lpCmdLine++;
			}
			
		}
	}

	parms.argv = argv;

	COM_InitArgv (parms.argc, parms.argv);

	parms.argc = com_argc;
	parms.argv = com_argv;

	hwnd_dialog = CreateDialog(hInstance, MAKEINTRESOURCE(IDD_DIALOG1),
			NULL, NULL);

	if (hwnd_dialog)
	{
		if (GetWindowRect (hwnd_dialog, &rect))
		{
			if (rect.left > (rect.top * 2))
			{
				SetWindowPos (hwnd_dialog, 0,
					(rect.left / 2) - ((rect.right 
							    - rect.left) / 2),
					rect.top, 0, 0,
					SWP_NOZORDER | SWP_NOSIZE);
			}
		}

		ShowWindow (hwnd_dialog, SW_SHOWDEFAULT);
		UpdateWindow (hwnd_dialog);
		SetForegroundWindow (hwnd_dialog);
	}

// take the greater of all the available memory or half the total memory,
// but at least 8 Mb and no more than 16 Mb, unless they explicitly
// request otherwise
	parms.memsize = lpBuffer.dwAvailPhys;

	if (parms.memsize < MINIMUM_WIN_MEMORY)
		parms.memsize = MINIMUM_WIN_MEMORY;

	if (parms.memsize < (lpBuffer.dwTotalPhys >> 1))
		parms.memsize = lpBuffer.dwTotalPhys >> 1;

	if (parms.memsize > MAXIMUM_WIN_MEMORY)
		parms.memsize = MAXIMUM_WIN_MEMORY;

	if (COM_CheckParm ("-heapsize"))
	{
		t = COM_CheckParm("-heapsize") + 1;

		if (t < com_argc)
			parms.memsize = Q_atoi (com_argv[t]) * 1024;
	}

	parms.membase = malloc (parms.memsize);

	if (!parms.membase)
		Sys_Error ("Not enough memory free; check disk space\n");

	tevent = CreateEvent(NULL, FALSE, FALSE, NULL);

	if (!tevent)
		Sys_Error ("Couldn't create event");

	Sys_Init ();

// because sound is off until we become active
	S_BlockSound ();

	Sys_Printf ("Host_Init\n");
	Host_Init (&parms);

	oldtime = Sys_DoubleTime ();

    /* main window message loop */
	while (1)
	{
// yield the CPU for a little while when paused, minimized, or not the focus
/*		if ((cl.paused && (!ActiveApp && !DDActive)) || Minimized || block_drawing)
		{
			SleepUntilInput (PAUSE_SLEEP);
			scr_skipupdate = 1;		// no point in bothering to draw
		}
		else*/ if (!ActiveApp && !DDActive)
		{
			SleepUntilInput (NOT_FOCUS_SLEEP);
		}

		newtime = Sys_DoubleTime ();
		time = newtime - oldtime;
		Host_Frame (time);
		oldtime = newtime;
	}

    /* return success of application */
    return TRUE;
}

/*
	SV_Error

	Sends a datagram to all the clients informing them of the server
	crash, then exits
*/
void
SV_Error (char *error, ...)
{
	va_list		argptr;
	static	char		string[1024];
	static	qboolean inerror = false;

	if (inerror)
		Sys_Error ("SV_Error: recursively entered (%s)", string);

	inerror = true;

	va_start (argptr, error);
	vsnprintf (string, sizeof(string), error, argptr);
	va_end (argptr);

	Con_Printf ("SV_Error: %s\n",string);

	//SV_FinalMessage (va("server crashed: %s\n", string));
		

	Sys_Error ("SV_Error: %s\n",string);
}
