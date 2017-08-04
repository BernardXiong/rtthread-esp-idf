#include <stdio.h>
#include <string.h>

#include <tcpip_adapter.h>
#include <drv_wlan.h>
#include <lwip/dhcp.h>
#include <lwip/ip_addr.h>

void tcpip_adapter_init(void)
{
}

static esp_interface_t to_esp_interface(tcpip_adapter_if_t tcpip_if)
{
    esp_interface_t interface = ESP_IF_MAX;

    switch (tcpip_if)
    {
    case TCPIP_ADAPTER_IF_STA:
        interface = ESP_IF_WIFI_STA;
        break;
    case TCPIP_ADAPTER_IF_AP:
        interface = ESP_IF_WIFI_AP;
        break;
    case TCPIP_ADAPTER_IF_ETH:
        interface = ESP_IF_ETH;
        break;
    default:
        break;
    }

    return interface;
}

esp_err_t tcpip_adapter_start(tcpip_adapter_if_t tcpip_if, uint8_t *mac, tcpip_adapter_ip_info_t *ip_info)
{
    esp32_wlan_hw_init(to_esp_interface(tcpip_if), mac);
    return ESP_OK;
}

esp_err_t tcpip_adapter_stop(tcpip_adapter_if_t tcpip_if)
{
    return ESP_OK;
}

esp_err_t tcpip_adapter_get_ip_info(tcpip_adapter_if_t tcpip_if, tcpip_adapter_ip_info_t *ip_info)
{
    struct netif *netif = esp32_wlan_get_interface(tcpip_if);
    if (netif && ip_info)
    {
    }

    return ESP_OK;
}

esp_err_t tcpip_adapter_up(tcpip_adapter_if_t tcpip_if)
{
    struct netif *netif = esp32_wlan_get_interface(tcpip_if);
    if (netif)
    {
        netif_set_up(netif);
    }

    return ESP_OK;
}

esp_err_t tcpip_adapter_down(tcpip_adapter_if_t tcpip_if)
{
    struct netif *netif = esp32_wlan_get_interface(tcpip_if);
    if (netif)
    {
        /* Modify ip address to trigger tcp/udp pcb cleanup */
        netif_set_addr(netif, ip_2_ip4(IP4_ADDR_ANY), ip_2_ip4(IP4_ADDR_ANY), ip_2_ip4(IP4_ADDR_ANY));
        netif_set_down(netif);
    }

    return ESP_OK;
}

esp_err_t tcpip_adapter_dhcps_start(tcpip_adapter_if_t tcpip_if)
{
    return ESP_OK;
}

esp_err_t tcpip_adapter_dhcps_stop(tcpip_adapter_if_t tcpip_if)
{
    return ESP_OK;
}

esp_err_t tcpip_adapter_dhcpc_get_status(tcpip_adapter_if_t tcpip_if, tcpip_adapter_dhcp_status_t *status)
{
    struct netif *netif = esp32_wlan_get_interface(tcpip_if);
    if (netif)
    {
    }

    return ESP_OK;
}

esp_err_t tcpip_adapter_dhcpc_start(tcpip_adapter_if_t tcpip_if)
{
    struct netif *netif = esp32_wlan_get_interface(tcpip_if);
    if (netif)
    {
        if (netif_is_up(netif))
        {
            netif_set_addr(netif, ip_2_ip4(IP4_ADDR_ANY), ip_2_ip4(IP4_ADDR_ANY), ip_2_ip4(IP4_ADDR_ANY));
        }
        dhcp_start(netif);
    }

    return ESP_OK;
}

esp_err_t tcpip_adapter_dhcpc_stop(tcpip_adapter_if_t tcpip_if)
{
    struct netif *netif = esp32_wlan_get_interface(tcpip_if);
    if (netif)
    {
        dhcp_stop(netif);
    }

    return ESP_OK;
}

esp_err_t tcpip_adapter_ap_input(void *buffer, uint16_t len, void *eb)
{
    struct netif *netif = esp32_wlan_get_interface(TCPIP_ADAPTER_IF_AP);
    if (netif)
    {
        esp32_wlanif_input(netif, buffer, len, eb);
    }

    return ESP_OK;
}

esp_err_t tcpip_adapter_sta_input(void *buffer, uint16_t len, void *eb)
{
    struct netif *netif = esp32_wlan_get_interface(TCPIP_ADAPTER_IF_STA);
    if (netif)
    {
        esp32_wlanif_input(netif, buffer, len, eb);
    }

    return ESP_OK;
}
