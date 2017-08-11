#include <rtthread.h>

int list_mem(void)
{
    rt_kprintf("free memory : %d\n", xPortGetFreeHeapSize());
    return 0;
}

