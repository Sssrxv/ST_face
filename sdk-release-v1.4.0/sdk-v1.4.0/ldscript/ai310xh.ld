MEMORY
{
  ROM (RX) : ORIGIN = (0x00000000), LENGTH = (8 * 1024)
  SRAM (WX) : ORIGIN = (0x10000000), LENGTH = (236 * 1024)
  DDR (WX) : ORIGIN = (0x80000000), LENGTH = (64 * 1024 * 1024 - ((64 - 62) * 1024 * 1024))
  DMA (WX) : ORIGIN = ((0x80000000) + (64 * 1024 * 1024 - ((64 - 62) * 1024 * 1024))), LENGTH = ((64 - 62) * 1024 * 1024)
}
ENTRY(Reset_Handler)
SECTIONS
{
    .text :
    {
        Image$$VECTORS$$Base = .;
        * (RESET)
        KEEP(*(.isr_vector))
        Image$$VECTORS$$Limit = .;
        *(SVC_TABLE)
        *(.text*)
        KEEP(*(.init))
        KEEP(*(.fini))
        *crtbegin.o(.ctors)
        *crtbegin?.o(.ctors)
        *(EXCLUDE_FILE(*crtend?.o *crtend.o) .ctors)
        *(SORT(.ctors.*))
        *(.ctors)
        *crtbegin.o(.dtors)
        *crtbegin?.o(.dtors)
        *(EXCLUDE_FILE(*crtend?.o *crtend.o) .dtors)
        *(SORT(.dtors.*))
        *(.dtors)
        Image$$RO_DATA$$Base = .;
        *(.rodata*)
        Image$$RO_DATA$$Limit = .;
        KEEP(*(.eh_frame*))
    } > DDR
    .ARM.extab :
    {
        *(.ARM.extab* .gnu.linkonce.armextab.*)
    } > DDR
    __exidx_start = .;
    .ARM.exidx :
    {
        *(.ARM.exidx* .gnu.linkonce.armexidx.*)
    } > DDR
    __exidx_end = .;
    .zero.table :
    {
        . = ALIGN(4);
        __zero_table_start__ = .;
        LONG (__bss_start__)
        LONG (__bss_end__ - __bss_start__)
        __zero_table_end__ = .;
    } > DDR
    __etext = .;
    .data ALIGN(0x400):
    {
        Image$$RW_DATA$$Base = .;
        __data_start__ = .;
        *(vtable)
        *(.data*)
        Image$$RW_DATA$$Limit = .;
        . = ALIGN(4);
        PROVIDE (__preinit_array_start = .);
        KEEP(*(.preinit_array))
        PROVIDE (__preinit_array_end = .);
        . = ALIGN(4);
        PROVIDE (__init_array_start = .);
        KEEP(*(SORT(.init_array.*)))
        KEEP(*(.init_array))
        PROVIDE (__init_array_end = .);
        . = ALIGN(4);
        PROVIDE (__fini_array_start = .);
        KEEP(*(SORT(.fini_array.*)))
        KEEP(*(.fini_array))
        PROVIDE (__fini_array_end = .);
        . = ALIGN(4);
        _shell_command_start = .;
        KEEP (*(shellCommand))
        _shell_command_end = .;
        . = ALIGN(4);
        _device_list_start = .;
        KEEP (*(.udevice_list))
        _device_list_end = .;
        . = ALIGN(4);
        _MODULE_TEST_start = .;
        KEEP (*(MODULE_TEST))
        _MODULE_TEST_end = .;
        . = ALIGN(4);
        _segger_rtt_start = .;
        KEEP (*(.segger_rtt))
        _segger_rtt_end = .;
        . = ALIGN(4);
        __data_end__ = .;
    } > DDR
    .bss ALIGN(0x400):
    {
        Image$$ZI_DATA$$Base = .;
        __bss_start__ = .;
        *(.bss*)
        *(COMMON)
        __bss_end__ = .;
        Image$$ZI_DATA$$Limit = .;
        __end__ = .;
        end = __end__;
    } > DDR
    .heap (NOLOAD):
    {
        . = ORIGIN(DDR) + (3 * 1024 * 1024);
        . = ALIGN(8);
        Image$$HEAP$$ZI$$Base = .;
        . += (58 * 1024 * 1024);
        Image$$HEAP$$ZI$$Limit = .;
        __HeapLimit = .;
    } > DDR
    .dma (NOLOAD):
    {
        . = ALIGN(64);
        KEEP(*(.dma*))
        . = ALIGN(64);
    } > DMA
    .dma_heap (NOLOAD):
    {
        . = ALIGN(64);
        Image$$DMA_HEAP$$ZI$$Base = .;
        . += (1 * 1024 * 1024);
        Image$$DMA_HEAP$$ZI$$Limit = .;
        . = ALIGN(64);
    } > DMA
    .stack (NOLOAD):
    {
        . = ORIGIN(DDR) + (3 * 1024 * 1024) + (58 * 1024 * 1024);
        . = ALIGN(8);
        __StackTop = .;
        Image$$SYS_STACK$$ZI$$Base = .;
        . += ((62 * 1024 * 1024) - (3 * 1024 * 1024) - (58 * 1024 * 1024)) - (256) - (1024) - (16*1024) - (16*1024) - (256);
        Image$$SYS_STACK$$ZI$$Limit = .;
        __stack = .;
        Image$$FIQ_STACK$$ZI$$Base = .;
        . += (256);
        Image$$FIQ_STACK$$ZI$$Limit = .;
        Image$$IRQ_STACK$$ZI$$Base = .;
        . += (16*1024);
        Image$$IRQ_STACK$$ZI$$Limit = .;
        Image$$SVC_STACK$$ZI$$Base = .;
        . += (16*1024);
        Image$$SVC_STACK$$ZI$$Limit = .;
        Image$$ABT_STACK$$ZI$$Base = .;
        . += (1024);
        Image$$ABT_STACK$$ZI$$Limit = .;
        Image$$UND_STACK$$ZI$$Base = .;
        . += (256);
        Image$$UND_STACK$$ZI$$Limit = .;
    } > DDR
}
