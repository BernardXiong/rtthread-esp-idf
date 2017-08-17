#include <stdint.h>
#include <stdlib.h>

#include <rtthread.h>
#include <drivers/audio.h>

#ifdef RT_USING_ESP_PSRAM
#include <drv_sdram.h>
#endif

#include "audio_device.h"

#define AUDIO_DEVICE_MP_CNT                 (4)
#define AUDIO_DEVICE_DECODE_MP_BLOCK_SZ     (4352 * 2)
#define AUDIO_DEVICE_DECODE_MP_SZ           ((AUDIO_DEVICE_DECODE_MP_BLOCK_SZ*2 + 4)*AUDIO_DEVICE_MP_CNT)

struct audio_device
{
    struct rt_device *snd;
    struct rt_mempool mp;

    int state;

    void (*evt_handler)(void *parameter, int state);
    void *parameter;
};
static struct audio_device* _audio_device = NULL;

uint8_t* audio_device_get_buffer(int *bufsz)
{
    if (bufsz)
    {
        *bufsz = AUDIO_DEVICE_DECODE_MP_BLOCK_SZ * 2;
    }

    return (uint8_t *)rt_mp_alloc(&(_audio_device->mp), RT_WAITING_FOREVER);
}

void audio_device_put_buffer(uint8_t *ptr)
{
    if (ptr) rt_mp_free(ptr);
    return ;
}

static rt_err_t audio_device_write_done(struct rt_device *device, void* ptr)
{
    if(!ptr)
    {
        rt_kprintf("device buf_release NULL\n");
        return -RT_ERROR;
    }

    rt_mp_free(ptr);
    return RT_EOK;
}

void audio_device_write(uint8_t *buffer, int size)
{
    if (_audio_device->snd && size != 0)
    {
        if (_audio_device->state == AUDIO_DEVICE_IDLE)
        {
            if (_audio_device->evt_handler)
                _audio_device->evt_handler(_audio_device->parameter, AUDIO_DEVICE_PLAYBACK);

            /* change audio device state */
            _audio_device->state = AUDIO_DEVICE_PLAYBACK;
        }

        rt_device_write(_audio_device->snd, 0, buffer, size);
    }
    else
    {
        /* release buffer directly */
        rt_mp_free(buffer);
    }

    return ;
}

int audio_device_init(void)
{
    uint8_t *mempool_ptr;

	if (!_audio_device)
	{
#ifdef RT_USING_ESP_PSRAM
	    _audio_device = (struct audio_device*) sdram_malloc (sizeof (struct audio_device) + AUDIO_DEVICE_DECODE_MP_SZ);
#else
	    _audio_device = (struct audio_device*) malloc (sizeof (struct audio_device) + AUDIO_DEVICE_DECODE_MP_SZ);
#endif
	    if (_audio_device == NULL) return -RT_ERROR;

	    _audio_device->evt_handler = NULL;
	    _audio_device->parameter   = NULL;

	    mempool_ptr = (uint8_t *)(_audio_device + 1);
	    rt_mp_init(&(_audio_device->mp), "adbuf", mempool_ptr, AUDIO_DEVICE_DECODE_MP_SZ, AUDIO_DEVICE_DECODE_MP_BLOCK_SZ * 2);

	    /* find snd device */
	    _audio_device->snd = rt_device_find("sound");
	    if (_audio_device->snd == NULL) return -1;

	    /* set tx complete call back function */
	    rt_device_set_tx_complete(_audio_device->snd, audio_device_write_done);
	}

    return RT_EOK;
}

int audio_device_set_evt_handler(void (*handler)(void *parameter, int state), void *parameter)
{
    if (_audio_device)
    {
        _audio_device->evt_handler = handler;
        _audio_device->parameter   = parameter;
    }

    return 0;
}

void audio_device_set_rate(int sample_rate)
{
    if (_audio_device->snd)
    {
        int rate = sample_rate;

        rt_device_control(_audio_device->snd, CODEC_CMD_SET_SAMPLERATE, &rate);
    }
}

int audio_device_get_rate(void)
{
    int rate = 0;

    if (_audio_device->snd)
    {
        rt_device_control(_audio_device->snd, CODEC_CMD_GET_SAMPLERATE, &rate);
    }

    return rate;
}


void audio_device_set_volume(int value)
{
    if (_audio_device->snd)
    {
        rt_device_control(_audio_device->snd, CODEC_CMD_SET_VOLUME, &value);
    }
}

int audio_device_get_volume(void)
{
    int value = 0;

    if (_audio_device->snd)
    {
        rt_device_control(_audio_device->snd, CODEC_CMD_GET_VOLUME, &value);
    }

    return value;
}

void audio_device_wait_free(void)
{
	extern void wait_codec_free(void);
    wait_codec_free();
}

void audio_device_open(void)
{
    _audio_device->state = AUDIO_DEVICE_IDLE;
    rt_device_open(_audio_device->snd, RT_DEVICE_OFLAG_WRONLY);
}

void audio_device_close(void)
{
	rt_device_close(_audio_device->snd);

    if (_audio_device->state == AUDIO_DEVICE_PLAYBACK)
    {
        if (_audio_device->evt_handler)
            _audio_device->evt_handler(_audio_device->parameter, AUDIO_DEVICE_CLOSE);
    }

    /* set to idle */
    _audio_device->state = AUDIO_DEVICE_CLOSE;
}

