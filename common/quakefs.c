/*
quakefs.c - virtual filesystem functions
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

#include <ctype.h>
#include <unistd.h>
#include <fcntl.h>
#include <qtypes.h>
#include <quakefs.h>
#include <sys.h>
#include <console.h>
#include <common_quakedef.h>
#include <zone.h>
#include <common.h>
#include <draw.h>

/*
All of Quake's data access is through a hierchal file system, but the contents of the file system can be transparently merged from several sources.

The "base directory" is the path to the directory holding the quake.exe and all game directories.  The sys_* files pass this to host_init in quakeparms_t->basedir.  This can be overridden with the "-basedir" command line parm to allow code debugging in a different directory.  The base directory is
only used during filesystem initialization.

The "game directory" is the first tree on the search path and directory that all generated files (savegames, screenshots, demos, config files) will be saved to.  This can be overridden with the "-game" command line parameter.  The game directory can never be changed while quake is executing.  This is a precacution against having a malicious server instruct clients to write files over areas they shouldn't.

The "cache directory" is only used during development to save network bandwidth, especially over ISDN / T1 lines.  If there is a cache directory
specified, when a file is found by the normal search path, it will be mirrored
into the cache directory, then opened there.
*/

/*
=============================================================================

QUAKE FILESYSTEM

=============================================================================
*/

/* Begin Generations */
#if defined(_EXPERIMENTAL_) && defined(GENERATIONS)
#include "unzip.h"
#endif

typedef unsigned char byte_t;
char	gamedirfile[MAX_OSPATH];

#if 0
#ifndef _AIX
typedef unsigned int uint_t;
typedef unsigned short ushort_t;
#endif
#endif
/* End Generations */

int com_filesize;

/*
	In-memory pack file structs
*/

typedef struct
{
	char	name[MAX_QPATH];
	int		filepos, filelen;
} packfile_t;

typedef struct pack_s
{
	char	filename[MAX_OSPATH];
	FILE	*handle;
	int		numfiles;
	packfile_t	*files;
} pack_t;

/*
	Structs for pack files on disk
*/
typedef struct
{
	char	name[56];
	int		filepos, filelen;
} dpackfile_t;

typedef struct
{
	char	id[4];
	int		dirofs;
	int		dirlen;
} dpackheader_t;

#define	MAX_FILES_IN_PACK	2048

char	com_gamedir[MAX_OSPATH];
char	com_basedir[MAX_OSPATH];

typedef struct searchpath_s
{
	char	filename[MAX_OSPATH];
	pack_t	*pack;		// only one of filename / pack will be used
	struct searchpath_s *next;
} searchpath_t;

searchpath_t	*com_searchpaths;
searchpath_t	*com_base_searchpaths;	// without gamedirs

/*
================
COM_filelength
================
*/
int COM_filelength (FILE *f)
{
	int		pos;
	int		end;

	pos = ftell (f);
	fseek (f, 0, SEEK_END);
	end = ftell (f);
	fseek (f, pos, SEEK_SET);

	return end;
}

int COM_FileOpenRead (char *path, FILE **hndl)
{
	FILE	*f;

	f = fopen(path, "rb");
	if (!f)
	{
		*hndl = NULL;
		return -1;
	}
	*hndl = f;
	
	return COM_filelength(f);
}

/*
============
COM_Path_f

============
*/
void COM_Path_f (void)
{
	searchpath_t	*s;
	
	Con_Printf ("Current search path:\n");
	for (s=com_searchpaths ; s ; s=s->next)
	{
		if (s == com_base_searchpaths)
			Con_Printf ("----------\n");
		if (s->pack)
			Con_Printf ("%s (%i files)\n", s->pack->filename, s->pack->numfiles);
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
	FILE	*f;
	char	name[MAX_OSPATH];
	
	snprintf(name, sizeof(name), "%s/%s", com_gamedir, filename);
	
	f = fopen (name, "wb");
	if (!f) {
		Sys_mkdir(com_gamedir);
		f = fopen (name, "wb");
		if (!f)
			Sys_Error ("Error opening %s", filename);
	}
	
	Sys_Printf ("COM_WriteFile: %s\n", name);
	fwrite (data, 1, len, f);
	fclose (f);
}


/*
============
COM_CreatePath

Only used for CopyFile and download
============
*/
void	COM_CreatePath (char *path)
{
	char	*ofs;
	
	for (ofs = path+1 ; *ofs ; ofs++)
	{
		if (*ofs == '/')
		{	// create the directory
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
	FILE	*in, *out;
	int		remaining, count;
	char	buf[4096];
	
	remaining = COM_FileOpenRead (netpath, &in);		
	COM_CreatePath (cachepath);	// create directories up to the cache file
	out = fopen(cachepath, "wb");
	if (!out)
		Sys_Error ("Error opening %s", cachepath);
	
	while (remaining)
	{
		if (remaining < sizeof(buf))
			count = remaining;
		else
			count = sizeof(buf);
		fread (buf, 1, count, in);
		fwrite (buf, 1, count, out);
		remaining -= count;
	}

	fclose (in);
	fclose (out);
}


gzFile *COM_gzOpenRead(const char *path, int offs, int len)
{
	int fd=open(path,O_RDONLY);
	unsigned char id[2];
	unsigned char len_bytes[4];
	if (fd==-1) {
		Sys_Error ("Couldn't open %s", path);
		return 0;
	}
	if (offs<0 || len<0) {
		// normal file
		offs=0;
		len=lseek(fd,0,SEEK_END);
		lseek(fd,0,SEEK_SET);
	}
	read(fd,id,2);
	if (id[0]==0x1f && id[1]==0x8b) {
		lseek(fd,offs+len-4,SEEK_SET);
		read(fd,len_bytes,4);
		len=((len_bytes[3]<<24)
			 |(len_bytes[2]<<16)
			 |(len_bytes[1]<<8)
			 |(len_bytes[0]));
	}
	lseek(fd,offs,SEEK_SET);
	com_filesize=len;
	return gzdopen(fd,"rb");
}

/*
===========
COM_FindFile

Finds the file in the search path.
Sets com_filesize and one of handle or file
===========
*/
int file_from_pak; // global indicating file came from pack file ZOID

int COM_FOpenFile (char *filename, gzFile **gzfile)
{
	searchpath_t	*search;
	char		netpath[MAX_OSPATH];
	pack_t		*pak;
	int			i;
	int			findtime;
	char		gzfilename[MAX_OSPATH];

	strncpy(gzfilename,filename,sizeof(gzfilename));
	strncat(gzfilename,".gz",sizeof(gzfilename));

	file_from_pak = 0;
		
//
// search through the path, one element at a time
//
	for (search = com_searchpaths ; search ; search = search->next)
	{
	// is the element a pak file?
		if (search->pack)
		{
		// look through all the pak file elements
			pak = search->pack;
			for (i=0 ; i<pak->numfiles ; i++) {
				char *fn=0;
				if (!strcmp (pak->files[i].name, filename))
					fn=filename;
				else if (!strcmp (pak->files[i].name, gzfilename))
					fn=gzfilename;
				if (fn)
				{	// found it!
					if(developer.value)
						Sys_Printf ("PackFile: %s : %s\n",pak->filename, fn);
				// open a new file on the pakfile
					//*file = fopen (pak->filename, "rb");
					//if (!*file)
					//	Sys_Error ("Couldn't reopen %s", pak->filename);
					//fseek (*file, pak->files[i].filepos, SEEK_SET);
					//com_filesize = pak->files[i].filelen;
					*gzfile=COM_gzOpenRead(pak->filename,pak->files[i].filepos,
										   pak->files[i].filelen);
					file_from_pak = 1;
					return com_filesize;
				}
			}
		}
		else
		{		
	// check a file in the directory tree
			snprintf(netpath, sizeof(netpath), "%s/%s",search->filename,
					 filename);
			
			findtime = Sys_FileTime (netpath);
			if (findtime == -1) {
				snprintf(netpath, sizeof(netpath), "%s/%s",search->filename,
						 gzfilename);
				findtime = Sys_FileTime (netpath);
				if (findtime == -1)
					continue;
			}
				
			if(developer.value)
				Sys_Printf ("FindFile: %s\n",netpath);

			//*file = fopen (netpath, "rb");
			//return COM_filelength (*file);
			*gzfile=COM_gzOpenRead(netpath,-1,-1);
			return com_filesize;
		}
		
	}
	
	Sys_Printf ("FindFile: can't find %s\n", filename);
	
	*gzfile = NULL;
	com_filesize = -1;
	return -1;
}

/*
============
COM_LoadFile

Filename are relative to the quake directory.
Allways appends a 0 byte to the loaded data.
============
*/
cache_user_t *loadcache;
byte	*loadbuf;
int		loadsize;
byte *COM_LoadFile (char *path, int usehunk)
{
	gzFile	*h;
	byte	*buf;
	char	base[32];
	int		len;

	buf = NULL;	// quiet compiler warning

// look for it in the filesystem or pack files
	len = com_filesize = COM_FOpenFile (path, &h);
	if (!h)
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
#ifndef SERVERONLY
	Draw_BeginDisc ();
#endif
	gzread (h, buf, len);
	gzclose (h);
#ifndef SERVERONLY
	Draw_EndDisc ();
#endif

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
	byte	*buf;
	
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
	dpackheader_t	header;
	int				i;
	packfile_t		*newfiles;
	int				numpackfiles;
	pack_t			*pack;
	FILE			*packhandle;
	dpackfile_t		info[MAX_FILES_IN_PACK];

	if (COM_FileOpenRead (packfile, &packhandle) == -1)
		return NULL;

	fread (&header, 1, sizeof(header), packhandle);
	if (header.id[0] != 'P' || header.id[1] != 'A'
	|| header.id[2] != 'C' || header.id[3] != 'K')
		Sys_Error ("%s is not a packfile", packfile);
	header.dirofs = LittleLong (header.dirofs);
	header.dirlen = LittleLong (header.dirlen);

	numpackfiles = header.dirlen / sizeof(dpackfile_t);

	if (numpackfiles > MAX_FILES_IN_PACK)
		Sys_Error ("%s has %i files", packfile, numpackfiles);

	newfiles = Z_Malloc (numpackfiles * sizeof(packfile_t));

	fseek (packhandle, header.dirofs, SEEK_SET);
	fread (info, 1, header.dirlen, packhandle);


// parse the directory
	for (i=0 ; i<numpackfiles ; i++)
	{
		strcpy (newfiles[i].name, info[i].name);
		newfiles[i].filepos = LittleLong(info[i].filepos);
		newfiles[i].filelen = LittleLong(info[i].filelen);
	}

	pack = Z_Malloc (sizeof (pack_t));
	strcpy (pack->filename, packfile);
	pack->handle = packhandle;
	pack->numfiles = numpackfiles;
	pack->files = newfiles;
	
	Con_Printf ("Added packfile %s (%i files)\n", packfile, numpackfiles);
	return pack;
}

#if defined(_EXPERIMENTAL_) && defined(GENERATIONS)
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

#endif

#if defined _EXPERIMENTAL_ && GENERATIONS 
pack_t *COM_LoadQ3PackFile (char *packfile)
{

	int                             i;
	packfile_t              	*newfiles;
	float                  		numpackfiles;
	unzFile            		*pak;
	pack_t 				*pack_old;
	int 				status;
	dpackfile_t            		info[MAX_FILES_IN_PACK];
	char szCurrentFileName[UNZ_MAXFILENAMEINZIP+1];

	pak = unzOpen(packfile);

	numpackfiles = 0;
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

void
COM_LoadGameDirectory(char *dir)
{
	int 			i;
	searchpath_t	*search;
	pack_t			*pak;
	char			pakfile[MAX_OSPATH];
	qboolean 		done = false;
	
	for ( i=0 ; !done ; i++ ) {	// Load all Pak1 files
		snprintf(pakfile, sizeof(pakfile), "%s/pak%i.pak", dir, i);

		pak = COM_LoadPackFile(pakfile);                
		
		if( !pak ) {
			done = true;
		} else {
			search = Hunk_Alloc (sizeof(searchpath_t));
			search->pack = pak;
			search->next = com_searchpaths;
			com_searchpaths = search;
		}
	}

#if defined _EXPERIMENTAL_ && GENERATIONS
	done = false;
	for ( i=0 ; !done ; i++ ) {	// Load all Pak3 files.
		snprintf(pakfile, sizeof(pakfile), "%s/pak%i.pak3", dir, i);
		
		pak = COM_LoadPak3File(pakfile);
 
		if(!pak) {
			done = true;
		} else {
			search = Hunk_Alloc (sizeof(searchpath_t));
			search->pack = pak;
			search->next = com_searchpaths;
			com_searchpaths = search;
        }
	}
#endif
}


/*
================
COM_AddGameDirectory

Sets com_gamedir, adds the directory to the head of the path,
then loads and adds pak1.pak pak2.pak ... 
================
*/
void COM_AddGameDirectory (char *dir)
{
	searchpath_t	*search;
	char			*p;

	if ((p = strrchr(dir, '/')) != NULL)
		strcpy(gamedirfile, ++p);
	else
		strcpy(gamedirfile, dir);
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

	COM_LoadGameDirectory(dir);
}

/*
================
COM_Gamedir

Sets the gamedir and path to a different directory.
================
*/
void COM_Gamedir (char *dir)
{
	searchpath_t	*search, *next;

	if (strstr(dir, "..") || strstr(dir, "/")
		|| strstr(dir, "\\") || strstr(dir, ":") )
	{
		Con_Printf ("Gamedir should be a single filename, not a path\n");
		return;
	}

	if (!strcmp(gamedirfile, dir))
		return;		// still the same
	strcpy (gamedirfile, dir);

	//
	// free up any current game dir info
	//
	while (com_searchpaths != com_base_searchpaths)
	{
		if (com_searchpaths->pack)
		{
			fclose (com_searchpaths->pack->handle);
			Z_Free (com_searchpaths->pack->files);
			Z_Free (com_searchpaths->pack);
		}
		next = com_searchpaths->next;
		Z_Free (com_searchpaths);
		com_searchpaths = next;
	}

	//
	// flush all data, so it will be forced to reload
	//
	Cache_Flush ();

	if (!strcmp(dir, GAMENAME) || !strcmp(dir, "qw"))
		return;

	snprintf(com_gamedir, sizeof(com_gamedir), "%s/%s", com_basedir, dir);

	//
	// add the directory to the search path
	//
	search = Z_Malloc (sizeof(searchpath_t));
	strcpy (search->filename, com_gamedir);
	search->next = com_searchpaths;
	com_searchpaths = search;

	//
	// add any pak files in the format pak0.pak pak1.pak, ...
	//

	COM_LoadGameDirectory(dir);
}

/*
================
COM_InitFilesystem
================
*/
void
COM_InitFilesystem ( void )
{
	int		i, len;
	char	*p;
	char	*games;

/*
	-basedir <path>
	Overrides the system supplied base directory
*/
	i = COM_CheckParm ("-basedir");
	if (i && i < com_argc-1)
		strcpy (com_basedir, com_argv[i+1]);
	else
		strcpy (com_basedir, host_parms.basedir);

/*
	start up with GAMENAME by default
*/
	COM_AddGameDirectory (va("%s/" GAMENAME, com_basedir) );
#ifdef QUAKEWORLD
	COM_AddGameDirectory (va("%s/qw", com_basedir) );
#endif	// QUAKEWORLD
	if ( hipnotic ) {
		COM_AddGameDirectory (va("%s/hipnotic", com_basedir) );
	}
	if ( rogue ) {
		COM_AddGameDirectory (va("%s/rogue", com_basedir) );
	}

	i = COM_CheckParm ("-game");
	if (i && i < com_argc-1) {

		len = strlen(com_argv[i+1]) + 1;
		games = (char *) malloc(len);
		strcpy(games, com_argv[i+1]);

		for ( p = strtok(games, ",") ; p != NULL; p = strtok(NULL, ",")) {
			COM_AddGameDirectory (va("%s/%s", com_basedir, p));
		}
		free(games);
	}

	// any set gamedirs will be freed up to here
	com_base_searchpaths = com_searchpaths;
}
