# rtthread-esp-idf
RT-Thread针对ESP32的移植（使用Espressif Systems的IDF框架）

# 获得rtthread esp-idf

获得RT-Thread的ESP32版本，需要安装git，并以以下方式获得代码：

```bash
git clone https://github.com/BernardXiong/rtthread-esp-idf

cd rtthread-esp-idf
git submodule init
git submodule update

cd esp-idf-port/esp-idf
git submodule init
git submodule update

git am ../0001-cpu_start-patch.patch
git am ../0002-add-esp_task.h.patch
git am ../0003-add-memset-to-0.patch
```

# 交叉编译工具链下载

* Linux版本：

- CentOS 7::

    sudo yum install git wget make ncurses-devel flex bison gperf python pyserial

- Ubuntu and Debian::

    sudo apt-get install git wget make libncurses-dev flex bison gperf python python-serial

- Arch::

    sudo pacman -S --needed gcc git make ncurses flex bison gperf python2-pyserial

[64位](https://dl.espressif.com/dl/xtensa-esp32-elf-linux64-1.22.0-61-gab8375a-5.2.0.tar.gz)
[32位](https://dl.espressif.com/dl/xtensa-esp32-elf-linux32-1.22.0-61-gab8375a-5.2.0.tar.gz)

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

# rt-iot audio开发板烧录

rt-iot audio开发板因为有音频的缘故，未包括串口自动复位烧录的操作，所以在进行烧录时，需要以按键方式辅助。

1. USB连接PC和开发板；
2. 在PC上运行烧写的软件，它会打开串口试图烧写；
3. 按住rt-iot audio朝外边的两个按键，然后按最内侧按键（reset按键），松开内侧按键，然后再松开外侧两个按键。

这个时候PC侧软件自动进入烧写模式，开始进行固件烧写。

## 音频

当前在applications下放了两个音频相关的代码：

* wav_play.c -- 用于播放一个wav文件；
* wav_record.c -- 用于录音（默认以16000 samplerate、单声道方式录音，并保存为文件）
