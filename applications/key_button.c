#include <rtdevice.h>
#include <finsh.h>

#include <board.h>

#define GPIO_PIN    2

static int btn_inited = 0;

static void btn_thread(void *parameter)
{
    while (1)
    {
        rt_thread_delay(RT_TICK_PER_SECOND/5);
        if (rt_pin_read(GPIO_PIN) == PIN_LOW)
        {
            rt_kprintf("btn down\n");
            while (1)
            {
                rt_thread_delay(RT_TICK_PER_SECOND/20);
                if (rt_pin_read(GPIO_PIN) == PIN_HIGH)
                {
                    rt_kprintf("btn up\n");
                    break;
                }
            }
        }
    }
}

int btn(int argc, char** argv)
{
    if (!btn_inited)
    {
        rt_thread_t tid;
        btn_inited = 1;

        rt_pin_mode(GPIO_PIN, PIN_MODE_INPUT);

        tid = rt_thread_create("btn", btn_thread, RT_NULL, 1024, 20, 20);
        if (tid) rt_thread_startup(tid);
    }

    return 0;
}
MSH_CMD_EXPORT(btn, button test);
