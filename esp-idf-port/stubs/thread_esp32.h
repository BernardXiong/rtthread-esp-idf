#ifndef THREAD_ESP_H__
#define THREAD_ESP_H__

#include <rtthread.h>
#include <freertos/FreeRTOS.h>

void thread_esp_init(struct rt_thread *tid);
void thread_esp_cleanup(struct rt_thread *tid);

void thread_inited(struct rt_thread *tid);

#endif
