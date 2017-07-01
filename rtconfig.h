#ifndef RT_CONFIG_H__
#define RT_CONFIG_H__

/* Automatically generated file; DO NOT EDIT. */
/* RT-Thread Project Configuration */

/* RT-Thread Kernel */

#define RT_NAME_MAX 8
#define RT_ALIGN_SIZE 4
#define RT_THREAD_PRIORITY_MAX 32
#define RT_TICK_PER_SECOND 1000
#define RT_DEBUG
#define RT_USING_OVERFLOW_CHECK
/* CONFIG_RT_DEBUG_INIT is not set */
/* CONFIG_RT_DEBUG_THREAD is not set */
#define RT_USING_HOOK
#define RT_USING_IDLE_HOOK
#define RT_USING_TIMER_SOFT
#define RT_TIMER_THREAD_PRIO 4
#define RT_TIMER_THREAD_STACK_SIZE 2048
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
/* CONFIG_RT_USING_SLAB is not set */

/* Kernel Device Object */

#define RT_USING_DEVICE
#define RT_USING_CONSOLE
#define RT_CONSOLEBUF_SIZE 128
#define RT_CONSOLE_DEVICE_NAME "uart"
/* CONFIG_RT_USING_MODULE is not set */

/* RT-Thread Components */

/* CONFIG_RT_USING_COMPONENTS_INIT is not set */

/* C++ features */

/* CONFIG_RT_USING_CPLUSPLUS is not set */

/* Command shell */

#define RT_USING_FINSH
#define FINSH_USING_SYMTAB
#define FINSH_USING_DESCRIPTION
#define FINSH_THREAD_STACK_SIZE 4096
/* CONFIG_FINSH_USING_AUTH is not set */
#define FINSH_DEFAULT_PASSWORD "rtthread"
#define FINSH_USING_MSH
#define FINSH_USING_MSH_DEFAULT
#define FINSH_USING_MSH_ONLY

/* Device virtual file system */

#define RT_USING_DFS
#define DFS_USING_WORKDIR
#define DFS_FILESYSTEMS_MAX 2
#define DFS_FD_MAX 4
#define RT_USING_DFS_ELMFAT
#define RT_DFS_ELM_CODE_PAGE 437
#define RT_DFS_ELM_MAX_LFN 256
#define RT_DFS_ELM_DRIVES 2
#define RT_DFS_ELM_MAX_SECTOR_SIZE 512
/* CONFIG_RT_DFS_ELM_USE_ERASE is not set */
#define RT_DFS_ELM_REENTRANT
#define RT_USING_DFS_DEVFS
/* CONFIG_RT_USING_DFS_NET is not set */
/* CONFIG_RT_USING_DFS_NFS is not set */

/* Device Drivers */

#define RT_USING_DEVICE_IPC
#define RT_USING_SERIAL
/* CONFIG_RT_USING_CAN is not set */
/* CONFIG_RT_USING_HWTIMER is not set */
#define RT_USING_I2C
#define RT_USING_I2C_BITOPS
#define RT_USING_PIN
/* CONFIG_RT_USING_MTD_NOR is not set */
/* CONFIG_RT_USING_MTD_NAND is not set */
/* CONFIG_RT_USING_RTC is not set */
/* CONFIG_RT_USING_SDIO is not set */
/* CONFIG_RT_USING_SPI is not set */
/* CONFIG_RT_USING_WDT is not set */
/* CONFIG_RT_USING_USB_HOST is not set */
/* CONFIG_RT_USING_USB_DEVICE is not set */

/* libc */

#define RT_USING_LIBC
#define RT_USING_PTHREADS

/* Network stack */

/* light weight TCP/IP stack */

#define RT_USING_LWIP
/* CONFIG_RT_USING_LWIP141 is not set */
/* CONFIG_RT_USING_LWIP200 is not set */
#define RT_USING_LWIP202
#define RT_LWIP_IGMP
#define RT_LWIP_ICMP
/* CONFIG_RT_LWIP_SNMP is not set */
#define RT_LWIP_DNS
#define RT_LWIP_DHCP
#define IP_SOF_BROADCAST 1
#define IP_SOF_BROADCAST_RECV 1
#define LWIP_USING_DHCPD
#define RT_LWIP_UDP
#define RT_LWIP_TCP
/* CONFIG_RT_LWIP_PPP is not set */
#define RT_LWIP_PBUF_NUM 16
#define RT_LWIP_RAW_PCB_NUM 4
#define RT_LWIP_UDP_PCB_NUM 4
#define RT_LWIP_TCP_PCB_NUM 4
#define RT_LWIP_TCP_SEG_NUM 40
#define RT_LWIP_TCP_SND_BUF 8196
#define RT_LWIP_TCP_WND 8196
#define RT_LWIP_TCPTHREAD_PRIORITY 10
#define RT_LWIP_TCPTHREAD_MBOX_SIZE 8
#define RT_LWIP_TCPTHREAD_STACKSIZE 1024
#define RT_LWIP_ETHTHREAD_PRIORITY 12
#define RT_LWIP_ETHTHREAD_STACKSIZE 1024
#define RT_LWIP_ETHTHREAD_MBOX_SIZE 8
#define RT_LWIP_REASSEMBLY_FRAG
#define LWIP_NETIF_STATUS_CALLBACK 1
/* CONFIG_LWIP_SOCKOPT is not set */

/* Modbus master and slave stack */

/* CONFIG_RT_USING_MODBUS is not set */

/* Board Support Package */

#define RT_USING_UART1
#define RT_USING_UART2
#define CONFIG_CHIP_ESP32
#define CONFIG_BOARD_ESP32
/* CONFIG_CONFIG_BOARD_ESP32_RTAUDIO is not set */
#define RT_USING_WIFI

/* RT-Thread online packages */

/* IoT - internet of things */

/* language packages */

/* miscellaneous packages */

/* CONFIG_PKG_USING_HELLO is not set */

/* multimedia packages */

/* security packages */

/* system packages */

#include "rtconfig_project.h"

#endif
