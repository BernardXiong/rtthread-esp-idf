/* Hello World Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include <stdio.h>

#include "esp_system.h"
#include "nvs_flash.h"

#include <rtthread.h>
#include <board.h>
#include <shell.h>
#include <finsh.h>

#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_event_loop.h"
#include "esp_log.h"
#include "wifi_airkiss.h"

// #define WIFI_USING_AIRKISS
#define WIFI_SSID   "realthread_309"
#define WIFI_PASSWD "02158995663"

static esp_err_t event_handler(void *ctx, system_event_t *event)
{
    switch(event->event_id) {
    case SYSTEM_EVENT_STA_START:
        esp_wifi_connect();
        rt_kprintf("STA_START\n");
        break;
    case SYSTEM_EVENT_STA_GOT_IP:
        rt_kprintf("STA_GOT_IP\n");
        break;
    case SYSTEM_EVENT_STA_DISCONNECTED:
        /* This is a workaround as ESP32 WiFi libs don't currently
           auto-reassociate. */
        esp_wifi_connect();
        rt_kprintf("STA_DISCONNECTED\n");
        break;
    default:
        break;
    }
    return ESP_OK;
}

#ifdef WIFI_USING_AIRKISS
void rt_hw_wifi_init()
{
    tcpip_adapter_init();
    ESP_ERROR_CHECK( esp_event_loop_init(event_handler, NULL) );
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK( esp_wifi_init(&cfg) );
    ESP_ERROR_CHECK( esp_wifi_set_storage(WIFI_STORAGE_FLASH) );
    ESP_ERROR_CHECK( esp_wifi_set_mode(WIFI_MODE_STA) );
    ESP_ERROR_CHECK( esp_wifi_start() );

    wifi_config_t wifi_config;
    memset(&wifi_config, 0x0, sizeof(wifi_config));

    esp_wifi_get_config(WIFI_IF_STA, &wifi_config);
    if (wifi_config.sta.ssid[0] == 0)
    {
        printf("startup airkiss...\n");
        wifi_airkiss();
    }
    else
    {
        esp_wifi_set_config(WIFI_IF_STA, &wifi_config);
        esp_wifi_connect();
    }
}
#else
void rt_hw_wifi_init()
{
    tcpip_adapter_init();
    ESP_ERROR_CHECK( esp_event_loop_init(event_handler, NULL) );
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK( esp_wifi_init(&cfg) );
    ESP_ERROR_CHECK( esp_wifi_set_storage(WIFI_STORAGE_RAM) );
    ESP_ERROR_CHECK( esp_wifi_set_mode(WIFI_MODE_STA) );
    wifi_config_t sta_config = {
        .sta = {
            .ssid      = WIFI_SSID,
            .password  = WIFI_PASSWD,
            .bssid_set = false
        }
    };
    ESP_ERROR_CHECK( esp_wifi_set_config(WIFI_IF_STA, &sta_config) );
    ESP_ERROR_CHECK( esp_wifi_start() );
    ESP_ERROR_CHECK( esp_wifi_connect() );
}
#endif

#include <dfs.h>
#include <dfs_fs.h>
#include <dfs_elm.h>
#include <dfs_init.h>
#include <devfs.h>
#include <drv_sflash.h>
#include <drv_mmc.h>

extern void lwip_system_init(void);
extern int  eth_system_device_init(void);

#include <libc.h>
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
    elm_init();
    lwip_system_init();
    eth_system_device_init();

    finsh_system_init();
#ifdef RT_USING_PTHREADS
    extern int pthread_system_init(void);
    pthread_system_init();
#endif

#if 0
    rt_hw_sflash_init();
    if (dfs_mount("flash", "/", "elm", 0, 0) == 0)
    {
        rt_kprintf("Mount filesystem done!\n");
    }
#endif

    /* mount sd to "/" */
    rt_hw_sdmmc_init();
    if (dfs_mount("sd", "/", "elm", 0, 0) == 0)
    {
        rt_kprintf("Mount filesystem done!\n");
    }
    else
    {
        rt_kprintf("mount failed, errno=%d\n", rt_get_errno());
    }

    return 0;
}

int libc_test(int argc, char** argv)
{
    printf("hello!!\n");
    printf("libc, 100=%d, 100.5=%f\n", 100, 100.5);
    printf("str: %s\n", "string");

    return 0;
}
MSH_CMD_EXPORT(libc_test, libc test!!!);

void app_main()
{
    nvs_flash_init();

    rtthread_components_init();
    rt_hw_wifi_init();

    return ;
}
