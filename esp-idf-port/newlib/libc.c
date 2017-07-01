#include <rtthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/time.h>
#include "libc.h"

#ifdef RT_USING_DFS
#include <dfs_posix.h>

#ifndef RT_USING_DFS_DEVFS
#error Please enable devfs by defining RT_USING_DFS_DEVFS in rtconfig.h
#endif

#include <devfs.h>
#endif

int libc_system_init(void)
{
#ifdef RT_USING_DFS
    struct rt_device *console_dev;

    console_dev = rt_console_get_device();
    if (console_dev)
    {
        /* initialize console device */
        rt_console_init(console_dev->parent.name);
    }
#endif

    return 0;
}
INIT_COMPONENT_EXPORT(libc_system_init);

