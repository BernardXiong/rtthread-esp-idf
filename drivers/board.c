#include <rtthread.h>
#include <rthw.h>
#include <rtm.h>
#include <rtdevice.h>
#include <board.h>

#include "esp_types.h"
#include "esp_attr.h"
#include "esp_intr.h"
#include "esp_log.h"

#include <thread_esp32.h>
#include "drv_uart.h"

void rt_hw_idle_hook(void)
{
    extern void esp_vApplicationIdleHook( void );

#if ( configUSE_IDLE_HOOK == 1 )
    extern void vApplicationIdleHook( void );
    vApplicationIdleHook();
#endif /* configUSE_IDLE_HOOK */

    esp_vApplicationIdleHook();
}

void rt_hw_board_init(void)
{
    /* initialize uart */
    rt_hw_uart_init();

#ifdef RT_USING_CONSOLE
    rt_console_set_device(CONSOLE_DEVICE);
#endif

    rt_thread_idle_sethook(rt_hw_idle_hook);
    rt_thread_inited_sethook(thread_inited);
}

