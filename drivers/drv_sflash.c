#include <stdint.h>
#include <string.h>

#include <rtthread.h>

#include "drv_sflash.h"
#include <esp_spi_flash.h>

struct device_sflash
{
    struct rt_device parent;

    uint32_t offset;
    uint32_t size;

    uint32_t page_size;
    uint8_t *sector_buffer;
};
static struct device_sflash _sflash;

static int need_copy(uint32_t addr)
{
    if (addr >= 0x3F800000 && addr <= 0x3F800000 + 4 * 1024 * 1024)
        return 1;

    return 0;
}

static rt_size_t drv_sflash_read(rt_device_t dev, rt_off_t pos, void *buffer, rt_size_t size)
{
    esp_err_t ret;
    struct device_sflash *sflash = (struct device_sflash *)dev;

    RT_ASSERT(dev != RT_NULL);

    if (((uint32_t)buffer & 0x03) || need_copy((uint32_t) buffer))
    {
        int index;
        uint8_t *ptr;

        ptr = (uint8_t *)buffer;
        for (index = 0; index < size; index ++)
        {
            ret = spi_flash_read((sflash->offset + pos + index) * sflash->page_size, sflash->sector_buffer, sflash->page_size);
            if (ret == ESP_OK)
            {
                memcpy(ptr, sflash->sector_buffer, sflash->page_size);
                ptr += sflash->page_size;
            }
            else return 0;
        }

        return size;
    }
    else
    {
        ret = spi_flash_read((sflash->offset + pos) * sflash->page_size, buffer, size * sflash->page_size);
        if (ret == ESP_OK)
        {
            return size;
        }
    }

    return 0;
}

static rt_size_t drv_sflash_write(rt_device_t dev, rt_off_t pos, const void *buffer, rt_size_t size)
{
    RT_ASSERT(dev != RT_NULL);

    /* read only */
    return size;
}

static rt_err_t drv_sflash_control(rt_device_t dev, rt_uint8_t cmd, void *args)
{
    struct device_sflash *sflash = (struct device_sflash *)dev;

    RT_ASSERT(dev != RT_NULL);

    if (cmd == RT_DEVICE_CTRL_BLK_GETGEOME)
    {
        struct rt_device_blk_geometry *geometry;

        geometry = (struct rt_device_blk_geometry *)args;
        geometry->bytes_per_sector = sflash->page_size;
        geometry->block_size       = sflash->page_size * 4;
        geometry->sector_count     = sflash->size / sflash->page_size;
    }

    return RT_EOK;
}

int rt_hw_sflash_init(void)
{
    rt_device_t device;

    device = (rt_device_t)&(_sflash.parent);

    _sflash.page_size = 512;
    _sflash.offset = 0x200000/_sflash.page_size;    /* 2M */
    _sflash.size   = 1 * 1024 * 1024;               /* 1M */
    _sflash.sector_buffer = malloc(_sflash.page_size);

    /* register sdcard device */
    device->type    = RT_Device_Class_Block;

    device->init    = RT_NULL;
    device->open    = RT_NULL;
    device->close   = RT_NULL;
    device->read    = drv_sflash_read;
    device->write   = drv_sflash_write;
    device->control = drv_sflash_control;

    /* no private, no callback */
    device->user_data   = RT_NULL;
    device->rx_indicate = RT_NULL;
    device->tx_complete = RT_NULL;

    rt_device_register(device, "flash", RT_DEVICE_FLAG_RDWR);

    return 0;
}
