#include <rtdevice.h>
#include <finsh.h>

#define GPIO_PIN    19

int led(int argc, char** argv)
{
    int value;
    int pin = GPIO_PIN;
	static int led_inited = 0;

    if (argc >= 2) pin = atoi(argv[1]);

    if (!led_inited)
    {
		led_inited = 1;
		rt_pin_mode(pin, PIN_MODE_OUTPUT);
    }

    if (argc >= 3)
    {
        value = atoi(argv[2]);
        rt_pin_write(pin, value);

        value = rt_pin_read(pin);
        rt_kprintf("read back value=%d\n", value);

        return 0;
    }

    value = rt_pin_read(pin);
    rt_kprintf("value=%d\n", value);
    if (value)
        rt_pin_write(pin, PIN_LOW);
    else
        rt_pin_write(pin, PIN_HIGH);

    return 0;
}
MSH_CMD_EXPORT(led, led test);

