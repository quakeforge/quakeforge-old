/*
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
#include "quakedef.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#ifdef HAVE_SYS_IOCTL_H
# include <sys/ioctl.h>
#endif
#ifdef HAVE_SYS_MMAN_H
# include <sys/mman.h>
#endif
#if defined HAVE_SYS_SOUNDCARD_H
# include <sys/soundcard.h>
#elif defined HAVE_LINUX_SOUNDCARD_H
# include <linux/soundcard.h>
#elif HAVE_MACHINE_SOUNDCARD_H
# include <machine/soundcard.h>
#endif

#include <sys/asoundlib.h>

#ifndef MAP_FAILED
# define MAP_FAILED ((void*)-1)
#endif

static int tryrates[] = { 11025, 22051, 44100, 8000 };
static int snd_inited;

static snd_pcm_t *pcm_handle;
static struct snd_pcm_channel_info cinfo;
static struct snd_pcm_channel_params params;
static struct snd_pcm_channel_setup setup;
static snd_pcm_mmap_control_t *mmap_control = NULL;
static char *mmap_data = NULL;

qboolean SNDDMA_Init(void)
{
	int card=0,dev=0;
	int rc;
	char *err_msg="";

	if ((rc=snd_pcm_open(&pcm_handle,card,dev,SND_PCM_OPEN_PLAYBACK))<0) {
		Con_Printf( "Error: audio open error: %s\n", snd_strerror(rc));
		return 0;
	}

	memset(&cinfo, 0, sizeof(cinfo));
	cinfo.channel = SND_PCM_CHANNEL_PLAYBACK;
	snd_pcm_channel_info(pcm_handle, &cinfo);
	Con_Printf("%08x %08x\n",cinfo.flags,cinfo.formats);

	err_msg="audio flush";
	if ((rc=snd_pcm_flush_channel(pcm_handle, SND_PCM_CHANNEL_PLAYBACK))<0)
		goto error;
	err_msg="audio munmap";
	if ((rc=snd_pcm_munmap(pcm_handle, SND_PCM_CHANNEL_PLAYBACK))<0)
		goto error;

	memset(&params, 0, sizeof(params));
	params.channel = SND_PCM_CHANNEL_PLAYBACK;
	params.mode = SND_PCM_MODE_BLOCK;
	params.format.interleave=1;
	params.format.format=SND_PCM_SFMT_U8;
	params.format.rate=11025;
	params.format.voices=2;
	params.start_mode = SND_PCM_START_GO;
	params.stop_mode = SND_PCM_STOP_ROLLOVER;
	params.buf.block.frag_size=128;
	params.buf.block.frags_min=1;
	params.buf.block.frags_max=-1;
//	err_msg="audio flush";
//	if ((rc=snd_pcm_flush_channel(pcm_handle, SND_PCM_CHANNEL_PLAYBACK))<0)
//		goto error;
	err_msg="audio params";
	if ((rc=snd_pcm_channel_params(pcm_handle, &params))<0)
		goto error;

	err_msg="audio mmap";
	if ((rc=snd_pcm_mmap(pcm_handle, SND_PCM_CHANNEL_PLAYBACK, &mmap_control, (void **)&mmap_data))<0)
		goto error;
	err_msg="audio prepare";
	if ((rc=snd_pcm_plugin_prepare(pcm_handle, SND_PCM_CHANNEL_PLAYBACK))<0)
		goto error;

	memset(&setup, 0, sizeof(setup));
	setup.mode = SND_PCM_MODE_BLOCK;
	setup.channel = SND_PCM_CHANNEL_PLAYBACK;
	err_msg="audio setup";
	if ((rc=snd_pcm_channel_setup(pcm_handle, &setup))<0)
		goto error;

	shm=&sn;
	memset((dma_t*)shm,0,sizeof(*shm));
    shm->splitbuffer = 0;
	shm->channels=2;
	shm->samples=setup.buf.block.frags*setup.buf.block.frag_size;	// mono samples in buffer
	shm->submission_chunk=128;					// don't mix less than this #
	shm->samplepos=0;							// in mono samples
	shm->samplebits=8;
	shm->speed=11025;
	shm->buffer=(unsigned char*)mmap_data;

	snd_inited=1;
	return 1;
 error:
	Con_Printf( "Error: %s: %s\n", err_msg, snd_strerror(rc));
	snd_pcm_close(pcm_handle);
	return 0;
}

int SNDDMA_GetDMAPos(void)
{
	if (!snd_inited) return 0;
	shm->samplepos=(mmap_control->status.frag_io+1)*setup.buf.block.frag_size;
	return shm->samplepos;
}

void SNDDMA_Shutdown(void)
{
	if (snd_inited)
	{
		snd_pcm_close(pcm_handle);
		snd_inited = 0;
	}
}

/*
==============
SNDDMA_Submit

Send sound to device if buffer isn't really the dma buffer
===============
*/
void SNDDMA_Submit(void)
{
	int count=paintedtime-soundtime;
	int i,s,e,frags;
	int rc;

	count+=setup.buf.block.frag_size-1;
	count/=setup.buf.block.frag_size;
	s=soundtime/setup.buf.block.frag_size;
	e=s+count;
	for (i=s; i<e; i++)
		mmap_control->fragments[i % setup.buf.block.frags].data=1;
	switch (mmap_control->status.status) {
	case SND_PCM_STATUS_PREPARED:
		if ((rc=snd_pcm_channel_go(pcm_handle, SND_PCM_CHANNEL_PLAYBACK))<0) {
			fprintf(stderr, "unable to start playback. %s\n", 
					snd_strerror(rc));
			exit(1);
		}
		break;
	case SND_PCM_STATUS_RUNNING:
		break;
	case SND_PCM_STATUS_UNDERRUN:
		if ((rc=snd_pcm_plugin_prepare(pcm_handle, SND_PCM_CHANNEL_PLAYBACK))<0) {
			fprintf(stderr, "underrun: playback channel prepare error. %s\n", 
					snd_strerror(rc));
			exit(1);
		}
		break;
	default:
		break;
	}
}
