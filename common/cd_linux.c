/*
Copyright (C) 1996-1997  Id Software, Inc.
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
// Quake is a trademark of Id Software, Inc., (c) 1996 Id Software, Inc. All
// rights reserved.

#include <qtypes.h>
#include <quakedef.h>
#include <console.h>
#include <cdaudio.h>
#include <common.h>
#include <sound.h>
#include <cmd.h>
#include <cvar.h>
#include <client.h>

#include <stdio.h>
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif
#include <stdlib.h>
#include <sys/ioctl.h>
#include <sys/file.h>
#include <sys/types.h>
#include <fcntl.h>
#include <string.h>
#include <time.h>
#include <errno.h>

#ifdef USE_LINUX_CD
# include <linux/cdrom.h>
#endif
#ifdef USE_BSD_CD
# include <sys/cdio.h>
#endif


static qboolean cdValid = false;
static qboolean	playing = false;
static qboolean	wasPlaying = false;
static qboolean	initialized = false;
static qboolean	enabled = true;
static qboolean playLooping = false;
static float	cdvolume;
static byte 	remap[100];
static byte	playTrack;
static byte	maxTrack;

static int cdfile = -1;
static char cd_dev[64] = "/dev/cdrom";

#ifdef USE_BSD_CD
# define CDROMEJECT		CDIOCEJECT
# define CDROMCLOSETRAY		CDIOCCLOSE
# define CDROMREADTOCHDR	CDIOREADTOCHEADER
# define CDROMREADTOCENTRY	CDIOREADTOCENTRY
# define CDROMPLAYTRKIND	CDIOCPLAYTRACKS
# define CDROMRESUME		CDIOCRESUME
# define CDROMSTOP		CDIOCSTOP
# define CDROMPAUSE		CDIOCPAUSE
# define CDROMSUBCHNL		CDIOCREADSUBCHANNEL
#endif


static void CDAudio_Eject(void)
{
	if (cdfile == -1 || !enabled)
		return; // no cd init'd

	if ( ioctl(cdfile, CDROMEJECT) == -1 )
		Con_DPrintf("CD eject ioctl failed\n");
}


static void CDAudio_CloseDoor(void)
{
	if (cdfile == -1 || !enabled)
		return; // no cd init'd

	if ( ioctl(cdfile, CDROMCLOSETRAY) == -1 )
		Con_DPrintf("CD close ioctl failed\n");
}

static int CDAudio_GetAudioDiskInfo(void)
{
#if defined(USE_LINUX_CD)
	struct cdrom_tochdr tochdr;
#elif defined(USE_BSD_CD)
	struct ioc_toc_header tochdr;
#endif

	cdValid = false;

	if ( ioctl(cdfile, CDROMREADTOCHDR, &tochdr) == -1 )
	{
		Con_DPrintf("CD readtochdr ioctl failed\n");
		return -1;
	}

#if defined(USE_LINUX_CD)
	if (tochdr.cdth_trk0 < 1)
#elif defined(USE_BSD_CD)
	if (tochdr.starting_track < 1)
#endif
	{
		Con_DPrintf("CDAudio: no music tracks\n");
		return -1;
	}

	cdValid = true;
#if defined(USE_LINUX_CD)
	maxTrack = tochdr.cdth_trk1;
#elif defined(USE_BSD_CD)
	maxTrack = tochdr.ending_track;
#endif

	return 0;
}


void CDAudio_Play(byte track, qboolean looping)
{
#if defined(USE_LINUX_CD)
	struct cdrom_tocentry entry;
	struct cdrom_ti ti;
#elif defined(USE_BSD_CD)
#ifdef HAVE_STRUCT_IOC_READ_TOC_SINGLE_ENTRY_ENTRY
	struct ioc_read_toc_single_entry entry;
#endif
	struct ioc_play_track ti;
#endif

	if (cdfile == -1 || !enabled)
		return;

	if (!cdValid)
	{
		CDAudio_GetAudioDiskInfo();
		if (!cdValid)
			return;
	}

	track = remap[track];

	if (track < 1 || track > maxTrack)
	{
		Con_DPrintf("CDAudio: Bad track number %u.\n", track);
		return;
	}

#if !defined(USE_BSD_CD) || defined(HAVE_STRUCT_IOC_READ_TOC_SINGLE_ENTRY_ENTRY)
	// don't try to play a non-audio track
#if defined(USE_LINUX_CD)
	entry.cdte_track = track;
	entry.cdte_format = CDROM_MSF;
#elif defined(USE_BSD_CD)
	entry.track = track;
	entry.address_format = CD_MSF_FORMAT;
#endif
	if ( ioctl(cdfile, CDROMREADTOCENTRY, &entry) == -1 )
	{
		Con_DPrintf("ioctl cdromreadtocentry failed\n");
		return;
	}
#if defined(USE_LINUX_CD)
	if (entry.cdte_ctrl == CDROM_DATA_TRACK)
#elif defined(USE_BSD_CD)
	/* XXX: Is this magic number really necessary? */
	if (entry.entry.control & 4)
#endif
	{
		Con_Printf("CDAudio: track %i is not audio\n", track);
		return;
	}
#endif /* HAVE_STRUCT_IOC_READ_TOC_SINGLE_ENTRY_ENTRY */

	if (playing)
	{
		if (playTrack == track)
			return;
		CDAudio_Stop();
	}

#if defined(USE_LINUX_CD)
	ti.cdti_trk0 = track;
	ti.cdti_trk1 = track;
	ti.cdti_ind0 = 1;
	ti.cdti_ind1 = 99;
#elif defined(USE_BSD_CD)
	ti.start_track = track;
	ti.end_track = track;
	ti.start_index = 1;
	ti.end_index = 99;
#endif

#ifdef USE_BSD_CD
	if (ioctl(cdfile, CDIOCSTART) == -1) {
		Con_DPrintf("CD start ioctl failed\n");
	}
#endif

	if (ioctl(cdfile, CDROMPLAYTRKIND, &ti) == -1) {
		Con_DPrintf("CD play ioctl failed\n");
		return;
	}

	if ( ioctl(cdfile, CDROMRESUME) == -1 )
		Con_DPrintf("CD resume ioctl failed\n");

	playLooping = looping;
	playTrack = track;
	playing = true;

	if (cdvolume == 0.0)
		CDAudio_Pause ();
}


void CDAudio_Stop(void)
{
	if (cdfile == -1 || !enabled)
		return;

	if (!playing)
		return;

	if ( ioctl(cdfile, CDROMSTOP) == -1 )
		Con_DPrintf("CD stop ioctl failed (%d)\n", errno);

	wasPlaying = false;
	playing = false;
}

void CDAudio_Pause(void)
{
	if (cdfile == -1 || !enabled)
		return;

	if (!playing)
		return;

	if ( ioctl(cdfile, CDROMPAUSE) == -1 )
		Con_DPrintf("CD pause ioctl failed\n");

	wasPlaying = playing;
	playing = false;
}


void CDAudio_Resume(void)
{
	if (cdfile == -1 || !enabled)
		return;

	if (!cdValid)
		return;

	if (!wasPlaying)
		return;

	if ( ioctl(cdfile, CDROMRESUME) == -1 )
		Con_DPrintf("CD resume ioctl failed\n");
	playing = true;
}


void CDAudio_Update(void)
{
#if defined(USE_LINUX_CD)
	struct cdrom_subchnl subchnl;
#elif defined(USE_BSD_CD)
	struct ioc_read_subchannel subchnl;
	struct cd_sub_channel_info subchnldata;
#endif
	static time_t lastchk;

	if (!enabled)
		return;

	if (bgmvolume->value != cdvolume)
	{
		if (cdvolume)
		{
			bgmvolume->value = cdvolume = 0.0;
			CDAudio_Pause ();
		}
		else
		{
			bgmvolume->value = cdvolume = 1.0;
			CDAudio_Resume ();
		}
	}

	if (playing && lastchk < time(NULL)) {
		lastchk = time(NULL) + 2; //two seconds between chks
#if defined(USE_LINUX_CD)
		subchnl.cdsc_format	= CDROM_MSF;
#elif defined(USE_BSD_CD)
		subchnl.address_format	= CD_MSF_FORMAT;
		subchnl.data_format	= CD_CURRENT_POSITION;
		subchnl.track		= 0;
		subchnl.data_len	= sizeof(subchnldata);
		subchnl.data		= &subchnldata;
#endif
		if (ioctl(cdfile, CDROMSUBCHNL, &subchnl) == -1 ) {
			Con_DPrintf("CD subcchannel ioctl failed\n");
			playing = false;
			return;
		}
#if defined(USE_LINUX_CD)
		if (subchnl.cdsc_audiostatus != CDROM_AUDIO_PLAY &&
			subchnl.cdsc_audiostatus != CDROM_AUDIO_PAUSED)
#elif defined(USE_BSD_CD)
		if (subchnl.data->header.audio_status != CD_AS_PLAY_IN_PROGRESS
		    && subchnl.data->header.audio_status != CD_AS_PLAY_PAUSED)
#endif
		{
			playing = false;
			if (playLooping)
				CDAudio_Play(playTrack, true);
		}
	}
}

int CDAudio_Init(void)
{
	int i;

#ifdef UQUAKE
	if (cls.state == ca_dedicated)
		return -1;
#endif

	if (COM_CheckParm("-nocdaudio"))
		return -1;

	if ((i = COM_CheckParm("-cddev")) != 0 && i < com_argc - 1) {
		strncpy(cd_dev, com_argv[i + 1], sizeof(cd_dev));
		cd_dev[sizeof(cd_dev) - 1] = 0;
	}

	if ((cdfile = open(cd_dev, O_RDONLY)) == -1) {
		Con_Printf("CDAudio_Init: open of \"%s\" failed (%i)\n", cd_dev, errno);
		cdfile = -1;
		return -1;
	}

	for (i = 0; i < 100; i++)
		remap[i] = i;
	initialized = true;
	enabled = true;

	if (CDAudio_GetAudioDiskInfo())
	{
		Con_Printf("CDAudio_Init: No CD in player.\n");
		cdValid = false;
	}

	Cmd_AddCommand ("cd", CD_f);

	Con_Printf("CD Audio Initialized\n");

	return 0;
}


void CDAudio_Shutdown(void)
{
	if (!initialized)
		return;
	CDAudio_Stop();
	close(cdfile);
	cdfile = -1;
}
