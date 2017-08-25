#ifndef RT_CONFIG_H__
#define RT_CONFIG_H__

/* Automatically generated file; DO NOT EDIT. */
/* RT-Thread Project Configuration */

/* RT-Thread Kernel */

#define RT_NAME_MAX 8
#define RT_ALIGN_SIZE 4
#define RT_THREAD_PRIORITY_MAX 32
#define RT_TICK_PER_SECOND 100
#define RT_DEBUG
#define RT_USING_OVERFLOW_CHECK
/* RT_DEBUG_INIT is not set */
/* RT_DEBUG_THREAD is not set */
#define RT_USING_HOOK
#define RT_USING_IDLE_HOOK
#define RT_USING_TIMER_SOFT
#define RT_TIMER_THREAD_PRIO 4
#define RT_TIMER_THREAD_STACK_SIZE 1024
#define IDLE_THREAD_STACK_SIZE 1024

/* Inter-Thread communication */

#define RT_USING_SEMAPHORE
#define RT_USING_MUTEX
#define RT_USING_EVENT
#define RT_USING_MAILBOX
#define RT_USING_MESSAGEQUEUE

/* Memory Management */

#define RT_USING_MEMPOOL
#define RT_USING_MEMHEAP
#define RT_USING_HEAP
#define RT_USING_SMALL_MEM
/* RT_USING_SLAB is not set */

/* Kernel Device Object */

#define RT_USING_DEVICE
#define RT_USING_CONSOLE
#define RT_CONSOLEBUF_SIZE 128
#define RT_CONSOLE_DEVICE_NAME "uart"
/* RT_USING_MODULE is not set */

/* RT-Thread Components */

/* RT_USING_COMPONENTS_INIT is not set */

/* C++ features */

/* RT_USING_CPLUSPLUS is not set */

/* Command shell */

#define RT_USING_FINSH
#define FINSH_USING_SYMTAB
#define FINSH_USING_DESCRIPTION
#define FINSH_THREAD_STACK_SIZE 4096
/* FINSH_USING_AUTH is not set */
#define FINSH_DEFAULT_PASSWORD "rtthread"
#define FINSH_CMD_SIZE 256
#define FINSH_THREAD_PRIORITY 20
#define FINSH_USING_MSH
#define FINSH_USING_MSH_DEFAULT
/* FINSH_USING_MSH_ONLY is not set */

/* Device virtual file system */

#define RT_USING_DFS
#define DFS_USING_WORKDIR
#define DFS_FILESYSTEMS_MAX 4
#define DFS_FD_MAX 4
#define RT_USING_DFS_ELMFAT
#define RT_DFS_ELM_CODE_PAGE 437
/* ELM_LFN_MODE0 is not set */
/* ELM_LFN_MODE1 is not set */
/* ELM_LFN_MODE2 is not set */
#define ELM_LFN_MODE3
#define RT_DFS_ELM_USE_LFN 3
#define RT_DFS_ELM_MAX_LFN 255
#define RT_DFS_ELM_DRIVES 2
#define RT_DFS_ELM_MAX_SECTOR_SIZE 512
/* RT_DFS_ELM_USE_ERASE is not set */
#define RT_DFS_ELM_REENTRANT
#define RT_USING_DFS_DEVFS
/* RT_USING_DFS_NET is not set */
#define RT_USING_DFS_ROMFS
/* RT_USING_DFS_NFS is not set */

/* Device Drivers */

#define RT_USING_DEVICE_IPC
#define RT_USING_SERIAL
/* RT_USING_CAN is not set */
/* RT_USING_HWTIMER is not set */
#define RT_USING_I2C
#define RT_USING_I2C_BITOPS
#define RT_USING_PIN
/* RT_USING_MTD_NOR is not set */
/* RT_USING_MTD_NAND is not set */
/* RT_USING_RTC is not set */
/* RT_USING_SDIO is not set */
/* RT_USING_SPI is not set */
/* RT_USING_WDT is not set */
/* RT_USING_USB_HOST is not set */
/* RT_USING_USB_DEVICE is not set */

/* libc */

#define RT_USING_LIBC
#define RT_USING_PTHREADS

/* Network stack */

/* light weight TCP/IP stack */

#define RT_USING_LWIP
/* RT_USING_LWIP141 is not set */
/* RT_USING_LWIP200 is not set */
#define RT_USING_LWIP202
#define RT_LWIP_IPV6
#define RT_LWIP_IGMP
#define RT_LWIP_ICMP
/* RT_LWIP_SNMP is not set */
#define RT_LWIP_DNS
#define RT_LWIP_DHCP
#define IP_SOF_BROADCAST 1
#define IP_SOF_BROADCAST_RECV 1
#define LWIP_USING_DHCPD
#define RT_LWIP_UDP
#define RT_LWIP_TCP
/* RT_LWIP_PPP is not set */
#define RT_LWIP_PBUF_NUM 16
#define RT_LWIP_NETCONN_NUM 8
#define RT_LWIP_RAW_PCB_NUM 4
#define RT_LWIP_UDP_PCB_NUM 4
#define RT_LWIP_TCP_PCB_NUM 4
#define RT_LWIP_TCP_SEG_NUM 40
#define RT_LWIP_TCP_SND_BUF 8196
#define RT_LWIP_TCP_WND 8196
#define RT_LWIP_TCPTHREAD_PRIORITY 10
#define RT_LWIP_TCPTHREAD_MBOX_SIZE 8
#define RT_LWIP_TCPTHREAD_STACKSIZE 2048
#define RT_LWIP_ETHTHREAD_PRIORITY 12
#define RT_LWIP_ETHTHREAD_STACKSIZE 1024
#define RT_LWIP_ETHTHREAD_MBOX_SIZE 8
#define RT_LWIP_REASSEMBLY_FRAG
#define LWIP_NETIF_STATUS_CALLBACK 1
#define LWIP_SOCKOPT
#define SO_REUSE 1
#define LWIP_SO_RCVTIMEO 1
#define LWIP_SO_SNDTIMEO 1
#define LWIP_SO_RCVBUF 1

/* Modbus master and slave stack */

/* RT_USING_MODBUS is not set */

/* RT-Thread online packages */

/* IoT - internet of things */

/* language packages */

/* miscellaneous packages */

/* PKG_USING_HELLO is not set */

/* multimedia packages */

/* security packages */

/* system packages */

#define RT_USING_UART1
#define RT_USING_UART2
#define RT_USING_ESP_PSRAM
#define RT_ESP_PSRAM_SIZE 4096
#define CONFIG_ESP_AUDIO
#define CONFIG_CHIP_ESP32
/* CONFIG_BOARD_ESP32 is not set */
#define CONFIG_BOARD_RTIOT_AUDIO
/* CONFIG_BOARD_LYRAT is not set */
#define RT_USING_WIFI

/* Multi-Media Audio */

#define RT_USING_AUDIO_PLAYER

/* components in IDF of Espressif */

/* IDF_MBEDTLS is not set */
/* IDF_LWIP is not set */
#include "rtconfig_project.h"

#endif
