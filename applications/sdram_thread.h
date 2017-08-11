#ifndef SDRAM_THREAD_H__
#define SDRAM_THREAD_H__

rt_thread_t sdram_thread_create(const char *name,
                             void (*entry)(void *parameter),
                             void       *parameter,
                             rt_uint32_t stack_size,
                             rt_uint8_t  priority);

#endif
