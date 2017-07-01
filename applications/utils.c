#include <stdint.h>
#include <stdio.h>

#include <rtthread.h>

#define __is_print(ch) ((unsigned int)((ch) - ' ') < 127u - ' ')

void dump_hex(const uint8_t *ptr, size_t buflen)
{
    unsigned char *buf = (unsigned char*)ptr;
    int i, j;
    for (i=0; i<buflen; i+=16) 
    {
        rt_kprintf("%06x: ", i);
        for (j=0; j<16; j++)
            if (i+j < buflen)
                rt_kprintf("%02x ", buf[i+j]);
            else
                rt_kprintf("   ");
        rt_kprintf(" ");
        for (j=0; j<16; j++)
            if (i+j < buflen)
                rt_kprintf("%c", __is_print(buf[i+j]) ? buf[i+j] : '.');
        rt_kprintf("\n");
    }
}
