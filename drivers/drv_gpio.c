#include <rthw.h>
#include <rtdevice.h>

#ifdef RT_USING_PIN
#include "drv_gpio.h"

#include "driver/gpio.h"

void esp32_gpio_mode(rt_device_t dev, rt_base_t pin, rt_base_t mode)
{
	switch (mode)
	{
	case PIN_MODE_OUTPUT:
		gpio_set_direction(pin, GPIO_MODE_INPUT_OUTPUT);
		break;
	case PIN_MODE_INPUT:
		gpio_set_direction(pin, GPIO_MODE_INPUT);
		break;
	case PIN_MODE_INPUT_PULLUP:
		gpio_set_direction(pin, GPIO_MODE_INPUT_OUTPUT_OD);
		break;
	}
}

void esp32_gpio_write(rt_device_t dev, rt_base_t pin, rt_base_t value)
{
    if (value == PIN_HIGH)
    {
        gpio_set_level(pin, 1);
    }
    else
    {
        gpio_set_level(pin, 0);
    }
}

int esp32_gpio_read(rt_device_t dev, rt_base_t pin)
{
    int value;

    if (gpio_get_level(pin))
    {
        value = PIN_HIGH;
    }
    else
    {
        value = PIN_LOW;
    }

    return value;
}

rt_err_t esp32_gpio_attach_irq(struct rt_device *device, rt_int32_t pin,
    rt_uint32_t mode, void (*hdr)(void *args), void *args)
{
	switch (mode)
	{
	case PIN_IRQ_MODE_RISING:
		gpio_set_intr_type(pin, GPIO_INTR_POSEDGE);
		break;
	case PIN_IRQ_MODE_FALLING:
		gpio_set_intr_type(pin, GPIO_INTR_NEGEDGE);
		break;
	case PIN_IRQ_MODE_RISING_FALLING:
		gpio_set_intr_type(pin, GPIO_INTR_ANYEDGE);
		break;

	default:
		return -RT_EIO;
	}

	gpio_isr_handler_add(pin, hdr, args);

    return RT_EOK;
}

rt_err_t esp32_gpio_dettach_irq(struct rt_device *device, rt_int32_t pin)
{
	gpio_isr_handler_remove(pin);
    return RT_EOK;
}

rt_err_t esp32_gpio_irq_enable(struct rt_device *device, rt_base_t pin,
    rt_uint32_t enabled)
{
    if (enabled == PIN_IRQ_ENABLE)
        gpio_intr_enable(pin);
    else if (enabled == PIN_IRQ_DISABLE)
        gpio_intr_disable(pin);
    else return -RT_EIO;

    return RT_EOK;
}

const static struct rt_pin_ops _esp32_gpio_ops =
{
    esp32_gpio_mode,
    esp32_gpio_write,
    esp32_gpio_read,
    esp32_gpio_attach_irq,
    esp32_gpio_dettach_irq,
    esp32_gpio_irq_enable,
};

int esp32_hw_pin_init(void)
{
    int result;

	gpio_install_isr_service(0);
    result = rt_device_pin_register("gpio", &_esp32_gpio_ops, RT_NULL);
    return result;
}

#endif

