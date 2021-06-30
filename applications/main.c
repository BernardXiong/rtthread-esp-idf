/* Hello World Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include <stdio.h>

#if 1
void app_main()
{
    nvs_flash_init();

    printf("hello!\n");
    return 0;
}
#else
#include "esp_system.h"
#include "nvs_flash.h"

#include <rtthread.h>
#include <board.h>
#include <shell.h>
#include <finsh.h>
#include <libc.h>

#include <dfs.h>
#include <dfs_fs.h>
#include <dfs_elm.h>
#include <dfs_romfs.h>
#include <dfs_init.h>
#include <devfs.h>

#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_event_loop.h"
#include "esp_log.h"

#include "drv_sdram.h"
#include "drv_i2c.h"
#include <drv_sflash.h>
#include <drv_mmc.h>
#include <drv_gpio.h>

#ifdef CONFIG_ESP_AUDIO
#include "drv_io_pa.h"
#include "audio_device.h"
#endif

#include <user_com.h>

extern const struct romfs_dirent romfs_root;
extern void lwip_system_init(void);
extern rt_err_t codec_hw_init(const char *name);

/* pre-initialization for stdio console */
int rtthread_stdio_init(void)
{
    extern int reent_std_init(void);

    dfs_init();
    devfs_init();

    /* mount devfs */
    dfs_mount(RT_NULL, "/dev", "devfs", 0, 0);

    libc_system_init();
    reent_std_init();
    return 0;
}

int rtthread_components_init(void)
{
#ifdef RT_USING_ESP_PSRAM
    /* initialize psram */
    rt_sdram_heap_init();
#endif

    esp32_hw_pin_init();
    elm_init();

    finsh_system_init();
#ifdef RT_USING_PTHREADS
    extern int pthread_system_init(void);
    pthread_system_init();
#endif

    /* initialize romroot */
    dfs_romfs_init();
    /* mount rom file system */
    if (dfs_mount(RT_NULL, "/", "rom", 0, &romfs_root) == 0)
    {
        rt_kprintf("ROM file system initializated!\n");
    }

    rt_hw_sflash_init();
    if (dfs_mount("flash", "/flash", "elm", 0, 0) == 0)
    {
        rt_kprintf("Mount flash done!\n");
    }

#ifdef RT_USING_I2C
    rt_hw_drv_i2c_init();
#endif

#ifdef CONFIG_ESP_AUDIO
    /* audio */
    io_pa_init();
    io_pa_enable(1);

    codec_hw_init("i2c0");
    audio_device_init();
#endif

    /* sdcard */
    rt_hw_sdmmc_init();
    if (dfs_mount("sd", "/sdcard", "elm", 0, 0) == 0)
    {
        rt_kprintf("Mount sdcard done!\n");
    }
    else
    {
        rt_kprintf("mount failed, errno=%d\n", rt_get_errno());
    }

    /* user commponents intialization */
    user_components_init();

    return 0;
}

void app_main()
{
    nvs_flash_init();

    rtthread_components_init();

    return ;
}
#endif
