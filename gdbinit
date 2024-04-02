cd sdk-release-v1.4.0/
source scripts/gdb/gdbdbg.gdb
source scripts/gdb/sram.gdb
reset

cd ../
load xt4/bin/xt4.elf
file xt4/bin/xt4.elf

#WARNING: "set $pc=Reset_Handler" must set before "b main"
#set $pc=Reset_Handler
b main