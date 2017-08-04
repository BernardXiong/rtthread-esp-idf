#ifndef DRV_SDRAM_H__
#define DRV_SDRAM_H__

void *sdram_malloc(unsigned long size);
void sdram_free(void *ptr);
void *sdram_calloc(unsigned int n, unsigned int size);
void *sdram_realloc(void *ptr, unsigned long size);

int rt_sdram_heap_init(void);

#endif
