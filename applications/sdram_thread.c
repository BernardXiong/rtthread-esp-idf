#include <rtthread.h>
#include <drv_sdram.h>

rt_thread_t sdram_thread_create(const char *name,
                             void (*entry)(void *parameter),
                             void       *parameter,
                             rt_uint32_t stack_size,
                             rt_uint8_t  priority)
{
    rt_thread_t tid;
    rt_uint8_t *stack;

    stack = (rt_uint8_t *)sdram_malloc(stack_size);
    tid = (rt_thread_t)sdram_malloc(sizeof(struct rt_thread));

    if (tid && stack)
    {
        rt_err_t result;

        result = rt_thread_init(tid, name, entry, parameter, stack, stack_size, priority, 20);
        if (result == RT_EOK)
        {
            /* change to dynamic object */
            tid->type = RT_Object_Class_Thread;
            rt_thread_startup(tid);
        }
        else goto __exit;

        return tid;
    }

__exit:
    if (tid) sdram_free(tid);
    if (stack) sdram_free(stack);

    return RT_NULL;
}
