#include <rtthread.h>
#include <rtdevice.h>

#include "drv_i2c.h"
#include <driver/gpio.h>

#define I2C_SDA_IO      18
#define I2C_SCL_IO      23

struct esp_i2c_bit_data
{
    gpio_num_t sda;
    gpio_num_t scl;
};

static void gpio_set_sda(void *data, rt_int32_t state)
{
    struct esp_i2c_bit_data *bd = data;

    if (state)
    {
        gpio_set_level(bd->sda, 1);
    }
    else
    {
        gpio_set_level(bd->sda, 0);
    }
}

static void gpio_set_scl(void *data, rt_int32_t state)
{
    struct esp_i2c_bit_data *bd = data;

    if (state)
    {
        gpio_set_level(bd->scl, 1);
    }
    else
    {
        gpio_set_level(bd->scl, 0);
    }
}

static rt_int32_t gpio_get_sda(void *data)
{
    struct esp_i2c_bit_data *bd = data;

    return gpio_get_level(bd->sda);
}

static rt_int32_t gpio_get_scl(void *data)
{
    struct esp_i2c_bit_data *bd = data;

    return gpio_get_level(bd->scl);
}

static void gpio_udelay(rt_uint32_t us)
{
    volatile rt_int32_t i;
    for (; us > 0; us--)
    {
        i = 30;
        while (i--);
    }
}

int rt_hw_drv_i2c_init(void)
{
    /* register I2C0: SCL SDA */
    static struct rt_i2c_bus_device i2c_device;
    static struct esp_i2c_bit_data _i2c_bdata = {.sda = I2C_SDA_IO, .scl = I2C_SCL_IO};

    static const struct rt_i2c_bit_ops _i2c_bit_ops =
    {
        (void*)&_i2c_bdata,
        gpio_set_sda,
        gpio_set_scl,
        gpio_get_sda,
        gpio_get_scl,

        gpio_udelay,

        5,
        100
    };

    gpio_config_t io_conf;

    io_conf.intr_type = GPIO_PIN_INTR_DISABLE;
    io_conf.mode = GPIO_MODE_INPUT_OUTPUT_OD;
    io_conf.pin_bit_mask = (1<<I2C_SDA_IO) | (1 << I2C_SCL_IO);
    io_conf.pull_down_en = 0;
    io_conf.pull_up_en = 1;
    gpio_config(&io_conf);

    i2c_device.priv = (void *)&_i2c_bit_ops;
    rt_i2c_bit_add_bus(&i2c_device, "i2c0");

    return RT_EOK;
}
