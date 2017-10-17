
all:
	@echo "start compile"
	@scons
	python esp-idf-port/esp-idf/components/esptool_py/esptool/esptool.py --chip esp32 elf2image --flash_mode "dio" --flash_freq "40m" --flash_size "4MB"  -o rtthread.bin rtthread-esp32.elf

clean:
	scons -c
