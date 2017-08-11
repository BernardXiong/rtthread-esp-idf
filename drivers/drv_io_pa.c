#include <rtthread.h>
#include <board.h>

#include "driver/gpio.h"

#ifdef PA_GPIO
int io_pa_init(void)
{
    gpio_config_t io_conf;

    io_conf.intr_type = GPIO_PIN_INTR_DISABLE;
    io_conf.mode = GPIO_MODE_OUTPUT;
    io_conf.pin_bit_mask = (1 << PA_GPIO);
    io_conf.pull_down_en = 0;
    io_conf.pull_up_en = 0;
    gpio_config(&io_conf);

    return 0;
}

int io_pa_enable(int enable)
{
    gpio_set_level(PA_GPIO, enable);
    return 0;
}
#endif

