/*
 *  Copyright (C) 2015 Stas Sergeev <stsp@users.sourceforge.net>
 *
 * The below copyright strings have to be distributed unchanged together
 * with this file. This prefix can not be modified or separated.
 */

/*
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

/*
 * Purpose: libao file writer output plugin.
 *
 * libao is so lame that we need a completely different handling
 * of the file writing interface.
 *
 * Author: Stas Sergeev.
 */

#include "emu.h"
#include "init.h"
#include "sound/sound.h"
#include <stdio.h>
#include <ao/ao.h>

#define ENABLED 0

static const char *aosndf_name = "Sound Output: libao wav writer";
static ao_device *ao;
#if ENABLED
static const char *ao_drv_manual_name = "wav";
#else
static const char *ao_drv_manual_name = "null";
#endif
static const char *file_name = "/tmp/ao.wav";
static struct player_params params;
static int started;

static int aosndf_open(void *arg)
{
    ao_sample_format info = {};
    int id;
    params.rate = 44100;
    params.format = PCM_FORMAT_S16_LE;
    params.channels = 2;
    info.channels = params.channels;
    info.rate = params.rate;
    info.byte_format = AO_FMT_LITTLE;
    info.bits = 16;
    id = ao_driver_id(ao_drv_manual_name);
    if (id == -1)
	return 0;
    ao = ao_open_file(id, file_name, 1, &info, NULL);
    if (!ao) {
#if ENABLED
	error("libao: opening %s failed\n", file_name);
#endif
	return 0;
    }
    return 1;
}

static void aosndf_close(void *arg)
{
    ao_close(ao);
}

static void aosndf_start(void *arg)
{
    started = 1;
}

static void aosndf_stop(void *arg)
{
    started = 0;
}

static void aosndf_timer(double dtime, void *arg)
{
    #define BUF_SIZE 1024
    char buf[BUF_SIZE];
    ssize_t size, total;
    if (!started)
	return;
    total = pcm_frag_size(dtime, &params);
    if (total < BUF_SIZE)
	return;
    while (total) {
	size = total;
	if (size > BUF_SIZE)
	    size = BUF_SIZE;
	size = pcm_data_get(buf, size, &params);
	if (!size)
	    break;
	ao_play(ao, buf, size);
	total -= size;
    }
}

CONSTRUCTOR(static void aosndf_init(void))
{
    struct pcm_player player = {};
    player.name = aosndf_name;
    player.open = aosndf_open;
    player.close = aosndf_close;
    player.start = aosndf_start;
    player.stop = aosndf_stop;
    player.timer = aosndf_timer;
    player.id = PCM_ID_P;
    params.handle = pcm_register_player(player);
}