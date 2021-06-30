import os
import sys
import rtconfig

if os.getenv('RTT_ROOT'):
    RTT_ROOT = os.getenv('RTT_ROOT')
else:
    RTT_ROOT = os.path.normpath(os.getcwd() + '/rt-thread')

sys.path = sys.path + [os.path.join(RTT_ROOT, 'tools')]
from building import *

IDF_PATH = os.path.normpath(os.getcwd() + 'idf-esp32')

TARGET = 'rtthread-%s.%s' % ('esp32', rtconfig.TARGET_EXT)

convert = 'python esp-idf-port/esp-idf/components/esptool_py/esptool/esptool.py --chip esp32 elf2image --flash_mode "dio" --flash_freq "40m" --flash_size "4MB"  -o $TARGET $SOURCE'
cvt= Builder(action = convert)

env = Environment(tools = ['mingw'],
    AS = rtconfig.AS, ASFLAGS = rtconfig.AFLAGS,
    CC = rtconfig.CC, CFLAGS = rtconfig.CFLAGS,
    CXX = rtconfig.CXX, CXXFLAGS = rtconfig.CXXFLAGS,
    AR = rtconfig.AR, ARFLAGS = '-rc',
    LINK = rtconfig.LINK, LINKFLAGS = rtconfig.LFLAGS)
env.PrependENVPath('PATH', rtconfig.EXEC_PATH)
env.Append(BUILDERS = {'ConvertELF': cvt})
env['ASCOM'] = env['ASPPCOM']

Export('RTT_ROOT')
Export('rtconfig')

# env['LINKCOM']=env['LINKCOM'].replace("$_LIBFLAGS","-Wl,--start-group $_LIBFLAGS -Wl,-end-group")
# print(env['LINKCOM'])

# prepare building environment
objs = PrepareBuilding(env, RTT_ROOT, has_libcpu=True)

# make a building
program = DoBuilding(TARGET, objs)

obj_files = []
for item in program:
    for child in item.all_children():
        if child.has_builder():
            file_ln = str(child)
            file_ln = file_ln.replace('\\', '/')
            # print(file_ln)

            obj_files.append(file_ln)
obj_files.sort()

link_file = open('link.txt', 'w')
for item in obj_files:
    link_file.write(item + '\n')
link_file.close()

env['LINKCOM']= '$LINK -o $TARGET esp-idf-port/windowspill_asm.o esp-idf-port/save_extra_nw.o esp-idf-port/restore_extra_nw.o $LINKFLAGS $__RPATH $_LIBDIRFLAGS $_LIBFLAGS @link.txt'

binfile = env.ConvertELF('rtthread.bin', TARGET)
