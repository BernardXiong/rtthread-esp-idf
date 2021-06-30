import os

# toolchains options
ARCH='xtensa'
CPU='esp32'
CROSS_TOOL='gcc'
BOARD_NAME = 'ESP-WROOM-32'

if os.getenv('RTT_CC'):
    CROSS_TOOL = os.getenv('RTT_CC')

if  CROSS_TOOL == 'gcc':
    PLATFORM    = 'gcc'
    EXEC_PATH   = r'D:\tools\msys32\opt\xtensa-esp32-elf\bin'
else:
    print ('================ERROR============================')
    print ('Unknown compiler')
    print ('=================================================')
    exit(0)

if os.getenv('RTT_EXEC_PATH'):
    EXEC_PATH = os.getenv('RTT_EXEC_PATH')

BUILD = 'debug'

if PLATFORM == 'gcc':
    # toolchains
    PREFIX = 'xtensa-esp32-elf-'
    CC  = PREFIX + 'gcc'
    CXX = PREFIX + 'g++'
    AS = PREFIX + 'gcc'
    AR = PREFIX + 'ar'
    LINK = PREFIX + 'g++'
    TARGET_EXT = 'elf'
    SIZE = PREFIX + 'size'
    OBJDUMP = PREFIX + 'objdump'
    OBJCPY = PREFIX + 'objcopy'

    DEVICE = ' -mlongcalls -Wno-frame-address'
    CFLAGS = DEVICE + ' -ffunction-sections -fdata-sections -fstrict-volatile-bitfields -Wall -Wno-error=unused-function -Wno-error=unused-but-set-variable -Wno-error=unused-variable -Wno-error=deprecated-declarations -Wextra -Wno-unused-parameter -Wno-sign-compare'
    AFLAGS = ' -c' + DEVICE + ' -x assembler-with-cpp '
    LFLAGS = DEVICE + ' @esp-idf-port/idf-link.txt -Wl,--cref -Wl,--Map=rtthread.map '

    CPATH = ''
    LPATH = ''

    if BUILD == 'debug':
        CFLAGS += ' -ggdb -Og'
        AFLAGS += ' -ggdb'
    else:
        CFLAGS += ' -O2'

    CXXFLAGS = CFLAGS + ' -std=gnu++11 -fno-exceptions -fno-rtti'
    CFLAGS   = CFLAGS + ' -std=gnu99'

    POST_ACTION = SIZE + ' $TARGET \n'
