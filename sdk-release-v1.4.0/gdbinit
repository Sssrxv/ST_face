source scripts/gdb/gdbdbg.gdb
source scripts/gdb/sram.gdb
reset

load examples/rtos/bin/rtos_example.elf
file examples/rtos/bin/rtos_example.elf

#WARNING: "set $pc=Reset_Handler" must set before "b main"
#set $pc=Reset_Handler
b main