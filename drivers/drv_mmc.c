#include <rtthread.h>
#include <rtdevice.h>
#include <string.h>

#include "drv_mmc.h"
#include "sdmmc_cmd.h"
#include "driver/sdmmc_host.h"
#include "driver/sdmmc_defs.h"

struct esp_sdmmc_device
{
    struct rt_device parent;

    sdmmc_host_t host;
    sdmmc_card_t card;

    uint8_t *sector_buffer;
};
static struct esp_sdmmc_device _sdmmc;

static int need_copy(uint32_t addr)
{
    if (addr >= 0x3F800000 && addr <= 0x3F800000 + 4 * 1024 * 1024)
        return 1;

    return 0;
}

static rt_size_t esp_sdmmc_read(rt_device_t dev, rt_off_t pos, void *buffer, rt_size_t size)
{
    esp_err_t err;
    sdmmc_card_t *card;
    struct esp_sdmmc_device *sdmmc = (struct esp_sdmmc_device *)dev;

    // rt_kprintf("sdmmc: read %d blocks @ %d to 0x%08x\n", size, pos, buffer);

    card = &(sdmmc->card);

    if (((uint32_t)buffer & 0x03) || need_copy((uint32_t) buffer))
    {
        int index;
        uint8_t *ptr;

        ptr = (uint8_t*)buffer;
        for (index = 0; index < size; index ++)
        {
            err = sdmmc_read_sectors(card, sdmmc->sector_buffer, pos, 1);
            if (err == ESP_OK)
            {
                memcpy(ptr, sdmmc->sector_buffer,  card->csd.sector_size);

                ptr += card->csd.sector_size;
                pos += 1;
            }
            else
            {
                return 0;
            }
        }

        return size;
    }
    else
    {
        err = sdmmc_read_sectors(card, buffer, pos, size);
        if (err == ESP_OK)
        {
            return size;
        }
    }

    return 0;
}

static rt_size_t esp_sdmmc_write(rt_device_t dev, rt_off_t pos, const void *buffer, rt_size_t size)
{
    esp_err_t err;
    sdmmc_card_t *card;
    struct esp_sdmmc_device *sdmmc = (struct esp_sdmmc_device *)dev;

    // rt_kprintf("sdmmc: write %d blocks @ %d to 0x%08x\n", size, pos, buffer);

    card = &(sdmmc->card);

    if (((uint32_t)buffer & 0x03) || need_copy((uint32_t) buffer))
    {
        int index;
        uint8_t *ptr;

        ptr = (uint8_t*)buffer;
        for (index = 0; index < size; index ++)
        {
            memcpy(sdmmc->sector_buffer, ptr, card->csd.sector_size);

            err = sdmmc_write_sectors(card, sdmmc->sector_buffer, pos, 1);
            if (err == ESP_OK)
            {
                ptr += card->csd.sector_size;
                pos += 1;
            }
            else
            {
                return 0;
            }
        }

        return size;
    }
    else
    {
        err = sdmmc_write_sectors(card, buffer, pos, size);
        if (err == ESP_OK)
        {
            return size;
        }
    }

    return 0;
}

static rt_err_t esp_sdmmc_control(rt_device_t dev, rt_uint8_t cmd, void *args)
{
    struct esp_sdmmc_device *sdmmc = (struct esp_sdmmc_device *)dev;

    RT_ASSERT(dev != RT_NULL);

    if (cmd == RT_DEVICE_CTRL_BLK_GETGEOME)
    {
        struct rt_device_blk_geometry *geometry;
        sdmmc_card_t *card;

        card = &(sdmmc->card);
        geometry = (struct rt_device_blk_geometry *)args;
        if (geometry == RT_NULL) return -RT_ERROR;

        geometry->bytes_per_sector = card->csd.sector_size;
        geometry->block_size       = card->csd.sector_size * 4;
        geometry->sector_count     = card->csd.capacity;
    }

    return RT_EOK;
}

int rt_hw_sdmmc_init(void)
{
    esp_err_t err;
    rt_device_t device;
    sdmmc_host_t _host = SDMMC_HOST_DEFAULT();
    sdmmc_slot_config_t slot_config = SDMMC_SLOT_CONFIG_DEFAULT();
    sdmmc_card_t *card;

    /* we use 1bit sdmmc */
    memcpy(&_sdmmc.host, &_host, sizeof(_host));
    // _sdmmc.host = SDMMC_HOST_DEFAULT();
    _sdmmc.host.flags = SDMMC_HOST_FLAG_1BIT;

    device = (rt_device_t)&(_sdmmc.parent);
    card = &(_sdmmc.card);

    // enable SDMMC
    sdmmc_host_init();

    // enable card slot
    err = sdmmc_host_init_slot(_sdmmc.host.slot, &slot_config);
    if (err != ESP_OK)
    {
        return -1;
    }

    // probe and initialize card
    err = sdmmc_card_init(&_sdmmc.host, card);
    if (err != ESP_OK)
    {
        rt_kprintf("sdmmc_card_init failed 0x(%x)", err);
        return -1;
    }

#if 0
    rt_kprintf("Name : %s\n", card->cid.name);
    rt_kprintf("Type : %s\n", (card->ocr & SD_OCR_SDHC_CAP)?"SDHC/SDXC":"SDSC");
    rt_kprintf("Speed: %s\n", (card->csd.tr_speed > 25000000)?"high speed":"default speed");
    rt_kprintf("Size : %dMB\n", ((uint64_t) card->csd.capacity) * card->csd.sector_size / (1024 * 1024));
    rt_kprintf("CSD  : ver=%d, sector_size=%d, capacity=%d read_bl_len=%d\n",
            card->csd.csd_ver,
            card->csd.sector_size, card->csd.capacity, card->csd.read_block_len);
    rt_kprintf("SCR  : sd_spec=%d, bus_width=%d\n", card->scr.sd_spec, card->scr.bus_width);
#endif

    _sdmmc.sector_buffer = (uint8_t*) malloc (card->csd.sector_size);

    /* register sdcard device */
    device->type    = RT_Device_Class_Block;

    device->init    = RT_NULL;
    device->open    = RT_NULL;
    device->close   = RT_NULL;
    device->read    = esp_sdmmc_read;
    device->write   = esp_sdmmc_write;
    device->control = esp_sdmmc_control;

    /* no private, no callback */
    device->user_data   = RT_NULL;
    device->rx_indicate = RT_NULL;
    device->tx_complete = RT_NULL;

    rt_device_register(device, "sd", RT_DEVICE_FLAG_RDWR | RT_DEVICE_FLAG_REMOVABLE );

    return 0;
}

