#ifndef DRV_WLAN_H__
#define DRV_WLAN_H__

void esp32_wlanif_input(struct netif *netif, void *buffer, u16_t len, void* eb);

struct netif *esp32_wlan_get_interface(esp_interface_t interface);
int esp32_wlan_hw_init(esp_interface_t netif, uint8_t *macaddr);

#endif
