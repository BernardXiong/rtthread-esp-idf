#include <stdint.h>

#include <rthw.h>
#include <rtthread.h>
#include <rtdevice.h>
#include <board.h>

#include <drivers/audio.h>

#include "driver/i2s.h"
#include "driver/i2c.h"

#include "codec_es8388.h"

#ifdef RT_USING_FINSH
#include <finsh.h>
extern void set_volume(int volume);
static void es8388_reg_write_finsh(uint8_t address, uint8_t data);

FINSH_FUNCTION_EXPORT_ALIAS(set_volume, vol, set codec volume);
FINSH_FUNCTION_EXPORT_ALIAS(es8388_reg_write_finsh, es8388, write ES8388 register);
#endif /* RT_USING_FINSH */

// #define CODEC_DEBUG

#ifdef CODEC_DEBUG
#define ES8388_DEBUG(...)         rt_kprintf("[ES8388] %d ", rt_tick_get()); rt_kprintf(__VA_ARGS__);
#else
#define ES8388_DEBUG(...)
#endif /* CODEC_DEBUG */

#define I2S_BCK_IO      5
#define I2S_WS_IO       25
#define I2S_DOUT_IO     26
#define I2S_DIN_IO      35

#define codec_printf    rt_kprintf

#ifndef ES8388_IIC_ADDR
#error  "no es8388"
#endif

// #define ES8388_IIC_ADDR 0x11

#define CH_MONO         I2S_CHANNEL_FMT_ALL_RIGHT
#define CH_STEREO       I2S_CHANNEL_FMT_RIGHT_LEFT

#define DMA_BUF_COUNT 	4
#define DMA_BUFSZ		640

#define CODEC_MODE_TX	0x01
#define CODEC_MODE_RX	0x02

/* data node for Tx Mode */
struct codec_data_node
{
    char *data_ptr;
    rt_size_t  data_size;
};

struct codec_device
{
    /* inherit from rt_device */
    struct rt_device parent;
    struct rt_data_queue data_queue;

    i2s_port_t i2s_obj;
	/* codec mode */
	int mode;

    /* i2c mode */
    struct rt_i2c_bus_device * i2c_device;
};
static struct codec_device es8388_codec;

void wait_codec_free(void);

int i2s_init(i2s_port_t i2s_num)
{
    i2s_config_t i2s_config = {
            .mode = I2S_MODE_MASTER | I2S_MODE_TX,
            .sample_rate = 8000,
            .bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT,
            .channel_format = I2S_CHANNEL_FMT_RIGHT_LEFT,   // 2-channels
            .communication_format = I2S_COMM_FORMAT_I2S,
            .dma_buf_count = 4,                             // number of buffers, 128 max.
            .dma_buf_len = 640,                             // size of each buffer
            .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1        // Interrupt level 1
    };

    i2s_pin_config_t pin_config = {
        .bck_io_num = I2S_BCK_IO,
        .ws_io_num = I2S_WS_IO,
        .data_out_num = I2S_DOUT_IO,
        .data_in_num = I2S_DIN_IO
    };

    i2s_driver_install(i2s_num, &i2s_config, 0, NULL);
    i2s_set_pin(i2s_num, &pin_config);

    // set MCLK
    PIN_FUNC_SELECT(PERIPHS_IO_MUX_GPIO0_U, FUNC_GPIO0_CLK_OUT1);
    SET_PERI_REG_BITS(PIN_CTRL, CLK_OUT1, 0, CLK_OUT1_S);

    return 0;
}

int codec_i2s_init(i2s_port_t i2s_num, const i2s_config_t *i2s_config)
{
    i2s_pin_config_t pin_config = {
        .bck_io_num = I2S_BCK_IO,
        .ws_io_num = I2S_WS_IO,
        .data_out_num = I2S_DOUT_IO,
        .data_in_num = I2S_DIN_IO
    };

    i2s_driver_install(i2s_num, i2s_config, 0, NULL);
    i2s_set_pin(i2s_num, &pin_config);

    // set MCLK
    PIN_FUNC_SELECT(PERIPHS_IO_MUX_GPIO0_U, FUNC_GPIO0_CLK_OUT1);
    SET_PERI_REG_BITS(PIN_CTRL, CLK_OUT1, 0, CLK_OUT1_S);

    return 0;
}

static uint8_t es8388_reg_read(struct codec_device *codec, uint8_t address)
{
    struct rt_i2c_msg msg[2];
    uint8_t data;

    RT_ASSERT(codec->i2c_device != RT_NULL);

    msg[0].addr = ES8388_IIC_ADDR; /* CE pull-up */
    msg[0].flags = RT_I2C_WR;
    msg[0].len = 1;
    msg[0].buf = &address;

    msg[1].addr = ES8388_IIC_ADDR; /* CE pull-up */
    msg[1].flags = RT_I2C_RD;
    msg[1].len = 1;
    msg[1].buf = &data;

    rt_i2c_transfer(codec->i2c_device, msg, 2);

    return data;
}

static void es8388_write_nocheck(struct codec_device *codec, uint8_t address, uint8_t data)
{
    struct rt_i2c_msg msg;
    rt_uint8_t send_buffer[2];

    ES8388_DEBUG("write 0x%02X ==> %d\n", data, address);

    RT_ASSERT(codec->i2c_device != RT_NULL);

    send_buffer[0] = address;
    send_buffer[1] = data;

    msg.addr = ES8388_IIC_ADDR; /* CE pull-up */
    msg.flags = RT_I2C_WR;
    msg.len = 2;
    msg.buf = send_buffer;

    rt_i2c_transfer(codec->i2c_device, &msg, 1);
}

static void es8388_reg_write(struct codec_device *codec, uint8_t address, uint8_t data)
{
    uint8_t tmp;
    int retry = 100;

    es8388_write_nocheck(codec, address, data);

    while(1)
    {
        tmp = es8388_reg_read(codec, address);
        if(tmp == data)
        {
            ES8388_DEBUG("check pass, %d %02X:%02X\n", address, data, tmp);
            break;
        }

        ES8388_DEBUG("check %d %02X:%02X\n", address, data, tmp);

        if( !retry-- )
        {
            rt_kprintf("register address %d check timeout!, %02X:%02X\n", address, data, tmp);
            break;
        }

        rt_thread_delay(1);
    }
}

void vol(struct codec_device *codec, uint16_t v) // 0~99
{
    if(v > 100)
        v = 100;

    v = 33*v/100; /* 0~33 */

    ES8388_DEBUG("ES8388S volume set to %d\n", v);

    /*
    ES8388  use R46/R47(OUT1VOL) and R48/R49(OUT2VOL),
    ES8388S use R48/R49(OUTVOL).
    */

    es8388_write_nocheck(codec, 46, v);
    es8388_write_nocheck(codec, 47, v);

    es8388_reg_write(codec, 48, v);
    es8388_reg_write(codec, 49, v);
}

void es8388_reset(struct codec_device *codec)
{
    es8388_write_nocheck(codec, ES8388_CONTROL1, 0x80);
    rt_thread_delay(10);
    es8388_write_nocheck(codec, ES8388_CONTROL1, 0x00);
    rt_thread_delay(10);
}

static void es8388_suspend(struct codec_device *codec)
{
    es8388_write_nocheck(codec, 0x0F, 0x34 ); // Reg 15 0x0F = 0x34 (ADC Mute), ES8388S=0x24
    es8388_write_nocheck(codec, 0x19, 0x36 ); // Reg 25 0x19 = 0x36 (DAC Mute), ES8388S=0x26
    es8388_reg_write(codec, 0x02, 0xF3 ); // Reg 02 0x02 = 0xF3 Power down DEM and STM.
    es8388_write_nocheck(codec, 0x03, 0xFC ); // Reg 03 0x03 = 0xFC Power Down ADC / Analog Input / Micbias for Record. ES8388S=0xAC
    es8388_reg_write(codec, 0x04, 0xC0 ); // Reg 04 0x04 = 0xC0 Power down DAC and disable LOUT/ROUIT.

    es8388_reg_write(codec, 0x2B, 0x90 );
    es8388_write_nocheck(codec, 0x05, 0xFF ); // 0xFF, ES8388S=0xE8
    es8388_write_nocheck(codec, 0x06, 0xFF ); // 0xFF, ES8388S=0xC3
    es8388_reg_write(codec, 0x27, 0x38 );
    es8388_reg_write(codec, 0x2A, 0x38 );
    es8388_reg_write(codec, 0x01, 0x7A ); // 3A ==> 7A
}

static rt_err_t codec_init(rt_device_t dev)
{
    struct codec_device *codec = (struct codec_device *)dev;

    // set chip to slave mode
    es8388_reg_write(codec, ES8388_MASTERMODE, 0x00);       // slave mode.

    // power down DEM and STM of ADC and DAC
    es8388_reg_write(codec, 2, R02_DACVREF_DOWN | R02_ADCVREF_DOWN | R02_ADC_STM_RST | R02_DAC_STM_RST | R02_DAC_DIG_PDN | R02_ADC_DIG_PDN);

    // set DACLRC and ADCLRC same
    es8388_reg_write(codec, 43, R43_SLRCK_SAME | R43_DAC_ANACLK_UP | R43_ADC_ANACLK_UP | R43_MCLK_EN | R43_OFFSET_EN);
    // set chip to play&record mode
    //es8388_reg_write(codec, 0, R00_VMID_500K | R00_REF_EN);
    es8388_reg_write(codec, 0, 0x36);
    // power up analog and lbias
    es8388_write_nocheck(codec, 1, R01_VREFBUF_ON | R01_VREF_ON | R01_IBIASGEN_ON | R01_ANA_ON);

    // Power up DAC and analog Output. R04_ROUT1_EN | R04_LOUT1_EN |
    //es8388_reg_write(codec, 4, R04_ROUT_EN | R04_LOUT_EN | R04_DACR_UP | R04_DACL_UP);
    es8388_write_nocheck(codec, 4, R04_ROUT_EN | R04_LOUT_EN | R04_ROUT1_EN | R04_LOUT1_EN | R04_DACR_UP | R04_DACL_UP);

    es8388_reg_write(codec, 23, R23_DAC_FMT_I2S | R23_DAC_DACWL_16BIT);
    es8388_reg_write(codec, 24, R24_DAC_RATIO_AUTO); /*  */

    // Set DAC Digital Volume
    es8388_reg_write(codec, 26, 0x00); /* left  DAC digital volume control, 0DB. */
    es8388_reg_write(codec, 27, 0x00); /* right DAC digital volume control, 0DB. */

    // UnMute DAC
    es8388_reg_write(codec, 25, R25_DAC_UNMUTE | R25_DAC_SOFT_RAMP_DIS | R25_DAC_RAMP_4LRCK);

    /******************* ADC config begin *******************/
    // Power up ADC / Analog Input / Micbias for Record
    es8388_reg_write(codec, 3, R03_INT_NORMAL | R03_ADC_NORMAL | R03_ADC_BIASGEN_NORMAL | R03_MICB_NORMAL | R03_ADCR_NORMAL | R03_ADCL_NORMAL | R03_AINR_NORMAL | R03_AINL_NORMAL);
    // Select Analog input channel for ADC,  R10_RINSEL_DIFFERENTIAL | R10_DSSEL_ONE
    es8388_reg_write(codec, 10, R10_CAP_MODE_DIS | R10_LINSEL_DIFFERENTIAL);
    es8388_reg_write(codec, 11, R11_ASDOUT_EN | R11_DS_LIN1_RIN1 | 0x02); // default 110 => 010
    // Select  PGA  Gain  for  ADC analog input
    es8388_reg_write(codec, 9, R09_MICAMPL_DB(24) ); /* 0~24db. ==> max33 */
    // digital interface.
    es8388_reg_write(codec, 12, R12_ADC_FMT_I2S | R12_ADCWL_16BIT);
    es8388_reg_write(codec, 13, R13_ADC_RATIO_AUTO); /* ADC ratio selection for slave mode. */
    // Set ADC Digital Volume
    es8388_reg_write(codec, 16, 0); /* left  ADC Digital Volume */
    es8388_reg_write(codec, 17, 0); /* right ADC Digital volume, ES8388 only. */
    // UnMute ADC
    es8388_reg_write(codec, 15, R15_ADC_UNMUTE | R15_ADC_SOFT_RAMP_DIS | R15_ADC_RAMP_4LRCK);

    // Set ALC mode if necessary
    es8388_reg_write(codec, 14, R14_ADC_LHPF_EN ); /* R14_ADC_RHPF_EN */
    es8388_reg_write(codec, 19, R19_ADC_ALC_LVL_DB(-1.5) | R19_ADC_ALC_HLD(0) );
    es8388_reg_write(codec, 20, R20_ADC_ALCDCY_410US | R20_ADC_ALCATK_104US); // 48=> 8, 1:6
    es8388_reg_write(codec, 21, R21_ADC_ALCMODE | R21_ADC_ALCZC_OFF | R21_ADC_TIME_OUT_OFF | R21_ADC_ALC_WIN_SIZE_96);
    es8388_reg_write(codec, 22, R22_ADC_ALC_NGAT_EN | R22_ADC_ALC_NGTH_DB(-33) | R22_ADC_ALC_NGG_PGA_HELD); /* -30 ~ -76.5, 1.5db/setp */
    es8388_reg_write(codec, 18, R18_ADC_ALC_LEFT | R18_ALC_MAXGAIN_23_5DB | R18_ALC_MINGAIN_0DB);
    /******************* ADC config end *******************/

    // Set Mixer for PA Output
    es8388_reg_write(codec, 38, R38_RMIXSEL_ADC | R38_LMIXSEL_ADC);
    es8388_reg_write(codec, 39, R39_LI2LO_DIS | R39_LI2LOVOL_GAIN_0DB | R39_LD2LO_EN);
    //es8388_reg_write(codec, 40, 0x28); //@TODO: no description, write 38 but return 28
    //es8388_reg_write(codec, 41, 0x28); //@TODO: no description, write 38 but return 28
    es8388_reg_write(codec, 42, R42_RI2RO_DIS | R42_RI2ROVOL_GAIN_0DB | R42_RD2RO_EN);

    // Set output volume.
    vol(codec, 0);

    // Power up DEM and STM
    es8388_reg_write(codec, 2, R02_DACVREF_UP | R02_ADCVREF_UP | R02_DACDLL_UP | R02_ADCDLL_UP);

    // Set output volume.
    vol(codec, 65);

    // CHIP I2S init
    i2s_init(codec->i2s_obj);
	i2s_stop(codec->i2s_obj);

    return RT_EOK;
}

rt_err_t sample_rate(struct codec_device *codec, int sr)
{
    sr &= ~(0x01);
    i2s_set_sample_rates(codec->i2s_obj, sr);

    return RT_EOK;
}

static rt_err_t codec_open(rt_device_t dev, rt_uint16_t oflag)
{
	// int mode, rate, bits, ch;
    rt_err_t result = RT_EOK;
    struct codec_device *codec = (struct codec_device *)dev;

    i2s_config_t i2s_config = {
            .mode = I2S_MODE_MASTER,
            .sample_rate = 16000,
            .bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT,
            .channel_format = I2S_CHANNEL_FMT_RIGHT_LEFT,   // 2-channels
            .communication_format = I2S_COMM_FORMAT_I2S,
            .dma_buf_count = 8,                             // number of buffers, 128 max.
            .dma_buf_len = 320,                             // size of each buffer
            .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1        // Interrupt level 1
    };

    if (oflag & RT_DEVICE_OFLAG_RDONLY)
    {
		codec->mode |= CODEC_MODE_RX;
		i2s_config.mode |= I2S_MODE_RX;
		i2s_config.channel_format = I2S_CHANNEL_FMT_ONLY_RIGHT;
    }

	if (oflag & RT_DEVICE_OFLAG_WRONLY)
	{
		codec->mode |= CODEC_MODE_TX;
		i2s_config.mode |= I2S_MODE_TX;
		i2s_config.channel_format = I2S_CHANNEL_FMT_RIGHT_LEFT;
	}

    /* re-init codec */
	if (codec_i2s_init(codec->i2s_obj, &i2s_config) != ESP_OK)
		result = -RT_EIO;

    return result;
}

static rt_err_t codec_close(rt_device_t dev)
{
    rt_err_t result = RT_EOK;
    struct codec_device *codec = (struct codec_device *)dev;

	/* wait codec freed */
	if (codec->mode & CODEC_MODE_TX)
		wait_codec_free();

	/* clean codec mode */
	codec->mode = 0;

	/* disable i2s */
	i2s_driver_uninstall(codec->i2s_obj);

    return result;
}

static rt_err_t codec_control(rt_device_t dev, rt_uint8_t cmd, void *args)
{
    rt_err_t result = RT_EOK;
    struct codec_device *codec = (struct codec_device *)dev;

    switch (cmd)
    {
    case RT_DEVICE_CTRL_SUSPEND:
        es8388_suspend(codec);
        break;

    case CODEC_CMD_RESET:
        result = codec_init(dev);
        break;

    case CODEC_CMD_SET_VOLUME:
        vol(codec, *((uint16_t*) args));
        break;

    case CODEC_CMD_SET_SAMPLERATE:
        result = sample_rate(codec, *((int*) args));
        break;

    default:
        result = RT_ERROR;
    }
    return result;
}

static rt_size_t codec_read(rt_device_t dev, rt_off_t pos,
                             void* buffer, rt_size_t size)
{
	rt_size_t read_bytes = 0;
    struct codec_device *codec = (struct codec_device *)dev;

	read_bytes = i2s_read_bytes(codec->i2s_obj, buffer, size, portMAX_DELAY);
    return read_bytes;
}

static rt_size_t codec_write(rt_device_t dev, rt_off_t pos,
                             const void* buffer, rt_size_t size)
{
    struct codec_device *codec = (struct codec_device *)dev;

    rt_data_queue_push(&(codec->data_queue), buffer, size, RT_WAITING_FOREVER);
    return size;
}

/******************************************/

void i2s_dataqueue_thread(void* parameter)
{
    rt_uint8_t *data_ptr;
    rt_size_t size;

    struct codec_device *codec = &es8388_codec;

    while (1)
    {
        if (rt_data_queue_pop(&(codec->data_queue), (const void**)&data_ptr, &size, RT_WAITING_FOREVER) == RT_EOK)
        {
            i2s_write_bytes(codec->i2s_obj, (char*)data_ptr, size, portMAX_DELAY);

            /* write done */
            if (codec->parent.tx_complete)
            {
                codec->parent.tx_complete(&(codec->parent), data_ptr);
            }
        }
    }
}

rt_err_t codec_hw_init(const char * i2c_bus_device_name)
{
    struct rt_i2c_bus_device * i2c_device;
    struct codec_device *codec = &es8388_codec;

    i2c_device = rt_i2c_bus_device_find(i2c_bus_device_name);
    if(i2c_device == RT_NULL)
    {
        codec_printf("i2c bus device %s not found!\r\n", i2c_bus_device_name);
        return -RT_ENOSYS;
    }
    codec->i2c_device = i2c_device;

    codec->i2s_obj = I2S_NUM_0;

    codec->parent.type = RT_Device_Class_Sound;
    codec->parent.rx_indicate = RT_NULL;
    codec->parent.tx_complete = RT_NULL;
    codec->parent.user_data   = RT_NULL;

    codec->parent.control = codec_control;
    codec->parent.init    = codec_init;
    codec->parent.open    = codec_open;
    codec->parent.close   = codec_close;
    codec->parent.read    = codec_read;
    codec->parent.write   = codec_write;

    rt_data_queue_init(&codec->data_queue, 8, 0, NULL);
    {
        rt_thread_t tid;
        tid = rt_thread_create("es8388", i2s_dataqueue_thread, RT_NULL,
            2048, 8, 10);
        if (tid) rt_thread_startup(tid);
    }

    /* register the device */
    rt_device_register(&codec->parent, "sound", 
    	RT_DEVICE_FLAG_STANDALONE | RT_DEVICE_FLAG_WRONLY | RT_DEVICE_FLAG_DMA_TX);
    rt_device_init(&codec->parent);

    return RT_EOK;
}

static void es8388_reg_write_finsh(uint8_t address, uint8_t data)
{
    es8388_reg_write(&es8388_codec, address, data);
}

void set_volume(int volume)
{
    vol(&es8388_codec, volume);
}

void wait_codec_free(void)
{
    struct codec_device *codec = &es8388_codec;

    while (codec->data_queue.get_index != codec->data_queue.put_index)
        rt_thread_delay(1);
}

void set_codec_standby(void)
{
    struct codec_device *codec = &es8388_codec;

    es8388_suspend(codec);
}

