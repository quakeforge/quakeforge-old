/*
Copyright (C) 1996-1997 Id Software, Inc.
Portions Copyright (C) 1999,2000  Nelson Rush.
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
// common.c -- misc functions used in client and server

/*
All of Quake's data access is through a hierchal file system, but the contents of the file system can be transparently merged from several sources.

The "base directory" is the path to the directory holding the quake.exe and all game directories.  The sys_* files pass this to host_init in quakeparms_t->basedir.  This can be overridden with the "-basedir" command line parm to allow code debugging in a different directory.  The base directory is
only used during filesystem initialization.

The "game directory" is the first tree on the search path and directory that all generated files (savegames, screenshots, demos, config files) will be saved to.  This can be overridden with the "-game" command line parameter.  The game directory can never be changed while quake is executing.  This is a precacution against having a malicious server instruct clients to write files over areas they shouldn't.

The "cache directory" is only used during development to save network bandwidth, especially over ISDN / T1 lines.  If there is a cache directory
specified, when a file is found by the normal search path, it will be mirrored
into the cache directory, then opened there.



FIXME:
The file "parms.txt" will be read out of the game directory and appended to the current command line arguments to allow different games to initialize startup parms differently.  This could be used to add a "-sspeed 22050" for the high quality sound edition.  Because they are added at the end, they will not override an explicit setting on the original command line.
	
*/

/*
=============================================================================

QUAKE FILESYSTEM

=============================================================================
*/

/* Begin Generations */
#ifdef _EXPERIMENTAL_
#include "unzip.h"
#endif

typedef unsigned char byte_t;

#ifndef _AIX
typedef unsigned int uint_t;
typedef unsigned short ushort_t;
#endif
/* End Generations */

int     com_filesize;


//
// in memory
//

typedef struct
{
	char    name[MAX_QPATH];
	int             filepos, filelen;
} packfile_t;

typedef struct pack_s
{
	char    filename[MAX_OSPATH];
	int             handle;
	int             numfiles;
	packfile_t      *files;
} pack_t;

//
// on disk
//
typedef struct
{
	char    name[56];
	int             filepos, filelen;
} dpackfile_t;

typedef struct
{
	char    id[4];
	int             dirofs;
	int             dirlen;
} dpackheader_t;

#define MAX_FILES_IN_PACK       2048

char    com_cachedir[MAX_OSPATH];
char    com_gamedir[MAX_OSPATH];

typedef struct searchpath_s
{
	char    filename[MAX_OSPATH];
	pack_t  *pack;          // only one of filename / pack will be used
	struct searchpath_s *next;
} searchpath_t;

searchpath_t    *com_searchpaths;

/*
============
COM_Path_f

============
*/
void COM_Path_f (void)
{
	searchpath_t    *s;
	
	Con_Printf ("Current search path:\n");
	for (s=com_searchpaths ; s ; s=s->next)
	{
		if (s->pack)
		{
			Con_Printf ("%s (%i files)\n", s->pack->filename, s->pack->numfiles);
		}
		else
			Con_Printf ("%s\n", s->filename);
	}
}

/*
============
COM_WriteFile

The filename will be prefixed by the current game directory
============
*/
void COM_WriteFile (char *filename, void *data, int len)
{
	int             handle;
	char    name[MAX_OSPATH];
	
	snprintf(name, sizeof(name), "%s/%s", com_gamedir, filename);

	handle = Sys_FileOpenWrite (name);
	if (handle == -1)
	{
		Sys_Printf ("COM_WriteFile: failed on %s\n", name);
		return;
	}
	
	Sys_Printf ("COM_WriteFile: %s\n", name);
	Sys_FileWrite (handle, data, len);
	Sys_FileClose (handle);
}


/*
============
COM_CreatePath

Only used for CopyFile
============
*/
void    COM_CreatePath (char *path)
{
	char    *ofs;
	
	for (ofs = path+1 ; *ofs ; ofs++)
	{
		if (*ofs == '/')
		{       // create the directory
			*ofs = 0;
			Sys_mkdir (path);
			*ofs = '/';
		}
	}
}


/*
===========
COM_CopyFile

Copies a file over from the net to the local cache, creating any directories
needed.  This is for the convenience of developers using ISDN from home.
===========
*/
void COM_CopyFile (char *netpath, char *cachepath)
{
	int             in, out;
	int             remaining, count;
	char    buf[4096];
	
	remaining = Sys_FileOpenRead (netpath, &in);            
	COM_CreatePath (cachepath);     // create directories up to the cache file
	out = Sys_FileOpenWrite (cachepath);
	
	while (remaining)
	{
		if (remaining < sizeof(buf))
			count = remaining;
		else
			count = sizeof(buf);
		Sys_FileRead (in, buf, count);
		Sys_FileWrite (out, buf, count);
		remaining -= count;
	}

	Sys_FileClose (in);
	Sys_FileClose (out);    
}

/*
===========
COM_FindFile

Finds the file in the search path.
Sets com_filesize and one of handle or file
===========
*/
int COM_FindFile (char *filename, int *handle, FILE **file)
{
	searchpath_t    *search;
	char            netpath[MAX_OSPATH];
	char            cachepath[MAX_OSPATH];
	pack_t          *pak;
	int                     i;
	int                     findtime, cachetime;

	if (file && handle)
		Sys_Error ("COM_FindFile: both handle and file set");
	if (!file && !handle)
		Sys_Error ("COM_FindFile: neither handle or file set");
		
//
// search through the path, one element at a time
//
	search = com_searchpaths;
	if (proghack)
	{	// gross hack to use quake 1 progs with quake 2 maps
		if (!strcmp(filename, "progs.dat"))
			search = search->next;
	}

	for ( ; search ; search = search->next)
	{
	// is the element a pak file?
		if (search->pack)
		{
		// look through all the pak file elements
			pak = search->pack;
			for (i=0 ; i<pak->numfiles ; i++)
				if (!strcmp (pak->files[i].name, filename))
				{       // found it!
					if(developer.value)
						Sys_Printf ("PackFile: %s : %s\n",pak->filename, filename);
					if (handle)
					{
						*handle = pak->handle;
						Sys_FileSeek (pak->handle, pak->files[i].filepos);
					}
					else
					{       // open a new file on the pakfile
						*file = fopen (pak->filename, "rb");
						if (*file)
							fseek (*file, pak->files[i].filepos, SEEK_SET);
					}
					com_filesize = pak->files[i].filelen;
					return com_filesize;
				}
		}
		else
		{               
	// check a file in the directory tree
			snprintf(netpath, sizeof(netpath), "%s/%s",search->filename, filename);
			
			findtime = Sys_FileTime (netpath);
			if (findtime == -1)
				continue;
				
		// see if the file needs to be updated in the cache
			if (!com_cachedir[0])
				strcpy (cachepath, netpath);
			else
			{	
#if defined(_WIN32)
				if ((strlen(netpath) < 2) || (netpath[1] != ':'))
					snprintf(cachepath, sizeof(cachepath),"%s%s", com_cachedir, netpath);
				else
					snprintf(cachepath, sizeof(cachepath),"%s%s", com_cachedir, netpath+2);
#else
				snprintf(cachepath, sizeof(cachepath),"%s%s", com_cachedir, netpath);
#endif

				cachetime = Sys_FileTime (cachepath);
			
				if (cachetime < findtime)
					COM_CopyFile (netpath, cachepath);
				strcpy (netpath, cachepath);
			}	

			if(developer.value)
				Sys_Printf ("FindFile: %s\n",netpath);
			com_filesize = Sys_FileOpenRead (netpath, &i);
			if (handle)
				*handle = i;
			else
			{
				Sys_FileClose (i);
				*file = fopen (netpath, "rb");
			}
			return com_filesize;
		}
		
	}
	
	Sys_Printf ("FindFile: can't find %s\n", filename);
	
	if (handle)
		*handle = -1;
	else
		*file = NULL;
	com_filesize = -1;
	return -1;
}


/*
===========
COM_OpenFile

filename never has a leading slash, but may contain directory walks
returns a handle and a length
it may actually be inside a pak file
===========
*/
int COM_OpenFile (char *filename, int *handle)
{
	return COM_FindFile (filename, handle, NULL);
}

/*
===========
COM_FOpenFile

If the requested file is inside a packfile, a new FILE * will be opened
into the file.
===========
*/
int COM_FOpenFile (char *filename, FILE **file)
{
	return COM_FindFile (filename, NULL, file);
}

/*
============
COM_CloseFile

If it is a pak file handle, don't really close it
============
*/
void COM_CloseFile (int h)
{
	searchpath_t    *s;
	
	for (s = com_searchpaths ; s ; s=s->next)
		if (s->pack && s->pack->handle == h)
			return;
			
	Sys_FileClose (h);
}


/*
============
COM_LoadFile

Filename are reletive to the quake directory.
Allways appends a 0 byte.
============
*/
cache_user_t *loadcache;
byte    *loadbuf;
int             loadsize;
byte *COM_LoadFile (char *path, int usehunk)
{
	int             h;
	byte    *buf;
	char    base[32];
	int             len;

	buf = NULL;     // quiet compiler warning

// look for it in the filesystem or pack files
	len = COM_OpenFile (path, &h);
	if (h == -1)
		return NULL;
	
// extract the filename base name for hunk tag
	COM_FileBase (path, base);
	
	if (usehunk == 1)
		buf = Hunk_AllocName (len+1, base);
	else if (usehunk == 2)
		buf = Hunk_TempAlloc (len+1);
	else if (usehunk == 0)
		buf = Z_Malloc (len+1);
	else if (usehunk == 3)
		buf = Cache_Alloc (loadcache, len+1, base);
	else if (usehunk == 4)
	{
		if (len+1 > loadsize)
			buf = Hunk_TempAlloc (len+1);
		else
			buf = loadbuf;
	}
	else
		Sys_Error ("COM_LoadFile: bad usehunk");

	if (!buf)
		Sys_Error ("COM_LoadFile: not enough space for %s", path);
		
	((byte *)buf)[len] = 0;

	Draw_BeginDisc ();
	Sys_FileRead (h, buf, len);                     
	COM_CloseFile (h);
	Draw_EndDisc ();

	return buf;
}

byte *COM_LoadHunkFile (char *path)
{
	return COM_LoadFile (path, 1);
}

byte *COM_LoadTempFile (char *path)
{
	return COM_LoadFile (path, 2);
}

void COM_LoadCacheFile (char *path, struct cache_user_s *cu)
{
	loadcache = cu;
	COM_LoadFile (path, 3);
}

// uses temp hunk if larger than bufsize
byte *COM_LoadStackFile (char *path, void *buffer, int bufsize)
{
	byte    *buf;
	
	loadbuf = (byte *)buffer;
	loadsize = bufsize;
	buf = COM_LoadFile (path, 4);
	
	return buf;
}

/*
=================
COM_LoadPackFile

Takes an explicit (not game tree related) path to a pak file.

Loads the header and directory, adding the files at the beginning
of the list so they override previous pack files.
=================
*/
pack_t *COM_LoadPackFile (char *packfile)
{
	dpackheader_t   header;
	int                             i;
	packfile_t              *newfiles;
	int                             numpackfiles;
	pack_t                  *pack;
	int                             packhandle;
	dpackfile_t             info[MAX_FILES_IN_PACK];
	unsigned short          crc;

	if (Sys_FileOpenRead (packfile, &packhandle) == -1)
	{
//              Con_Printf ("Couldn't open %s\n", packfile);
		return NULL;
	}
	Sys_FileRead (packhandle, (void *)&header, sizeof(header));
	if (header.id[0] != 'P' || header.id[1] != 'A'
	|| header.id[2] != 'C' || header.id[3] != 'K')
		Sys_Error ("%s is not a packfile", packfile);
	header.dirofs = LittleLong (header.dirofs);
	header.dirlen = LittleLong (header.dirlen);

	numpackfiles = header.dirlen / sizeof(dpackfile_t);

	if (numpackfiles > MAX_FILES_IN_PACK)
		Sys_Error ("%s has %i files", packfile, numpackfiles);

	if (numpackfiles != PAK0_COUNT)
		com_modified = true;    // not the original file

	newfiles = Hunk_AllocName (numpackfiles * sizeof(packfile_t), "packfile");

	Sys_FileSeek (packhandle, header.dirofs);
	Sys_FileRead (packhandle, (void *)info, header.dirlen);

// crc the directory to check for modifications
	CRC_Init (&crc);
	for (i=0 ; i<header.dirlen ; i++)
		CRC_ProcessByte (&crc, ((byte *)info)[i]);
	if (crc != PAK0_CRC)
		com_modified = true;

// parse the directory
	for (i=0 ; i<numpackfiles ; i++)
	{
		strcpy (newfiles[i].name, info[i].name);
		newfiles[i].filepos = LittleLong(info[i].filepos);
		newfiles[i].filelen = LittleLong(info[i].filelen);
	}

	pack = Hunk_Alloc (sizeof (pack_t));
	strcpy (pack->filename, packfile);
	pack->handle = packhandle;
	pack->numfiles = numpackfiles;
	pack->files = newfiles;
	
	Con_Printf ("Added packfile %s (%i files)\n", packfile, numpackfiles);
	return pack;
}

int
COM_pak3_checkfile(unzFile *pak, const char *path)
{
    int status;
        
    status = unzLocateFile(pak, path, 2);
    return (status == UNZ_OK);
}

void
COM_pak3_closepak(unzFile *pak)
{
    if (pak)
        unzClose(pak);
    pak = NULL;
}

void
COM_pak3_close(unzFile *pak)
{
    unzCloseCurrentFile(pak);
}



int
COM_pak3_read(unzFile *pak, void *buf, uint_t size, uint_t nmemb)
{
    int len;
   
    len = unzReadCurrentFile(pak, buf, size * nmemb);
    return len / size;
}

int
COM_pak3_open(unzFile *pak, const char *path)
{
   if (unzLocateFile(pak, path, 2) != UNZ_OK)
       return 0;
   if (unzOpenCurrentFile(pak) != UNZ_OK)
       return 0;
   return 1;
}

uint_t
COM_pak3_getlen(unzFile *pak)
{
    unz_file_info info;
    
    if (unzGetCurrentFileInfo(pak, &info, NULL, 0, NULL, 0, NULL, 0)
        != UNZ_OK)
        return 0;
    return info.uncompressed_size;
}

uint_t
COM_pak3_readfile(unzFile *pak, const char *path, uint_t bufsize, byte_t *buf)
{
    uint_t len;
   
    if (!COM_pak3_open(pak,path))
        return 0;
    
    if ((len = COM_pak3_getlen(pak)) != 0)
    {
        if (COM_pak3_read(pak, (void*)buf, 1, len) != len)
            len = 0;
    }
    COM_pak3_close(pak);
    return len;
}


// Todo: Make This work! :)
#if defined _EXPERIMENTAL_ && GENERATIONS 
pack_t *COM_LoadQ3PackFile (char *packfile)
{

	int                             i;
	packfile_t              	*newfiles;
	float                  		numpackfiles;
	unzFile            		*pak;
	pack_t 				*pack_old;
	int 				status;
//	int                           packhandle;
	dpackfile_t            		info[MAX_FILES_IN_PACK];
//	unz_file_info		fileInfo;
	char szCurrentFileName[UNZ_MAXFILENAMEINZIP+1];
//	int err;

	pak = unzOpen(packfile);

//	numpackfiles = header.dirlen / sizeof(dpackfile_t);
//	numpackfiles = COM_pak3_getlen(*pak)/sizeof(unzFile);	
	numpackfiles = 0;
//= COM_pak3_getlen(pak)/sizeof(unzFile);
	Con_Printf ("Assigned Numpackfiles\n");

	if (!pak)
		return NULL;

	newfiles = Hunk_AllocName (numpackfiles * sizeof(unzFile), "packfile");

	status=unzGoToFirstFile(pak);

	while(status == UNZ_OK) {
		unzGetCurrentFileInfo(pak,NULL,&szCurrentFileName,64,NULL,0,NULL,0);

		if(strcmp(newfiles[i].name, szCurrentFileName)==0)
			break;

		strcpy (newfiles[i].name, szCurrentFileName);
		Con_Printf ("strcpy'ed %s into newfiles[%i].name Ok\n",szCurrentFileName, i);

		newfiles[i].filepos = LittleLong(unztell(pak));
//		newfiles[i].filelen = LittleLong(COM_pak3_readfile(pak,packfile,64,64));

		Con_Printf ("Added File\n");
		status=unzGoToNextFile(pak);
		++numpackfiles;
		++i;
	}

	Con_Printf ("Added files in %s to game data Ok\n", packfile);

	pack_old = Hunk_Alloc (sizeof (pack_t));
	strcpy (pack_old->filename, packfile);
	//pack_old->handle = unzGetLocalExtrafield(packfile, NULL, NULL);
	pack_old->numfiles = numpackfiles;
	pack_old->files = newfiles;
	
	Con_Printf ("Added packfile %s (%.0f files)\n", packfile, numpackfiles);

	COM_pak3_close(pak);
	return pack_old;
}
#endif 


/*
================
COM_AddGameDirectory

Sets com_gamedir, adds the directory to the head of the path,
then loads and adds pak1.pak pak2.pak ... 
================
*/
void COM_AddGameDirectory (char *dir)
{
	int                             i;
	searchpath_t    *search;
	pack_t                  *pak;
	char                    pakfile[MAX_OSPATH];

	strcpy (com_gamedir, dir);

//
// add the directory to the search path
//
	search = Hunk_Alloc (sizeof(searchpath_t));
	strcpy (search->filename, dir);
	search->next = com_searchpaths;
	com_searchpaths = search;

//
// add any pak files in the format pak0.pak pak1.pak, ...
//
	for (i=0 ; ; i++)
	{
		snprintf(pakfile, sizeof(pakfile), "%s/pak%i.pak", dir, i);
		pak = COM_LoadPackFile (pakfile);
		if (!pak)
			break;
		search = Hunk_Alloc (sizeof(searchpath_t));
		search->pack = pak;
		search->next = com_searchpaths;
		com_searchpaths = search;               
	}

//
// add the contents of the parms.txt file to the end of the command line
//

}

/*
================
COM_InitFilesystem
================
*/
void COM_InitFilesystem (void)
{
	int		i, j, len;
	char		basedir[MAX_OSPATH];
	searchpath_t	*search;
	char *		p;
	char *		games;

//
// -basedir <path>
// Overrides the system supplied base directory (under GAMENAME)
//
	i = COM_CheckParm ("-basedir");
	if (i && i < com_argc-1)
		strcpy (basedir, com_argv[i+1]);
	else
		strcpy (basedir, host_parms.basedir);

	j = strlen (basedir);

	if (j > 0)
	{
		if ((basedir[j-1] == '\\') || (basedir[j-1] == '/'))
			basedir[j-1] = 0;
	}

//
// -cachedir <path>
// Overrides the system supplied cache directory (NULL or /qcache)
// -cachedir - will disable caching.
//
	i = COM_CheckParm ("-cachedir");
	if (i && i < com_argc-1)
	{
		if (com_argv[i+1][0] == '-')
			com_cachedir[0] = 0;
		else
			strcpy (com_cachedir, com_argv[i+1]);
	}
	else if (host_parms.cachedir)
		strcpy (com_cachedir, host_parms.cachedir);
	else
		com_cachedir[0] = 0;

//
// start up with GAMENAME by default
//
	COM_AddGameDirectory (va("%s/"GAMENAME, basedir) );

	if (COM_CheckParm ("-rogue"))
		COM_AddGameDirectory (va("%s/rogue", basedir) );
	if (COM_CheckParm ("-hipnotic"))
		COM_AddGameDirectory (va("%s/hipnotic", basedir) );

//
// -game <gamedir>
// Adds basedir/gamedir as an override game
//
	i = COM_CheckParm ("-game");
	if (i && i < com_argc-1)
	{
		com_modified = true;

		len = strlen(com_argv[i+1]) + 1;
		games = (char *)malloc(len);
		strcpy(games, com_argv[i+1]);
		
		for (p = strtok(games, ",");
				p != NULL;
				p = strtok(NULL, ",")) {
			COM_AddGameDirectory (va("%s/%s", basedir, p));
		}

		free(games);

//		COM_AddGameDirectory (va("%s/%s", basedir, com_argv[i+1]));
	}

//
// -path <dir or packfile> [<dir or packfile>] ...
// Fully specifies the exact serach path, overriding the generated one
//
	i = COM_CheckParm ("-path");
	if (i)
	{
		com_modified = true;
		com_searchpaths = NULL;
		while (++i < com_argc)
		{
			if (!com_argv[i] || com_argv[i][0] == '+' || com_argv[i][0] == '-')
				break;
			
			search = Hunk_Alloc (sizeof(searchpath_t));
			if ( !strcmp(COM_FileExtension(com_argv[i]), "pak") )
			{
				search->pack = COM_LoadPackFile (com_argv[i]);
				if (!search->pack)
					Sys_Error ("Couldn't load packfile: %s", com_argv[i]);
			}
			else
				strcpy (search->filename, com_argv[i]);
			search->next = com_searchpaths;
			com_searchpaths = search;
		}
	}

	if (COM_CheckParm ("-proghack"))
		proghack = true;
}


