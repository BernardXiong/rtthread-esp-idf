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
    print '================ERROR============================'
    print 'Unknown compiler'
    print '================================================='
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

    DEVICE = ' '
    CFLAGS = DEVICE + '-Os -ffunction-sections -fdata-sections -fstrict-volatile-bitfields -mlongcalls -nostdlib -Wall'
    AFLAGS = ' -c' + DEVICE + ' -x assembler-with-cpp -MMD -MP -I./esp-idf-port/esp-idf/components/freertos/include/freertos -I./esp-idf-port/esp-idf/components/esp32/include -I./esp-idf-port/esp-idf/components/soc/esp32/include -I.'
    LFLAGS = DEVICE + ' -u call_user_start_cpu0 -u __cxa_guard_dummy -Wl,--undefined=uxTopUsedPriority,--gc-sections,-Map=rtthread-esp32.map,-cref,-static -nostdlib -T esp32_out.ld -T esp32.common.ld -T ./esp-idf-port/esp-idf/components/esp32/ld/esp32.rom.ld -T ./esp-idf-port/esp-idf/components/esp32/ld/esp32.peripherals.ld -T ./esp-idf-port/esp-idf/components/esp32/ld/esp32.rom.spiflash.ld'

    CPATH = ''
    LPATH = ''

    if BUILD == 'debug':
        CFLAGS += ' -ggdb'
        AFLAGS += ' -ggdb'
    else:
        CFLAGS += ' -O2'

    CXXFLAGS = CFLAGS + ' -std=gnu++11 -fno-exceptions -fno-rtti'
    CFLAGS   = CFLAGS + ' -std=gnu99'

    POST_ACTION = SIZE + ' $TARGET \n'
