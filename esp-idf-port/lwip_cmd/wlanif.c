/*
 * File      : stk_emac.c
 *             EMAC Ethernet driver
 * COPYRIGHT (C) 2015, Shanghai Real-Thread Electronic Technology Co.,Ltd
 *
 * Change Logs:
 * Date           Author       Notes
 * 2017/7/1       Bernard      The first version
 */
#include <string.h>
#include <rtthread.h>
#include <netif/ethernetif.h>

#include <lwipopts.h>
#include <esp_interface.h>
#include <esp_wifi.h>
#include <esp_wifi_internal.h>

#define MAX_ADDR_LEN                6
#define ESP32_WLAN_DEVICE(eth)      (struct esp32_wlan*)(eth)

struct esp32_wlan
{
    /* inherit from Ethernet device */
    struct eth_device parent;

    /* interface address info. */
    rt_uint8_t  dev_addr[MAX_ADDR_LEN];         /* MAC address  */

    esp_interface_t netif; /* STA/AP */
};
static struct esp32_wlan _emac;

static rt_err_t esp32_wlan_init(rt_device_t dev)
{
    return RT_EOK;
}

static rt_err_t esp32_wlan_open(rt_device_t dev, rt_uint16_t oflag)
{
    return RT_EOK;
}

static rt_err_t esp32_wlan_close(rt_device_t dev)
{
    return RT_EOK;
}

static rt_size_t esp32_wlan_read(rt_device_t dev, rt_off_t pos, void* buffer, rt_size_t size)
{
    rt_set_errno(-RT_ENOSYS);
    return 0;
}

static rt_size_t esp32_wlan_write (rt_device_t dev, rt_off_t pos, const void* buffer, rt_size_t size)
{
    rt_set_errno(-RT_ENOSYS);
    return 0;
}

static rt_err_t esp32_wlan_control(rt_device_t dev, rt_uint8_t cmd, void *args)
{
    struct esp32_wlan *emac;

    emac = ESP32_WLAN_DEVICE(dev);
    RT_ASSERT(emac != RT_NULL);

    switch(cmd)
    {
    case NIOCTL_GADDR:
        /* get MAC address */
        if(args) memcpy(args, emac->dev_addr, 6);
        else return -RT_ERROR;
        break;

    default :
        break;
    }

    return RT_EOK;
}

/* Ethernet device interface */
/* transmit packet. */
rt_err_t esp32_wlan_tx(rt_device_t dev, struct pbuf* p)
{
    rt_err_t result = RT_EOK;
    struct esp32_wlan *emac;
    struct pbuf *q = p;
    err_t ret;

    emac = ESP32_WLAN_DEVICE(dev);
    RT_ASSERT(emac != RT_NULL);

    if(q->next == NULL)
    {
        ret = esp_wifi_internal_tx(emac->netif, q->payload, q->len);
        
    }
    else
    {
        q = pbuf_alloc(PBUF_RAW_TX, p->tot_len, PBUF_RAM);
        if (q != NULL)
        {
            pbuf_copy(q, p);
        }
        else
        {
            return -RT_ENOMEM;
        }

        ret = esp_wifi_internal_tx(emac->netif, q->payload, q->len);
        pbuf_free(q);
    }

    if (ret != ERR_OK)
        result = -RT_EIO;

    return result;
}

/* reception packet. */
struct pbuf *esp32_wlan_rx(rt_device_t dev)
{
    /* not use this interface to handle wlan rx */
    return NULL;
}

void esp32_wlanif_input(struct netif *netif, void *buffer, u16_t len, void* eb)
{
    struct pbuf *p;

    if(!buffer || !netif)
        goto _exit;

    p = pbuf_alloc(PBUF_RAW, len, PBUF_REF);
    if (p == NULL)
    {
        return;
    }
    p->payload = buffer;

    /* full packet send to tcpip_thread to process */
    if (netif->input(p, netif) != ERR_OK) 
    {
        LWIP_DEBUGF(NETIF_DEBUG, ("ethernetif_input: IP input error\n"));
        pbuf_free(p);
    }

_exit:
    return;
}

int esp32_wlan_hw_init(esp_interface_t netif)
{
    /* set MAC address */
    esp_wifi_get_mac(netif, &_emac.dev_addr[0]);
    _emac.netif       = netif;

    _emac.parent.parent.init       = esp32_wlan_init;
    _emac.parent.parent.open       = esp32_wlan_open;
    _emac.parent.parent.close      = esp32_wlan_close;
    _emac.parent.parent.read       = esp32_wlan_read;
    _emac.parent.parent.write      = esp32_wlan_write;
    _emac.parent.parent.control    = esp32_wlan_control;
    _emac.parent.parent.user_data  = RT_NULL;

    _emac.parent.eth_rx     = esp32_wlan_rx;
    _emac.parent.eth_tx     = esp32_wlan_tx;

    /* register ETH device */
    eth_device_init(&(_emac.parent), "w0");

    return 0;
}
INIT_DEVICE_EXPORT(esp32_wlan_hw_init);
