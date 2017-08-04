/*  Default entry point:  */
ENTRY(call_start_cpu0);

SECTIONS
{
  /* RTC fast memory holds RTC wake stub code,
     including from any source file named rtc_wake_stub*.c
  */
  .rtc.text :
  {
    . = ALIGN(4);
    *(.rtc.literal .rtc.text)
    *rtc_wake_stub*.o(.literal .text .literal.* .text.*)
  } >rtc_iram_seg

  /* RTC slow memory holds RTC wake stub
     data/rodata, including from any source file
     named rtc_wake_stub*.c
  */
  .rtc.data :
  {
    _rtc_data_start = ABSOLUTE(.);
    *(.rtc.data)
    *(.rtc.rodata)
    *rtc_wake_stub*.o(.data .rodata .data.* .rodata.* .bss .bss.*)
    _rtc_data_end = ABSOLUTE(.);
  } > rtc_slow_seg

  /* RTC bss, from any source file named rtc_wake_stub*.c */
  .rtc.bss (NOLOAD) :
  {
    _rtc_bss_start = ABSOLUTE(.);
    *rtc_wake_stub*.o(.bss .bss.*)
    *rtc_wake_stub*.o(COMMON)
    _rtc_bss_end = ABSOLUTE(.);
  } > rtc_slow_seg

  /* Send .iram0 code to iram */
  .iram0.vectors :
  {
    /* Vectors go to IRAM */
    _init_start = ABSOLUTE(.);
    /* Vectors according to builds/RF-2015.2-win32/esp108_v1_2_s5_512int_2/config.html */
    . = 0x0;
    KEEP(*(.WindowVectors.text));
    . = 0x180;
    KEEP(*(.Level2InterruptVector.text));
    . = 0x1c0;
    KEEP(*(.Level3InterruptVector.text));
    . = 0x200;
    KEEP(*(.Level4InterruptVector.text));
    . = 0x240;
    KEEP(*(.Level5InterruptVector.text));
    . = 0x280;
    KEEP(*(.DebugExceptionVector.text));
    . = 0x2c0;
    KEEP(*(.NMIExceptionVector.text));
    . = 0x300;
    KEEP(*(.KernelExceptionVector.text));
    . = 0x340;
    KEEP(*(.UserExceptionVector.text));
    . = 0x3C0;
    KEEP(*(.DoubleExceptionVector.text));
    . = 0x400;
    *(.*Vector.literal)

    *(.UserEnter.literal);
    *(.UserEnter.text);
    . = ALIGN (16);
    *(.entry.text)
    *(.init.literal)
    *(.init)
    _init_end = ABSOLUTE(.);

    /* This goes here, not at top of linker script, so addr2line finds it last,
       and uses it in preference to the first symbol in IRAM */
    _iram_start = ABSOLUTE(0);
  } > iram0_0_seg

  .iram0.text :
  {
    /* Code marked as runnning out of IRAM */
    _iram_text_start = ABSOLUTE(.);
    *(.iram1 .iram1.*)

    build/esp-idf-port/stubs/fmq.o(.literal .text .literal.* .text.*)
    build/esp-idf-port/stubs/libcpu.o(.literal .text .literal.* .text.*)

    /* freertos stub */
    build/esp-idf-port/esp-idf/components/freertos/croutine.o(.literal .text .literal.* .text.*)
    build/esp-idf-port/esp-idf/components/freertos/heap_regions.o(.literal .text .literal.* .text.*)
    build/esp-idf-port/esp-idf/components/freertos/heap_regions_debug.o(.literal .text .literal.* .text.*)
    build/esp-idf-port/esp-idf/components/freertos/list.o(.literal .text .literal.* .text.*)
    build/esp-idf-port/esp-idf/components/freertos/port.o(.literal .text .literal.* .text.*)
    build/esp-idf-port/esp-idf/components/freertos/portasm.o(.literal .text .literal.* .text.*)
    build/esp-idf-port/esp-idf/components/freertos/ringbuf.o(.literal .text .literal.* .text.*)
    build/esp-idf-port/esp-idf/components/freertos/xtensa_context.o(.literal .text .literal.* .text.*)
    build/esp-idf-port/esp-idf/components/freertos/xtensa_init.o(.literal .text .literal.* .text.*)
    build/esp-idf-port/esp-idf/components/freertos/xtensa_intr.o(.literal .text .literal.* .text.*)
    build/esp-idf-port/esp-idf/components/freertos/xtensa_intr_asm.o(.literal .text .literal.* .text.*)
    build/esp-idf-port/esp-idf/components/freertos/xtensa_overlay_os_hook.o(.literal .text .literal.* .text.*)
    build/esp-idf-port/esp-idf/components/freertos/xtensa_vectors.o(.literal .text .literal.* .text.*)

    /* rt-thread kernel */
    build/kernel/src/clock.o(.literal .text .literal.* .text.*)
    build/kernel/src/device.o(.literal .text .literal.* .text.*)
    build/kernel/src/idle.o(.literal .text .literal.* .text.*)
    build/kernel/src/ipc.o(.literal .text .literal.* .text.*)
    build/kernel/src/irq.o(.literal .text .literal.* .text.*)
    build/kernel/src/kservice.o(.literal .text .literal.* .text.*)
    build/kernel/src/object.o(.literal .text .literal.* .text.*)
    build/kernel/src/scheduler.o(.literal .text .literal.* .text.*)
    build/kernel/src/thread.o(.literal .text .literal.* .text.*)
    build/kernel/src/timer.o(.literal .text .literal.* .text.*)

    build/esp-idf-port/esp-idf/components/esp32/panic.o(.literal .text .literal.* .text.*)
    build/esp-idf-port/esp-idf/components/esp32/core_dump.o(.literal .text .literal.* .text.*)
    build/esp-idf-port/esp-idf/components/esp32/heap_alloc_caps.o(.literal .text .literal.* .text.*)
    build/esp-idf-port/esp-idf/components/spi_flash/spi_flash_rom_patch.o(.literal .text .literal.* .text.*)
    /* build/esp-idf-port/esp-idf/components/driver/i2s.o(.literal .text .literal.* .text.*) */

    *libphy.a:(.literal .text .literal.* .text.*)
    *librtc.a:(.literal .text .literal.* .text.*)
    *libsoc.a:(.literal .text .literal.* .text.*)
    *libhal.a:(.literal .text .literal.* .text.*)
    _iram_text_end = ABSOLUTE(.);
  } > iram0_0_seg

  .dram0.data :
  {
    _data_start = ABSOLUTE(.);
    KEEP(*(.data))
    KEEP(*(.data.*))
    KEEP(*(.gnu.linkonce.d.*))
    KEEP(*(.data1))
    KEEP(*(.sdata))
    KEEP(*(.sdata.*))
    KEEP(*(.gnu.linkonce.s.*))
    KEEP(*(.sdata2))
    KEEP(*(.sdata2.*))
    KEEP(*(.gnu.linkonce.s2.*))
    KEEP(*(.jcr))
    *(.dram1 .dram1.*)
    *libesp32.a:panic.o(.rodata .rodata.*)
    *libesp32.a:app_trace.o(.rodata .rodata.*)
    *libphy.a:(.rodata .rodata.*)
    _data_end = ABSOLUTE(.);
    . = ALIGN(4);
  } >dram0_0_seg

  /* Shared RAM */
  .dram0.bss (NOLOAD) :
  {
    . = ALIGN (8);
    _bss_start = ABSOLUTE(.);
    *(.dynsbss)
    *(.sbss)
    *(.sbss.*)
    *(.gnu.linkonce.sb.*)
    *(.scommon)
    *(.sbss2)
    *(.sbss2.*)
    *(.gnu.linkonce.sb2.*)
    *(.dynbss)
    *(.bss)
    *(.bss.*)
    *(.share.mem)
    *(.gnu.linkonce.b.*)
    *(COMMON)
    . = ALIGN (8);
    _bss_end = ABSOLUTE(.);
    _heap_start = ABSOLUTE(.);
  } >dram0_0_seg

  .flash.rodata :
  {
    _rodata_start = ABSOLUTE(.);
    *(.rodata)
    *(.rodata.*)
    *(.irom1.text) /* catch stray ICACHE_RODATA_ATTR */
    *(.gnu.linkonce.r.*)
    *(.rodata1)
    __XT_EXCEPTION_TABLE_ = ABSOLUTE(.);
    *(.xt_except_table)
    *(.gcc_except_table)
    *(.gnu.linkonce.e.*)
    *(.gnu.version_r)
    *(.eh_frame)
    . = (. + 3) & ~ 3;

    /* section information for finsh shell */
    . = ALIGN(4);
    __fsymtab_start = .;
    KEEP(*(FSymTab))
    __fsymtab_end = .;
    . = ALIGN(4);
    __vsymtab_start = .;
    KEEP(*(VSymTab))
    __vsymtab_end = .;
    . = ALIGN(4); 

    /* section information for initial. */
    . = ALIGN(4);
    __rt_init_start = .;
    KEEP(*(SORT(.rti_fn*)))
    __rt_init_end = .;
    . = ALIGN(4);

    /*  C++ constructor and destructor tables, properly ordered:  */
    __init_array_start = ABSOLUTE(.);
    KEEP (*crtbegin.o(.ctors))
    KEEP (*(EXCLUDE_FILE (*crtend.o) .ctors))
    KEEP (*(SORT(.ctors.*)))
    KEEP (*(.ctors))
    __init_array_end = ABSOLUTE(.);
    KEEP (*crtbegin.o(.dtors))
    KEEP (*(EXCLUDE_FILE (*crtend.o) .dtors))
    KEEP (*(SORT(.dtors.*)))
    KEEP (*(.dtors))
    /*  C++ exception handlers table:  */
    __XT_EXCEPTION_DESCS_ = ABSOLUTE(.);
    *(.xt_except_desc)
    *(.gnu.linkonce.h.*)
    __XT_EXCEPTION_DESCS_END__ = ABSOLUTE(.);
    *(.xt_except_desc_end)
    *(.dynamic)
    *(.gnu.version_d)
    _rodata_end = ABSOLUTE(.);
    /* Literals are also RO data. */
    _lit4_start = ABSOLUTE(.);
    *(*.lit4)
    *(.lit4.*)
    *(.gnu.linkonce.lit4.*)
    _lit4_end = ABSOLUTE(.);
    . = ALIGN(4);
  } >drom0_0_seg

  .flash.text :
  {
    _stext = .;
    _text_start = ABSOLUTE(.);
    *(.literal .text .literal.* .text.* .stub .gnu.warning .gnu.linkonce.literal.* .gnu.linkonce.t.*.literal .gnu.linkonce.t.*)
    *(.irom0.text) /* catch stray ICACHE_RODATA_ATTR */
    *(.fini.literal)
    *(.fini)
    *(.gnu.version)
    _text_end = ABSOLUTE(.);
    _etext = .;

    /* Similar to _iram_start, this symbol goes here so it is
       resolved by addr2line in preference to the first symbol in
       the flash.text segment.
    */
    _flash_cache_start = ABSOLUTE(0);
  } >iram0_2_seg
}
