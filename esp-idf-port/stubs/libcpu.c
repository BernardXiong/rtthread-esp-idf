#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "rom/ets_sys.h"
#include "esp_newlib.h"
#include <rtthread.h>
#include "event_groups.h"
#include "thread_esp32.h"

//#define SHOW_TSK_DEBUG_INFO
//#define SHOW_QUE_DEBUG_INFO
//#define SHOW_TIM_DEBUG_INFO
//#define SHOW_EVT_DEBUG_INFO

portSTACK_TYPE * volatile pxCurrentTCB[ portNUM_PROCESSORS ] = { NULL };
portSTACK_TYPE * volatile pxSaveTCB[ portNUM_PROCESSORS ] = { NULL };

#define RTT_USING_CPUID 0
extern rt_thread_t rt_current_thread;
extern void rt_hw_board_init();

static volatile unsigned short mq_index = 0;
static volatile unsigned short ms_index = 0;
static volatile unsigned short mx_index = 0;
static volatile unsigned short mr_index = 0;
static volatile unsigned short tm_index = 0;
static volatile unsigned short ev_index = 0;
static volatile BaseType_t xSchedulerRunning = pdFALSE;

void rt_hw_context_switch_to(rt_uint32_t to)
{
    pxSaveTCB[RTT_USING_CPUID] = (portSTACK_TYPE *)to;
}

void rt_hw_context_switch(rt_uint32_t from, rt_uint32_t to)
{
    pxSaveTCB[RTT_USING_CPUID] = (portSTACK_TYPE *)to;
    portYIELD();
}

void rt_hw_context_switch_interrupt(rt_uint32_t from, rt_uint32_t to)
{
    pxSaveTCB[RTT_USING_CPUID] = (portSTACK_TYPE *)to;
    portYIELD_FROM_ISR();
}

rt_base_t rt_hw_interrupt_disable(void)
{
    return portENTER_CRITICAL_NESTED();
}

void rt_hw_interrupt_enable(rt_base_t level)
{
    portEXIT_CRITICAL_NESTED(level);
}

static void thread_exit_entry(void *parameter)
{
    extern void rt_thread_exit(void);

    rt_thread_t tid;
    void (*entry)(void* parameter);

    tid = rt_thread_self();

    entry = tid->entry;
    entry(parameter);

    /* invoke thread_exit */
    rt_thread_exit();
}

rt_uint8_t *rt_hw_stack_init(void *tentry, void *parameter, rt_uint8_t *stack_addr, void *texit)
{
    rt_uint8_t *ret_sp;

    ret_sp = (rt_uint8_t *)pxPortInitialiseStack(stack_addr, thread_exit_entry, parameter, pdFALSE);

    return ret_sp;
}

void rtthread_startup(void)
{
    /* disbable interrupt */
    rt_hw_interrupt_disable();

    /* init board */
    rt_hw_board_init();

    /* show version */
    rt_kprintf("\n\n");
    rt_show_version();

    /* init tick */
    rt_system_tick_init();

    /* init kernel object */
    rt_system_object_init();

    /* init timer system */
    rt_system_timer_init();

    /* init scheduler system */
    rt_system_scheduler_init();

    /* init timer thread */
    rt_system_timer_thread_init();

    /* init idle thread */
    rt_thread_idle_init();
}

void rt_hw_console_output(const char *str)
{
    ets_printf(str);
}

struct _reent* __getreent()
{
    if( xSchedulerRunning != pdFALSE )
        return &rt_current_thread->xNewLib_reent;
    return _GLOBAL_REENT;
}

void *rt_malloc(rt_size_t nbytes) { return pvPortMalloc(nbytes); }
void rt_free(void *ptr) { vPortFree(ptr); }

BaseType_t xTaskCreatePinnedToCore(TaskFunction_t pxTaskCode, const char * const pcName, const uint32_t usStackDepth, void * const pvParameters, UBaseType_t uxPriority, TaskHandle_t * const pxCreatedTask, const BaseType_t xCoreID )
{
    BaseType_t xRunPrivileged;
    uint32_t usStackDepthC;
    if((uxPriority & portPRIVILEGE_BIT) != 0U)
        xRunPrivileged = pdTRUE;
    else
        xRunPrivileged = pdFALSE;
    uxPriority &= ~portPRIVILEGE_BIT;
    (void)(xRunPrivileged);

#if (configMAX_PRIORITIES) > (25)
    #pragma errno ("configMAX_PRIORITIES must <= 25")
#endif
    uxPriority = RT_THREAD_PRIORITY_MAX-5-uxPriority;
    usStackDepthC = usStackDepth*sizeof(StackType_t);

    BaseType_t xReturn = errCOULD_NOT_ALLOCATE_REQUIRED_MEMORY;
    rt_thread_t thread = rt_thread_create((const char *)pcName,pxTaskCode,pvParameters,usStackDepthC,uxPriority,10);
    if (thread != NULL)
    {
        rt_thread_startup(thread);
        xReturn = pdPASS;
    }

    if (pxCreatedTask) *pxCreatedTask = thread;
#ifdef SHOW_TSK_DEBUG_INFO
    ets_printf("TaskCreate cur:%s name:%s pri:%d size:%d\n",
                (rt_current_thread)?(rt_current_thread->name):("NULL"),
                pcName,uxPriority,usStackDepthC);
#endif
    return xReturn;
}

void vTaskDelete(xTaskHandle xTaskToDelete)
{
    rt_thread_t thread = xTaskToDelete;

    if (thread == NULL)
        thread = rt_current_thread;
#ifdef SHOW_TSK_DEBUG_INFO
    ets_printf("TaskDelete cur:%s name:%s\n",
                (rt_current_thread)?(rt_current_thread->name):("NULL"),
                thread->name);
#endif
    rt_thread_delete(thread);
    rt_schedule();
}

void vTaskSwitchContext( void )
{
    if (pxSaveTCB[RTT_USING_CPUID] != NULL)
    {
        pxCurrentTCB[RTT_USING_CPUID] = pxSaveTCB[RTT_USING_CPUID];
        pxSaveTCB[RTT_USING_CPUID] = NULL;
        return;
    }
    rt_schedule();
}

TaskHandle_t xTaskGetCurrentTaskHandleForCPU( BaseType_t cpuid )
{
    if (cpuid == RTT_USING_CPUID)
        return rt_current_thread;
    ets_printf("GetCurrentTaskHandleForCPU cpuid == %d\n",cpuid);
    return NULL;
}

BaseType_t xTaskGetSchedulerState( void )
{
    BaseType_t xReturn = taskSCHEDULER_NOT_STARTED;
    if( xSchedulerRunning != pdFALSE )
    {
        if (rt_critical_level() == 0)
            xReturn = taskSCHEDULER_RUNNING;
        else
            xReturn = taskSCHEDULER_SUSPENDED;
    }
    return xReturn;
}

void vTaskEnterCritical( portMUX_TYPE *mux )
{
    vPortCPUAcquireMutex( mux );
    if( xSchedulerRunning != pdFALSE )
        rt_enter_critical();
}

void vTaskExitCritical( portMUX_TYPE *mux )
{
    vPortCPUReleaseMutex( mux );
    if( xSchedulerRunning != pdFALSE )
        rt_exit_critical();
}

void vTaskSuspendAll( void )
{
    if( xSchedulerRunning != pdFALSE )
        rt_enter_critical();
}

BaseType_t xTaskResumeAll( void )
{
    if( xSchedulerRunning != pdFALSE )
        rt_exit_critical();
    return pdTRUE;
}

BaseType_t xTaskIncrementTick( void )
{
#if ( configUSE_TICK_HOOK == 1 )
    extern void vApplicationTickHook( void );
    vApplicationTickHook();
#endif /* configUSE_TICK_HOOK */
    extern void esp_vApplicationTickHook( void );
    esp_vApplicationTickHook();

    rt_interrupt_enter();
    rt_tick_increase();
    rt_interrupt_leave();
    return pdFALSE;
}

void *pvTaskGetThreadLocalStoragePointer( TaskHandle_t xTaskToQuery, BaseType_t xIndex )
{
    rt_thread_t thread = xTaskToQuery;

    if( xIndex < configNUM_THREAD_LOCAL_STORAGE_POINTERS )
        return thread->xtls[xIndex];
    return NULL;
}

void vTaskSetThreadLocalStoragePointerAndDelCallback( TaskHandle_t xTaskToSet, BaseType_t xIndex, void *pvValue , TlsDeleteCallbackFunction_t xDelCallback)
{
    rt_thread_t thread = xTaskToSet;
    if( xIndex < configNUM_THREAD_LOCAL_STORAGE_POINTERS )
    {
        thread->xtls[xIndex] = pvValue;
        thread->xtls_call[xIndex] = xDelCallback;
    }
}

TickType_t xTaskGetTickCount( void )
{
    return rt_tick_get();
}

void vTaskStartScheduler(void)
{
    rt_system_scheduler_start();
    xSchedulerRunning=pdTRUE;
    xPortStartScheduler();
}

void vTaskDelay( const TickType_t xTicksToDelay ) { rt_thread_delay(xTicksToDelay); }
TaskHandle_t xTaskGetCurrentTaskHandle( void ) { return rt_current_thread; }
BaseType_t xTaskGetAffinity( TaskHandle_t xTask ) { return RTT_USING_CPUID; }
char *pcTaskGetTaskName( TaskHandle_t xTaskToQuery ) { rt_thread_t thread=xTaskToQuery;return thread->name; }

extern rt_mailbox_t rt_fmq_create(const char *name, rt_size_t item, rt_size_t size, rt_uint8_t flag);
extern rt_err_t rt_fmq_delete(rt_mailbox_t mb);
extern rt_err_t rt_fmq_send(rt_mailbox_t mb, void *value, rt_int32_t pos, rt_int32_t timeout);
extern rt_err_t rt_fmq_recv(rt_mailbox_t mb, void *value, rt_int32_t peek, rt_int32_t timeout);

QueueHandle_t xQueueGenericCreate( const UBaseType_t uxQueueLength, const UBaseType_t uxItemSize, const uint8_t ucQueueType )
{
    char name[10] = {0};
    rt_object_t obj = 0;
    if (uxItemSize <= 0 || uxQueueLength <= 0)
    {
        if (ucQueueType == queueQUEUE_TYPE_MUTEX || ucQueueType == queueQUEUE_TYPE_RECURSIVE_MUTEX)
        {
            if (ucQueueType == queueQUEUE_TYPE_MUTEX)
            {
                sprintf(name,"mux%02d",((++mx_index)%100));
                obj = (rt_object_t)rt_sem_create(name,1,RT_IPC_FLAG_PRIO);
            }
            else
            {
                sprintf(name,"rmx%02d",((++mr_index)%100));
                obj = (rt_object_t)rt_mutex_create(name,RT_IPC_FLAG_PRIO);
            }
        }
        else
        {
            sprintf(name,"sem%02d",((++ms_index)%100));
            obj = (rt_object_t)rt_sem_create(name,0,RT_IPC_FLAG_PRIO);
        }
    }
    else
    {
        sprintf(name,"fmq%02d",((++mq_index)%100));
        obj = (rt_object_t)rt_fmq_create(name,uxItemSize,uxQueueLength,RT_IPC_FLAG_PRIO);
    }
#ifdef SHOW_QUE_DEBUG_INFO
    ets_printf("QueueCreate cur:%s name:%s count:%d size:%d type:%d\n",
                (rt_current_thread)?(rt_current_thread->name):("NULL"),
                name,uxQueueLength,uxItemSize,ucQueueType);
#endif
    return obj;
}

void vQueueDelete( QueueHandle_t xQueue )
{
    rt_object_t obj = xQueue;
#ifdef SHOW_QUE_DEBUG_INFO
    ets_printf("QueueDelete cur:%s name:%s\n",
                (rt_current_thread)?(rt_current_thread->name):("NULL"),
                obj->name);
#endif
    if (obj->type == RT_Object_Class_Semaphore)
        rt_sem_delete((rt_sem_t)obj);
    else if (obj->type == RT_Object_Class_Mutex)
        rt_mutex_delete((rt_mutex_t)obj);
    else
        rt_fmq_delete((rt_mailbox_t)obj);
}

BaseType_t xQueueGenericSend( QueueHandle_t xQueue, const void * const pvItemToQueue, TickType_t xTicksToWait, const BaseType_t xCopyPosition )
{
    rt_object_t obj = xQueue;
#ifdef SHOW_QUE_DEBUG_INFO
    ets_printf("QueueSend     cur:%s name:%s wait:%d pos:%d\n",
                (rt_current_thread)?(rt_current_thread->name):("NULL"),
                obj->name,xTicksToWait,xCopyPosition);
#endif
    rt_err_t err = RT_EOK;
    if (obj->type == RT_Object_Class_Semaphore)
        err = rt_sem_release((rt_sem_t)obj);
    else if (obj->type == RT_Object_Class_Mutex)
        err = rt_mutex_release((rt_mutex_t)obj);
    else
        err = rt_fmq_send((rt_mailbox_t)obj,(void *)pvItemToQueue,xCopyPosition,xTicksToWait);
#ifdef SHOW_QUE_DEBUG_INFO
    ets_printf("QueueSendOver cur:%s name:%s ret:%d\n",
                (rt_current_thread)?(rt_current_thread->name):("NULL"),
                obj->name,err);
#endif
    return (err==RT_EOK)?pdPASS:errQUEUE_FULL;
}

BaseType_t xQueueGenericSendFromISR( QueueHandle_t xQueue,
    const void * const pvItemToQueue,
    BaseType_t * const pxHigherPriorityTaskWoken,
    const BaseType_t xCopyPosition )
{
    rt_object_t obj = xQueue;
    rt_interrupt_enter();
#ifdef SHOW_QUE_DEBUG_INFO
    ets_printf("QueueSendISR     cur:%s name:%s pos:%d\n",
                (rt_current_thread)?(rt_current_thread->name):("NULL"),
                obj->name,xCopyPosition);
#endif
    rt_err_t err = RT_EOK;
    if (obj->type == RT_Object_Class_Semaphore)
        err = rt_sem_release((rt_sem_t)obj);
    else
        err = rt_fmq_send((rt_mailbox_t)obj,(void *)pvItemToQueue, xCopyPosition, 0);
#ifdef SHOW_QUE_DEBUG_INFO
    ets_printf("QueueSendISROver cur:%s name:%s ret:%d\n",
                (rt_current_thread)?(rt_current_thread->name):("NULL"),
                obj->name,err);
#endif
    if (pxHigherPriorityTaskWoken) *pxHigherPriorityTaskWoken = pdFALSE;
    rt_interrupt_leave();
    return (err==RT_EOK)?pdPASS:errQUEUE_FULL;
}

BaseType_t xQueueGenericReceive( QueueHandle_t xQueue, void * const pvBuffer, TickType_t xTicksToWait, const BaseType_t xJustPeeking )
{
    rt_object_t obj = xQueue;
#ifdef SHOW_QUE_DEBUG_INFO
    ets_printf("QueueRecv     cur:%s name:%s wait:%d peek:%d\n",
                (rt_current_thread)?(rt_current_thread->name):("NULL"),
                obj->name,xTicksToWait,xJustPeeking);
#endif
    rt_err_t err = RT_EOK;
    if (obj->type == RT_Object_Class_Semaphore)
        err = rt_sem_take((rt_sem_t)obj,xTicksToWait);
    else if (obj->type == RT_Object_Class_Mutex)
        err = rt_mutex_take((rt_mutex_t)obj,xTicksToWait);
    else
        err = rt_fmq_recv((rt_mailbox_t)obj,(void *)pvBuffer,xJustPeeking,xTicksToWait);
#ifdef SHOW_QUE_DEBUG_INFO
    ets_printf("QueueRecvOver cur:%s name:%s ret:%d\n",
                (rt_current_thread)?(rt_current_thread->name):("NULL"),
                obj->name,err);
#endif
    return (err==RT_EOK)?pdPASS:errQUEUE_EMPTY;
}

BaseType_t xQueueReceiveFromISR( QueueHandle_t xQueue, void * const pvBuffer, BaseType_t * const pxHigherPriorityTaskWoken )
{
    rt_object_t obj = xQueue;
    rt_interrupt_enter();
#ifdef SHOW_QUE_DEBUG_INFO
    ets_printf("QueueRecvISR     cur:%s name:%s\n",
                (rt_current_thread)?(rt_current_thread->name):("NULL"),
                obj->name);
#endif
    rt_err_t err = RT_EOK;
    if (obj->type == RT_Object_Class_Semaphore)
        err = rt_sem_take((rt_sem_t)obj,0);
    else
        err = rt_fmq_recv((rt_mailbox_t)obj,(void *)pvBuffer, pdFALSE, 0);
#ifdef SHOW_QUE_DEBUG_INFO
    ets_printf("QueueRecvISROver cur:%s name:%s ret:%d\n",
                (rt_current_thread)?(rt_current_thread->name):("NULL"),
                obj->name,err);
#endif

    if (pxHigherPriorityTaskWoken) *pxHigherPriorityTaskWoken = pdFALSE;
    rt_interrupt_leave();
    return (err==RT_EOK)?pdPASS:errQUEUE_EMPTY;
}

UBaseType_t uxQueueMessagesWaiting( const QueueHandle_t xQueue )
{
    rt_object_t obj = xQueue;
    unsigned portBASE_TYPE count = 0;
    if (obj->type == RT_Object_Class_Semaphore)
        count = ((rt_sem_t)obj)->value;
    else if (obj->type == RT_Object_Class_Mutex)
        count = ((rt_mutex_t)obj)->value;
    else
        count = ((rt_mailbox_t)obj)->entry;
    return count;
}

UBaseType_t uxQueueMessagesWaitingFromISR( const QueueHandle_t xQueue ) { return uxQueueMessagesWaiting(xQueue); }
QueueHandle_t xQueueCreateMutex( const uint8_t ucQueueType ) { return xQueueGenericCreate(1,0,ucQueueType); }
BaseType_t xQueueTakeMutexRecursive( QueueHandle_t xMutex, TickType_t xTicksToWait ) { return xQueueGenericReceive(xMutex,0,xTicksToWait,pdFALSE); }
BaseType_t xQueueGiveMutexRecursive( QueueHandle_t xMutex ) { return xQueueGenericSend(xMutex,0,0,queueSEND_TO_BACK); }

BaseType_t xQueueGiveFromISR( QueueHandle_t xQueue, BaseType_t * const pxHigherPriorityTaskWoken )
{
    return xQueueGenericSendFromISR(xQueue,0,pxHigherPriorityTaskWoken,queueSEND_TO_BACK);
}

void* xQueueGetMutexHolder( QueueHandle_t xSemaphore )
{
    return NULL;
}

BaseType_t xQueueGenericReset( QueueHandle_t xQueue, BaseType_t xNewQueue )
{
    return pdPASS;
}

BaseType_t xQueueIsQueueFullFromISR( const QueueHandle_t xQueue )
{
    BaseType_t xReturn = pdFALSE;
    rt_object_t obj = xQueue;

    if (obj->type == RT_Object_Class_MailBox)
    {
        rt_ubase_t level;
        struct rt_mailbox *mb = (struct rt_mailbox *)obj;

        level = rt_hw_interrupt_disable();
        if (mb->entry == mb->size) xReturn = pdTRUE;
        rt_hw_interrupt_enable(level);
    }

    return xReturn;
}

UBaseType_t uxQueueSpacesAvailable( const QueueHandle_t xQueue )
{
    UBaseType_t uxReturn = 0;
    rt_object_t obj = xQueue;

    if (obj->type == RT_Object_Class_MailBox)
    {
        rt_ubase_t level;
        struct rt_mailbox *mb = (struct rt_mailbox *)obj;

        level = rt_hw_interrupt_disable();
        uxReturn = mb->size - mb->entry;
        rt_hw_interrupt_enable(level);
    }

    return uxReturn;
}

TimerHandle_t xTimerCreate( const char * const pcTimerName, const TickType_t xTimerPeriodInTicks, const UBaseType_t uxAutoReload, void * const pvTimerID, TimerCallbackFunction_t pxCallbackFunction )
{
    char name[10] = {0}, *tname = (char *)pcTimerName;
    rt_timer_t obj = 0;
    if (pcTimerName == NULL)
    {
        sprintf(name,"tim%02d",((++tm_index)%100));
        tname = name;
    }
    if (xTimerPeriodInTicks > 0)
    {
        rt_uint8_t flag = (uxAutoReload == pdTRUE)?(RT_TIMER_FLAG_PERIODIC):(RT_TIMER_FLAG_ONE_SHOT);
        obj = rt_timer_create(tname,pxCallbackFunction,pvTimerID,xTimerPeriodInTicks,flag);
    }
#ifdef SHOW_TIM_DEBUG_INFO
    ets_printf("xTimerCreate cur:%s name:%s tick:%d auto:%d id:%x func:%x\n",
                (rt_current_thread)?(rt_current_thread->name):("NULL"),
                tname,xTimerPeriodInTicks,uxAutoReload,pvTimerID,pxCallbackFunction);
#endif
    return obj;
}

BaseType_t xTimerGenericCommand( TimerHandle_t xTimer, const BaseType_t xCommandID, const TickType_t xOptionalValue, BaseType_t * const pxHigherPriorityTaskWoken, const TickType_t xTicksToWait )
{
    rt_timer_t obj = xTimer;
    if (pxHigherPriorityTaskWoken)
        rt_interrupt_enter();
#ifdef SHOW_TIM_DEBUG_INFO
    ets_printf("xTimerCommand cur:%s name:%s cmd:%d val:%d tim:%d\n",
                (rt_current_thread)?(rt_current_thread->name):("NULL"),
                obj->parent.name,xCommandID,xOptionalValue,xBlockTime);
#endif
    rt_err_t err = RT_EOK;
    switch(xCommandID)
    {
    case tmrCOMMAND_START:
        err = rt_timer_start(obj);
        break;
    case tmrCOMMAND_STOP:
        err = rt_timer_stop(obj);
        break;
    case tmrCOMMAND_CHANGE_PERIOD:{
        rt_tick_t tick = xOptionalValue;
        err = rt_timer_control(obj,RT_TIMER_CTRL_SET_TIME,&tick);
        break;}
    case tmrCOMMAND_DELETE:
        err = rt_timer_delete(obj);
        break;
    }
#ifdef SHOW_TIM_DEBUG_INFO
    ets_printf("xTimerCommandOver cur:%s name:%s ret:%d\n",
                (rt_current_thread)?(rt_current_thread->name):("NULL"),
                obj->parent.name,err);
#endif
    if (pxHigherPriorityTaskWoken) {
        *pxHigherPriorityTaskWoken = pdFAIL;
        rt_interrupt_leave();
    }
    return (err==RT_EOK)?pdPASS:pdFAIL;
}

EventGroupHandle_t xEventGroupCreate( void )
{
    char name[10] = {0};
    sprintf(name,"evt%02d",((++ev_index)%100));
    rt_event_t obj = rt_event_create(name,RT_IPC_FLAG_PRIO);
#ifdef SHOW_EVT_DEBUG_INFO
    ets_printf("xEventGroupCreate cur:%s name:%s\n",
                (rt_current_thread)?(rt_current_thread->name):("NULL"),
                name);
#endif
    return obj;
}
void vEventGroupDelete( EventGroupHandle_t xEventGroup )
{
    rt_event_t obj = xEventGroup;
#ifdef SHOW_EVT_DEBUG_INFO
    ets_printf("vEventGroupDelete cur:%s name:%s\n",
                (rt_current_thread)?(rt_current_thread->name):("NULL"),
                obj->name);
#endif
    rt_event_delete(obj);
}
EventBits_t xEventGroupSetBits( EventGroupHandle_t xEventGroup, const EventBits_t uxBitsToSet )
{
    rt_event_t obj = xEventGroup;
    rt_err_t err = rt_event_send(obj,uxBitsToSet);
#ifdef SHOW_EVT_DEBUG_INFO
    ets_printf("xEventGroupSetBits cur:%s name:%s bits:%x ret:%d\n",
                (rt_current_thread)?(rt_current_thread->name):("NULL"),
                obj->parent.name,uxBitsToSet,err);
#endif
    return (err!=RT_EOK)?0:obj->set;
}
EventBits_t xEventGroupClearBits( EventGroupHandle_t xEventGroup, const EventBits_t uxBitsToClear )
{
    rt_event_t obj = xEventGroup;
    rt_uint32_t recved = 0;
    rt_err_t err = rt_event_recv(obj,uxBitsToClear,RT_EVENT_FLAG_OR|RT_EVENT_FLAG_CLEAR,0,&recved);
#ifdef SHOW_EVT_DEBUG_INFO
    ets_printf("xEventGroupClearBits cur:%s name:%s bits:%x ret:%d\n",
                (rt_current_thread)?(rt_current_thread->name):("NULL"),
                obj->parent.name,uxBitsToClear,err);
#endif
    return (err!=RT_EOK)?0:recved;
}
EventBits_t xEventGroupWaitBits( EventGroupHandle_t xEventGroup, const EventBits_t uxBitsToWaitFor, const BaseType_t xClearOnExit, const BaseType_t xWaitForAllBits, TickType_t xTicksToWait )
{
    rt_event_t obj = xEventGroup;
    rt_uint32_t recved = 0;
    rt_uint8_t option = 0;
    if (xClearOnExit == pdTRUE)
        option |= RT_EVENT_FLAG_CLEAR;
    if (xWaitForAllBits == pdTRUE)
        option |= RT_EVENT_FLAG_AND;
    else
        option |= RT_EVENT_FLAG_OR;
    rt_err_t err = rt_event_recv(obj,uxBitsToWaitFor,option,xTicksToWait,&recved);
#ifdef SHOW_EVT_DEBUG_INFO
    ets_printf("xEventGroupWaitBits cur:%s name:%s bits:%x ret:%d\n",
                (rt_current_thread)?(rt_current_thread->name):("NULL"),
                obj->parent.name,uxBitsToWaitFor,err);
#endif
    return (err!=RT_EOK)?0:recved;
}

