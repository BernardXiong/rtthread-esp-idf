#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include <rtthread.h>

#include <wifi_airkiss.h>

#include "esp_wifi.h"
#include "esp_log.h"
#include "esp_event_loop.h"
#include "esp_smartconfig.h"
#include "esp_system.h"
#include "nvs_flash.h"

#define TAG             "airkiss"
#define LINKED_BIT      (BIT0)
#define CONNECTED_BIT   (BIT1)

static rt_event_t smartconfig_event_group = NULL;

void smartconfig_handler(smartconfig_status_t status, void *pdata)
{
    switch (status) {
    case SC_STATUS_WAIT:
        ESP_LOGI(TAG, "SC_STATUS_WAIT");
        break;
    case SC_STATUS_FIND_CHANNEL:
        ESP_LOGI(TAG, "SC_STATUS_FIND_CHANNEL");
        break;
    case SC_STATUS_GETTING_SSID_PSWD:
        ESP_LOGI(TAG, "SC_STATUS_GETTING_SSID_PSWD");
        smartconfig_type_t *type = pdata;
        if (*type == SC_TYPE_ESPTOUCH) {
            ESP_LOGI(TAG, "SC_TYPE:SC_TYPE_ESPTOUCH");
        } else {
            ESP_LOGI(TAG, "SC_TYPE:SC_TYPE_AIRKISS");
        }
        break;
    case SC_STATUS_LINK:
        ESP_LOGI(TAG, "SC_STATUS_LINK");
        wifi_config_t wifi_config;
        wifi_config.sta = *((wifi_sta_config_t *)pdata);
        rt_kprintf("ssid:%s\npasswd:%s\n", wifi_config.sta.ssid, wifi_config.sta.password);

        esp_wifi_disconnect();
        esp_wifi_set_config(WIFI_IF_STA, &wifi_config);
        esp_wifi_connect();

        rt_event_send(smartconfig_event_group, LINKED_BIT);
        break;
    case SC_STATUS_LINK_OVER:
        ESP_LOGI(TAG, "SC_STATUS_LINK_OVER");
        if (pdata != NULL) {
            uint8_t phone_ip[4] = { 0 };

            memcpy(phone_ip, (uint8_t *) pdata, 4);
            ESP_LOGI(TAG, "Phone ip: %d.%d.%d.%d", phone_ip[0],
                   phone_ip[1], phone_ip[2], phone_ip[3]);
        }
        else
        {
            rt_kprintf("start airlink\n");
        }

        esp_smartconfig_stop();
        rt_event_send(smartconfig_event_group, CONNECTED_BIT);
        break;
    }
}

int wifi_airkiss(void)
{
    rt_uint32_t recv;

    if (!smartconfig_event_group)
    {
        smartconfig_event_group = rt_event_create("smartconfig", RT_IPC_FLAG_FIFO);
    }
    ESP_ERROR_CHECK(esp_smartconfig_set_type(SC_TYPE_ESPTOUCH_AIRKISS));
    ESP_ERROR_CHECK(esp_smartconfig_start(smartconfig_handler));

    rt_event_recv(smartconfig_event_group, CONNECTED_BIT, RT_EVENT_FLAG_OR | RT_EVENT_FLAG_CLEAR, RT_WAITING_FOREVER, &recv);
    return 0;
}
