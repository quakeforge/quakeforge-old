/*
	quakefs.c

	virtual filesystem functions

	Copyright (C) 1996-1997  Id Software, Inc.
	Copyright (C) 1999,2000  Nelson Rush.
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
	along with this program; if not, write to:

		Free Software Foundation, Inc.
		59 Temple Place - Suite 330
		Boston, MA  02111-1307, USA

	$Id$
*/

#include <config.h>
#include <ctype.h>
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif
#include <fcntl.h>
#include <qtypes.h>
#include <quakefs.h>
#include <sys.h>
#include <console.h>
#include <common_quakedef.h>
#include <zone.h>
#include <common.h>
#include <draw.h>
#include <cvars.h>
#include <cmd.h>
#if defined(QUAKEWORLD) && defined(SERVERONLY)
#include <common_protocol.h>
#include <server.h>
#endif

#include <dirent.h>
#include <fnmatch.h>

#ifdef WIN32
#include <io.h>
#endif

#ifdef _MSC_VER
#define _POSIX_
#endif
#include <limits.h>


/*
All of Quake's data access is through a hierchal file system, but the contents of the file system can be transparently merged from several sources.

The "base directory" is the path to the directory holding the quake.exe and all
game directories.  The sys_* files pass this to host_init in quakeparms_t-
>basedir.  This can be overridden with the "-basedir" command line parm to allow
code debugging in a different directory.  The base directory is only used during
filesystem initialization.

The "game directory" is the first tree on the search path and directory that all
generated files (savegames, screenshots, demos, config files) will be saved to.
This can be overridden with the "-game" command line parameter.  The game
directory can never be changed while quake is executing.  This is a precacution
against having a malicious server instruct clients to write files over areas
they shouldn't.

The "cache directory" is only used during development to save network bandwidth,
especially over ISDN / T1 lines.  If there is a cache directory specified, when
a file is found by the normal search path, it will be mirrored into the cache
directory, then opened there.
*/

/*
=============================================================================

QUAKE FILESYSTEM

=============================================================================
*/

char	gamedirfile[MAX_OSPATH];

cvar_t	*fs_basepath;
cvar_t	*fs_sharepath;
cvar_t	*fs_basegame;

#ifdef GENERATIONS
#include <unzip.h>
typedef unsigned char byte_t;

#ifndef _AIX
typedef unsigned int uint_t;
typedef unsigned short ushort_t;
#endif
#endif

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
	QFile	*handle;
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

typedef struct searchpath_s
{
	char	filename[MAX_OSPATH];
	pack_t	*pack;		// only one of filename / pack will be used
	struct searchpath_s *next;
} searchpath_t;

searchpath_t	*com_searchpaths;
searchpath_t	*com_base_searchpaths;	// without gamedirs

/*
	COM_filelength
*/
int
COM_filelength ( QFile *f )
{
	int		pos;
	int		end;

	pos = Qtell (f);
	Qseek (f, 0, SEEK_END);
	end = Qtell (f);
	Qseek (f, pos, SEEK_SET);

	return end;
}

/*
	COM_FileOpenRead
*/
int
COM_FileOpenRead ( char *path, QFile **hndl )
{
	QFile	*f;

	f = Qopen(path, "rbz");
	if (!f)
	{
		*hndl = NULL;
		return -1;
	}
	*hndl = f;

	return COM_filelength(f);
}

/*
	COM_Path_f
*/
void
COM_Path_f ( void )
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
	COM_Maplist_f

	List map files in gamepaths.
*/
void
COM_Maplist_f ( void )
{
	searchpath_t	*search;
	DIR				*dir_ptr;
	struct dirent	*dirent;
	char			buf[MAX_OSPATH];

	for (search = com_searchpaths ; search != NULL ; search = search->next) {
		if (search->pack) {
			int i;
			pack_t *pak = search->pack;
			Con_Printf ("Looking in %s...\n",search->pack->filename);
			for (i=0 ; i<pak->numfiles ; i++) {
				char *name=pak->files[i].name;
				if (!fnmatch ("maps/*.bsp", name, FNM_PATHNAME)
					|| !fnmatch ("maps/*.bsp.gz", name, FNM_PATHNAME))
					Con_Printf ("%s\n", name+5);
			}
		} else {
			snprintf (buf, sizeof(buf), "%s/maps", search->filename);
			dir_ptr = opendir(buf);
			Con_Printf ("Looking in %s...\n",buf);
			if (!dir_ptr)
				continue;
			while ((dirent = readdir (dir_ptr)))
				if (!fnmatch ("*.bsp", dirent->d_name, 0)
					|| !fnmatch ("*.bsp.gz", dirent->d_name, 0))
					Con_Printf ("%s\n", dirent->d_name);
			closedir (dir_ptr);
		}
	}
}

/*
	COM_WriteFile

	The filename will be prefixed by the current game directory
*/
void
COM_WriteFile ( char *filename, void *data, int len )
{
	QFile	*f;
	char	name[MAX_OSPATH];

	snprintf(name, sizeof(name), "%s/%s", com_gamedir, filename);

	f = Qopen (name, "wb");
	if (!f) {
		Sys_mkdir(com_gamedir);
		f = Qopen (name, "wb");
		if (!f)
			Sys_Error ("Error opening %s", filename);
	}

	Sys_Printf ("COM_WriteFile: %s\n", name);
	Qwrite (f, data, len);
	Qclose (f);
}


/*
	COM_CreatePath

	Only used for CopyFile and download
*/
void
COM_CreatePath ( char *path )
{
	char	*ofs;
	char	e_path[PATH_MAX];

	Qexpand_squiggle (path, e_path);
	path = e_path;

	for (ofs = path+1 ; *ofs ; ofs++) {
		if (*ofs == '/') {	// create the directory
			*ofs = 0;
			Sys_mkdir (path);
			*ofs = '/';
		}
	}
}


/*
	COM_CopyFile

	Copies a file over from the net to the local cache, creating any
	directories needed.  This is for the convenience of developers using
	ISDN from home.
*/
void
COM_CopyFile ( char *netpath, char *cachepath )
{
	QFile	*in, *out;
	int		remaining, count;
	char	buf[4096];

	remaining = COM_FileOpenRead (netpath, &in);
	COM_CreatePath (cachepath);	// create directories up to the cache file
	out = Qopen(cachepath, "wb");
	if (!out)
		Sys_Error ("Error opening %s", cachepath);

	while (remaining)
	{
		if (remaining < sizeof(buf))
			count = remaining;
		else
			count = sizeof(buf);
		Qread (in, buf, count);
		Qwrite (out, buf, count);
		remaining -= count;
	}

	Qclose (in);
	Qclose (out);
}

/*
	COM_OpenRead
*/
QFile *
COM_OpenRead ( const char *path, int offs, int len )
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
	return Qdopen(fd,"rbz");
	return 0;
}

int file_from_pak;	// global indicating file came from pack file ZOID

/*
	COM_FOpenFile

	Finds the file in the search path.
	Sets com_filesize and one of handle or file
*/
int
COM_FOpenFile ( char *filename, QFile **gzfile )
{
	searchpath_t	*search;
	char		netpath[MAX_OSPATH];
	pack_t		*pak;
	int			i;
	int			findtime;
#ifdef HAS_ZLIB
	char		gzfilename[MAX_OSPATH];
	int		filenamelen;;

	filenamelen = strlen(filename);
	strncpy(gzfilename,filename,sizeof(gzfilename));
	strncat(gzfilename,".gz",sizeof(gzfilename));
#endif

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
#ifdef HAS_ZLIB
				if (!strncmp(pak->files[i].name, filename, filenamelen)) {
					if (!pak->files[i].name[filenamelen])
						fn=filename;
					else if (!strcmp (pak->files[i].name, gzfilename))
						fn=gzfilename;
				}
#else
				if (!strcmp (pak->files[i].name, filename))
					fn=filename;
#endif
				if (fn)
				{	// found it!
					if (developer->value)
						Sys_Printf ("PackFile: %s : %s\n",pak->filename, fn);
					// open a new file on the pakfile
					*gzfile=COM_OpenRead(pak->filename,pak->files[i].filepos,
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
#ifdef HAS_ZLIB
				snprintf(netpath, sizeof(netpath), "%s/%s",search->filename,
						 gzfilename);
				findtime = Sys_FileTime (netpath);
				if (findtime == -1)
#endif
					continue;
			}

			if(developer->value)
				Sys_Printf ("FindFile: %s\n",netpath);

			*gzfile=COM_OpenRead(netpath,-1,-1);
			return com_filesize;
		}

	}

	Sys_Printf ("FindFile: can't find %s\n", filename);

	*gzfile = NULL;
	com_filesize = -1;
	return -1;
}

cache_user_t *loadcache;
byte	*loadbuf;
int		loadsize;

/*
	COM_LoadFile

	Filename are relative to the quake directory.
	Always appends a 0 byte to the loaded data.
*/
byte *
COM_LoadFile ( char *path, int usehunk )
{
	QFile	*h;
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
	Qread (h, buf, len);
	Qclose (h);
#ifndef SERVERONLY
	Draw_EndDisc ();
#endif

	return buf;
}

byte *
COM_LoadHunkFile ( char *path )
{
	return COM_LoadFile (path, 1);
}

byte *
COM_LoadTempFile ( char *path )
{
	return COM_LoadFile (path, 2);
}

void
COM_LoadCacheFile ( char *path, struct cache_user_s *cu )
{
	loadcache = cu;
	COM_LoadFile (path, 3);
}

// uses temp hunk if larger than bufsize
byte *
COM_LoadStackFile ( char *path, void *buffer, int bufsize )
{
	byte	*buf;

	loadbuf = (byte *)buffer;
	loadsize = bufsize;
	buf = COM_LoadFile (path, 4);

	return buf;
}

/*
	COM_LoadPackFile

	Takes an explicit (not game tree related) path to a pak file.

	Loads the header and directory, adding the files at the beginning
	of the list so they override previous pack files.
*/
pack_t *
COM_LoadPackFile ( char *packfile )
{
	dpackheader_t		header;
	int			i;
	packfile_t		*newfiles;
	int			numpackfiles;
	pack_t			*pack;
	QFile			*packhandle;
	dpackfile_t		info[MAX_FILES_IN_PACK];

	if (COM_FileOpenRead (packfile, &packhandle) == -1)
		return NULL;

	Qread (packhandle, &header, sizeof(header));
	if (header.id[0] != 'P' || header.id[1] != 'A'
	|| header.id[2] != 'C' || header.id[3] != 'K')
		Sys_Error ("%s is not a packfile", packfile);
	header.dirofs = LittleLong (header.dirofs);
	header.dirlen = LittleLong (header.dirlen);

	numpackfiles = header.dirlen / sizeof(dpackfile_t);

	if (numpackfiles > MAX_FILES_IN_PACK)
		Sys_Error ("%s has %i files", packfile, numpackfiles);

	newfiles = Z_Malloc (numpackfiles * sizeof(packfile_t));

	Qseek (packhandle, header.dirofs, SEEK_SET);
	Qread (packhandle, info, header.dirlen);


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

#ifdef GENERATIONS
int
COM_pakzip_checkfile ( unzFile *pak, const char *path )
{
	int status;

	status = unzLocateFile(pak, path, 2);
	return (status == UNZ_OK);
}

void
COM_pakzip_closepak ( unzFile *pak )
{
	if (pak)
		unzClose(pak);
	pak = NULL;
}

void
COM_pakzip_close ( unzFile *pak )
{
	unzCloseCurrentFile(pak);
}

int
COM_pakzip_read ( unzFile *pak, void *buf, uint_t size, uint_t nmemb )
{
	int len = unzReadCurrentFile(pak, buf, size * nmemb);
	return len / size;
}

int
COM_pakzip_open ( unzFile *pak, const char *path )
{
	if (unzLocateFile(pak, path, 2) != UNZ_OK)
		return 0;
	if (unzOpenCurrentFile(pak) != UNZ_OK)
		return 0;
	return 1;
}

uint_t
COM_pakzip_getlen ( unzFile *pak )
{
	unz_file_info info;

	if (unzGetCurrentFileInfo(pak, &info, NULL, 0, NULL, 0, NULL, 0)
		!= UNZ_OK)
		return 0;
	return info.uncompressed_size;
}

uint_t
COM_pakzip_readfile ( unzFile *pak, const char *path, uint_t bufsize, byte_t *buf )
{
	uint_t len;

	if (!COM_pakzip_open(pak,path))
		return 0;

	if ((len = COM_pakzip_getlen(pak)) != 0)
	{
		if (COM_pakzip_read(pak, (void*)buf, 1, len) != len)
			len = 0;
	}
	COM_pakzip_close(pak);
	return len;
}


pack_t *
COM_LoadPackZipFile ( char *packfile )
{
	int			i=0;
	packfile_t	*newfiles;
	float		numpackfiles;
	unzFile		*pak;
	pack_t		*pack_old;
	int			status;

//	This following variable info is unused ATM.
//	dpackfile_t	info[MAX_FILES_IN_PACK];
	char szCurrentFileName[UNZ_MAXFILENAMEINZIP+1];

	pak = unzOpen(packfile);

	numpackfiles = 0;
	Con_Printf ("Assigned Numpackfiles\n");

	if (!pak)
		return NULL;

	newfiles = Hunk_AllocName (numpackfiles * sizeof(unzFile), "packfile");

	status=unzGoToFirstFile(pak);

	while(status == UNZ_OK) {
//		unzGetCurrentFileInfo(pak,NULL,&szCurrentFileName,64,NULL,0,NULL,0);

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

	COM_pakzip_close(pak);
	return pack_old;
}
#endif

#define FBLOCK_SIZE	32
#define FNAME_SIZE	MAX_OSPATH

void
COM_LoadGameDirectory ( char *dir )
{
	searchpath_t	*search;
	pack_t			*pak;
	DIR				*dir_ptr;
	struct dirent	*dirent;
	char			**pakfiles = NULL;
	int				i = 0, bufsize = 0, count = 0;

	Con_DPrintf ("COM_LoadGameDirectory (\"%s\")\n", dir);

	pakfiles = malloc(FBLOCK_SIZE * sizeof(char *));
	bufsize += FBLOCK_SIZE;
	if (!pakfiles)
		goto COM_LoadGameDirectory_free;

	for (i = count; i < bufsize; i++) {
		pakfiles[i] = NULL;
	}

	dir_ptr = opendir(dir);
	if (!dir_ptr)
		return;

	while ((dirent = readdir(dir_ptr))) {
		if (!fnmatch("*.pak", dirent->d_name, 0)) {
			if (count >= bufsize) {
				bufsize += FBLOCK_SIZE;
				pakfiles = realloc(pakfiles, bufsize * sizeof(char *));
				if (!pakfiles)
					goto COM_LoadGameDirectory_free;
				for (i = count; i < bufsize; i++)
					pakfiles[i] = NULL;
			}

			pakfiles[count] = malloc(FNAME_SIZE);
			snprintf(pakfiles[count], FNAME_SIZE - 1, "%s/%s", dir,
					dirent->d_name);
			pakfiles[count][FNAME_SIZE - 1] = '\0';
			count++;
		}
	}
	closedir(dir_ptr);

	// XXX WARNING!!! This is /NOT/ subtable for strcmp!!!!!
	// This passes 'char **' instead of 'char *' to the cmp function!
	qsort(pakfiles, count, sizeof(char *),
			(int (*)(const void *, const void *)) Q_qstrcmp);

	for (i = 0; i < count; i++) {
		pak = COM_LoadPackFile(pakfiles[i]);

		if (!pak) {
			Sys_Error(va("Bad pakfile %s!!", pakfiles[i]));
		} else {
			search = Z_Malloc (sizeof(searchpath_t));
			search->pack = pak;
			search->next = com_searchpaths;
			com_searchpaths = search;
		}
	}

COM_LoadGameDirectory_free:
	for (i = 0; i < count; i++)
		free(pakfiles[i]);
	free(pakfiles);

#ifdef GENERATIONS
	for (done=false, i=0 ; !done ; i++ ) {
		snprintf(pakfile, sizeof(pakfile), "%s/pak%i.qz", dir, i);

		pak = COM_LoadPackZipFile(pakfile);

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
	COM_AddDirectory

	Sets com_gamedir, adds the directory to the head of the path,
	then loads and adds pak1.pak pak2.pak ...
*/
void
COM_AddDirectory ( char *dir )
{
	searchpath_t	*search;
	char			*p;
	char			e_dir[PATH_MAX];

	Qexpand_squiggle (dir, e_dir);
	dir = e_dir;

	if ((p = strrchr(dir, '/')) != NULL) {
		strcpy (gamedirfile, ++p);
		strcpy (com_gamedir, dir);
	} else {
		strcpy (gamedirfile, dir);
		strcpy (com_gamedir, va("%s/%s", fs_basepath->string, dir));
	}

//
// add the directory to the search path
//
	search = Z_Malloc (sizeof(searchpath_t));
	strcpy (search->filename, dir);
	search->next = com_searchpaths;
	com_searchpaths = search;

//
// add any pak files in the format pak0.pak pak1.pak, ...
//

	COM_LoadGameDirectory (dir);
}

/*
	COM_AddGameDirectory

	FIXME: this is a wrapper for COM_AddDirectory (which used to be
	this function) to have it load share and base paths.  Whenver
	someone goes through to clean up the fs code, this function should
	merge with COM_AddGameDirectory.
*/
void
COM_AddGameDirectory ( char *dir )
{
	Con_DPrintf ("COM_AddGameDirectory (\"%s/%s\")\n",
			fs_sharepath->string, dir);

	if (strcmp (fs_sharepath->string, fs_basepath->string) != 0)
		COM_AddDirectory (va("%s/%s", fs_sharepath->string, dir));
	COM_AddDirectory (va("%s/%s", fs_basepath->string, dir));
}

/*
	COM_Gamedir

	Sets the gamedir and path to a different directory.
*/
void
COM_Gamedir ( char *dir )
{
	searchpath_t	*next;

	if (strstr(dir, "..") || strstr(dir, "/")
		|| strstr(dir, "\\") || strstr(dir, ":") )
	{
		Con_Printf ("Gamedir should be a single filename, not a path\n");
		return;
	}

	if (strcmp (gamedirfile, dir) == 0)
		return;		// still the same
	strcpy (gamedirfile, dir);

	//
	// free up any current game dir info
	//
	while (com_searchpaths != com_base_searchpaths)
	{
		if (com_searchpaths->pack)
		{
			Qclose (com_searchpaths->pack->handle);
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

	if (strcmp (dir, fs_basegame->string) == 0)
		return;
#ifdef QUAKEWORLD
	if (strcmp (dir, "qw") == 0)
		return;
#endif

	COM_AddGameDirectory (dir);
}

/*
================
SV_Gamedir_f

Sets the gamedir and path to a different directory.
================
*/

void COM_Gamedir_f ( void )
{
	char			*dir;

	if (Cmd_Argc() == 1)
	{
		Con_Printf ("Current gamedir: %s\n", gamedirfile);
		return;
	}

	if (Cmd_Argc() != 2)
	{
		Con_Printf ("Usage: gamedir <newdir>\n");
		return;
	}

	dir = Cmd_Argv(1);

	if (strstr(dir, "..") || strstr(dir, "/")
		|| strstr(dir, "\\") || strstr(dir, ":") )
	{
		Con_Printf ("Gamedir should be a single filename, not a path\n");
		return;
	}

	COM_Gamedir (dir);
#if defined(QUAKEWORLD) && defined(SERVERONLY)
	Info_SetValueForStarKey (svs.info, "*gamedir", dir, MAX_SERVERINFO_STRING);
#endif
}

/*
	COM_InitFilesystem
*/
void
COM_InitFilesystem ( void )
{
#ifdef UQUAKE
	int		i;
#endif

	fs_basepath = Cvar_Get ("fs_basepath", FS_BASEPATH, CVAR_NONE,
			"the location of your game directories");
	fs_sharepath = Cvar_Get ("fs_sharepath", fs_basepath->string,
			CVAR_NONE, "read-only game directories");
	fs_basegame = Cvar_Get ("fs_basegame", GAMENAME,
			CVAR_NONE, "the default gamedir");

	Cmd_AddCommand ("gamedir", COM_Gamedir_f);

/*
	start up with fs_basegame by default
*/
	COM_AddGameDirectory (fs_basegame->string);
	if (hipnotic)
		COM_AddGameDirectory ("hipnotic");
	if (rogue)
		COM_AddGameDirectory ("rogue");

#ifdef QUAKEWORLD
	COM_AddGameDirectory ("qw");
#elif UQUAKE
	i = COM_CheckParm ("-game");
	if (i && i < com_argc-1)
		COM_AddGameDirectory (com_argv[i+1]);
#endif

	// any set gamedirs will be freed up to here
	com_base_searchpaths = com_searchpaths;
}
