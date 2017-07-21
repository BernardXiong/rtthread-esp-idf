# rtthread-esp-idf
RT-Thread针对ESP32的移植（使用Espressif Systems的IDF框架）

# 获得rtthread esp-idf

获得RT-Thread的ESP32版本，需要安装git，并以以下方式获得代码：

```bash
git clone https://github.com/BernardXiong/rtthread-esp-idf
git submodule init
git submodule update

cd rtthread-esp-idf/esp-idf-port
git submodule init
git submodule update

cd esp-idf
git am ../0001-cpu_start-patch.patch
git am ../0002-add-esp_task.h.patch
```

# 交叉编译工具链下载

* Linux版本：

[64位](https://dl.espressif.com/dl/xtensa-esp32-elf-linux64-1.22.0-59.tar.gz)
[32位](https://dl.espressif.com/dl/xtensa-esp32-elf-linux32-1.22.0-59.tar.gz)

* MacOS版本

[MacOS](https://dl.espressif.com/dl/xtensa-esp32-elf-osx-1.22.0-59.tar.gz)

* Windows版本

[Windows](https://dl.espressif.com/dl/xtensa-esp32-elf-win32-1.22.0-59.zip)

## 依赖软件
1. scons
2. python
3. pyserial

* 安装Python后，可以安装pip，以pip install scons pyserial的方式安装其他

# 编译

1. 修改根目录下"rtconfig.py"文件内EXEC_PATH = r'D:\tools\msys32\opt\xtensa-esp32-elf\bin'为xtensa-esp32-elf交叉编译器所在目录
2. 在命令行下使用scons命令编译
3. 使用python esp-idf/components/esptool_py/esptool/esptool.py --chip esp32 elf2image --flash_mode "dio" --flash_freq "40m" --flash_size "4MB"  -o rtthread.bin rtthread-esp32.elf命令生成bin文件
4. 也可直接使用make命令在根目录下编译并生成rtthread.bin文件
