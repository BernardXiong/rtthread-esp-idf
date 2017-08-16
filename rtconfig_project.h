#ifndef RTCONFIG_PROJECT_H__
#define RTCONFIG_PROJECT_H__

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/timers.h"
#include "freertos/queue.h"
#include "freertos/StackMacros.h"
#include "freertos/xtensa_rtos.h"

#include "sdkconfig.h"
#ifdef RT_TICK_PER_SECOND
#undef RT_TICK_PER_SECOND
#endif

#define RT_TICK_PER_SECOND	CONFIG_FREERTOS_HZ

#endif

