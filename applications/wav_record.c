#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <rtthread.h>
#include <finsh.h>

#include <drivers/audio.h>

#define RECORD_SECOND       10
#define RECORD_SAMPLERATE   16000
#define RECORD_CHUNK_SZ     ((RECORD_SAMPLERATE * 2) * 20 / 1000)

struct wav_header
{
    char riff_id[4];            //"RIFF"
    uint32_t size0;             //file len - 8
    char wave_fmt[8];           //"WAVEfmt "
    uint32_t size1;             //0x10
    uint16_t fmttag;            //0x01
    uint16_t channel;           //1
    uint32_t samplespersec;     //8000
    uint32_t bytepersec;        //8000 * 2
    uint16_t blockalign;        //1 * 16 / 8
    uint16_t bitpersamples;     //16
    char data_id[4];            //"data"
    uint32_t size2;             //file len - 44
};

static void init_wav_header(uint8_t *wav_buf, uint32_t wav_len, int samplerate)
{
    struct wav_header *header = (struct wav_header *)wav_buf;

    strcpy(header->riff_id, "RIFF");
    header->size0 = wav_len - 8;
    strcpy(header->wave_fmt, "WAVEfmt ");
    header->size1 = 16;
    header->fmttag = 1;
    header->channel = 1;
    header->samplespersec = samplerate;
    header->bytepersec = samplerate * 2;
    header->blockalign = 2;
    header->bitpersamples = 16;
    strcpy(header->data_id, "data");
    header->size2 = wav_len - 44;
}

int wavrecord(int argc, char** argv)
{
    FILE *fp = NULL;
    struct rt_device *audio;
    int second = 10;
    int rate = RECORD_SAMPLERATE;
    uint8_t *audio_buffer = NULL;
    struct wav_header* header = NULL;

    if (argc != 2 && argc != 3)
    {
        printf("Usage:\n");
        printf("%s file.wav\n", argv[0]);
        return -1;
    }

    fp = fopen(argv[1], "wb+");
    if (!fp)
    {
        printf("open file for recording failed!\n");
        return -1;
    }
    if (argc == 3) second = atoi(argv[2]);

    audio_buffer = malloc (RECORD_CHUNK_SZ);
    if (!audio_buffer) goto __exit;

    audio = rt_device_find("sound");
    if (audio)
    {
        header = (struct wav_header*)malloc (sizeof(struct wav_header));
        if (!header) goto __exit;
        /* write a dummy header */
        memset(header, 0x0, sizeof(struct wav_header));
        fwrite(header, sizeof(struct wav_header), 1, fp);

        if (rt_device_open(audio, RT_DEVICE_OFLAG_RDONLY) != RT_EOK)
            rt_kprintf("open device failed!\n");

        /* set sample rate */
        rt_device_control(audio, CODEC_CMD_SET_SAMPLERATE, &rate);

        {
            uint8_t *ptr;
            int length, total_length = 0, second_count = 0;

            ptr = audio_buffer;
            while (second_count < second)
            {
                length = rt_device_read(audio, -1, ptr, RECORD_CHUNK_SZ);
                if (length)
                {
                    fwrite(audio_buffer, length, 1, fp);
                    total_length += length;
                }

                if (total_length % (RECORD_SAMPLERATE * 2) == 0)
                {
                    printf("* %d\n", second_count);
                    second_count += 1;
                }
            }

            /* re-write wav header */
            init_wav_header((uint8_t*)header, total_length, 16000);
            fseek(fp, 0, SEEK_SET);
            fwrite(header, sizeof(struct wav_header), 1, fp);
            fclose(fp); fp = NULL;
        }

        rt_device_close(audio);
    }

__exit:
    if (fp) fclose(fp);
    if (audio_buffer) free(audio_buffer);
    if (header) free(header);

    return 0;
}
MSH_CMD_EXPORT(wavrecord, record voice to a wav file);
