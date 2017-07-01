#include <stdio.h>
#include <stdlib.h>

int reent_std_init(void)
{
    FILE* std_console;
    const char* console = "/dev/console";

    std_console = fopen(console, "w");
    _GLOBAL_REENT->_stdin  = std_console;
    _GLOBAL_REENT->_stdout = std_console;
    _GLOBAL_REENT->_stderr = std_console;

    return 0;
}
