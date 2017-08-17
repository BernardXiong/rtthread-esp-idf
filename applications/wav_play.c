#include <rtthread.h>
#include <finsh.h>

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "audio_device.h"

#define BUFSZ   4096

struct RIFF_HEADER_DEF
{
    char riff_id[4];     // 'R','I','F','F'
    uint32_t riff_size;
    char riff_format[4]; // 'W','A','V','E'
};

struct WAVE_FORMAT_DEF
{
    uint16_t FormatTag;
    uint16_t Channels;
    uint32_t SamplesPerSec;
    uint32_t AvgBytesPerSec;
    uint16_t BlockAlign;
    uint16_t BitsPerSample;
};

struct FMT_BLOCK_DEF
{
    char fmt_id[4];    // 'f','m','t',' '
    uint32_t fmt_size;
    struct WAVE_FORMAT_DEF wav_format;
};

struct DATA_BLOCK_DEF
{
    char data_id[4];     // 'R','I','F','F'
    uint32_t data_size;
};

struct wav_info
{
    struct RIFF_HEADER_DEF header;
    struct FMT_BLOCK_DEF   fmt_block;
    struct DATA_BLOCK_DEF  data_block;
};

int wavplay(int argc, char** argv)
{
    FILE *fp = NULL;
    uint16_t *buffer = NULL;
    struct wav_info *info = NULL;

    if (argc != 2)
    {
        printf("Usage:\n");
        printf("wavplay song.wav\n");
        return 0;
    }

    fp = fopen(argv[1], "rb");
    if (!fp) {printf("open file failed!\n"); goto __exit;}

    info = (struct wav_info*) malloc (sizeof (*info));
    if (!info) goto __exit;

    if (fread(&(info->header),     sizeof(struct RIFF_HEADER_DEF), 1, fp) != 1) goto __exit;
    if (fread(&(info->fmt_block),  sizeof(struct FMT_BLOCK_DEF),   1, fp) != 1) goto __exit;
    if (fread(&(info->data_block), sizeof(struct DATA_BLOCK_DEF),  1, fp) != 1) goto __exit;

    printf("wav information:\n");
    printf("     samplerate %d\n", info->fmt_block.wav_format.SamplesPerSec);
	printf("        channel %d\n", info->fmt_block.wav_format.Channels);
	
    audio_device_open();
    audio_device_set_rate(info->fmt_block.wav_format.SamplesPerSec);

    while (!feof(fp))
    {
        int length;

        buffer = (uint16_t*)audio_device_get_buffer(RT_NULL);

        length = fread(buffer, 1, BUFSZ, fp);
        if (length)
        {
            if (info->fmt_block.wav_format.Channels == 1)
            {
                /* extend to stereo channels */
                int index;
                uint16_t *ptr;

                ptr = (uint16_t*)((uint8_t*)buffer + BUFSZ * 2);
                for (index = 1; index < BUFSZ/2; index ++)
                {
                    *ptr = *(ptr - 1) = buffer[BUFSZ/2 - index];
                    ptr -= 2;
                }

                length = length * 2;
            }

            audio_device_write((uint8_t*)buffer, length);
        }
        else
        {
            audio_device_put_buffer((uint8_t*)buffer);
            break;
        }
    }
    audio_device_close();

__exit:
    if (fp) fclose(fp);
    if (info) free(info);

    return 0;
}
MSH_CMD_EXPORT(wavplay, wavplay song.wav);
