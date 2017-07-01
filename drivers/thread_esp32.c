#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include <rtthread.h>

#include <esp_newlib.h>
#include "thread_esp32.h"

void thread_esp_init(struct rt_thread *tid)
{
#if configUSE_TRACE_FACILITY_2 || !portUSING_MPU_WRAPPERS || XCHAL_CP_NUM <= 0
#pragma errno ("sizeof(xMPU_SETTINGS) must == 8")
#endif
    tid->xCoreID = 0; /* fix to main CPU core */
    vPortStoreTaskMPUSettings(&(tid->xMPUSettings),NULL, tid->stack_addr, tid->stack_size);
    esp_reent_init(&tid->xNewLib_reent);
    memset(tid->xtls, 0, sizeof(tid->xtls));
    memset(tid->xtls_call, 0, sizeof(tid->xtls_call));
}

void thread_esp_cleanup(struct rt_thread *tid)
{
    if (tid)
    {
        /* free tls info */
        {int x;for(x=0; x<configNUM_THREAD_LOCAL_STORAGE_POINTERS; x++){
            if (tid->xtls_call[x])
                tid->xtls_call[x](x, tid->xtls[x]);
        }}
        /* free reent */
        extern void _reclaim_reent (struct _reent * reent);
        _reclaim_reent(&(tid->xNewLib_reent));
    }
}

void thread_inited(struct rt_thread *tid)
{
    if (tid)
    {
        thread_esp_init(tid);
        tid->cleanup = thread_esp_cleanup;
    }
}

