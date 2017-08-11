#include <rtthread.h>
#include <finsh.h>

#include <esp_psram.h>
#include <rom/cache.h>

#define PSRAM_BEGIN_ADDR    0x3F800000

#ifdef RT_USING_ESP_PSRAM
#define RT_USING_SDRAM

#ifdef RT_USING_SDRAM
static struct rt_memheap sdram_heap;
#endif
#endif

/* initialize SPI pSRAM */
int rt_sdram_heap_init(void)
{
#ifdef RT_USING_SDRAM
    psram_enable(PSRAM_CACHE_F40M_S40M, PSRAM_VADDR_MODE_NORMAL);
    cache_sram_mmu_set( 0, 0, PSRAM_BEGIN_ADDR, 0, 32, 128 );

    rt_memheap_init(&sdram_heap, "SPIRAM",
        (void *)PSRAM_BEGIN_ADDR, RT_ESP_PSRAM_SIZE * 1024);
#endif

    return 0;
}

void *sdram_malloc(unsigned long size)
{
#ifdef RT_USING_SDRAM
    return rt_memheap_alloc(&sdram_heap, size);
#else
    return malloc(size);
#endif
}

void sdram_free(void *ptr)
{
#ifdef RT_USING_SDRAM
    rt_memheap_free(ptr);
#else
    return free(ptr);
#endif
}

void *sdram_calloc(unsigned int n, unsigned int size)
{
    void* ptr = NULL;

    ptr = sdram_malloc(n * size);
    if (ptr)
    {
        memset(ptr, 0, n * size);
    }

    return ptr;
}

void *sdram_realloc(void *ptr, unsigned long size)
{
#ifdef RT_USING_SDRAM
    return rt_memheap_realloc(&sdram_heap, ptr, size);
#else    
    return realloc(ptr, size);
#endif
}

