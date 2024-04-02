# Connect to the J-Link gdb server
define jlink_connect
  target remote localhost:2331
end

define reset_peripherals
  # Disable all interrupts and go to supervisor mode
  mon reg cpsr = 0xd3

  # Reset the chip to get to a known state
  #monitor reset
  # FIXME: reset all clks except DDR
  set *0x20E00050 = 0xFFFFFFFF
  set *0x20E00054 = 0xFFFFFFFF
  set *0x20E00058 = 0xFFFFFFFF
  set *0x20E0005C = 0xFFFFFFFF
  set *0x20E00060 = 0xFFFFFFFF
  set *0x20E00064 = 0xFFFFFFFF
  
  set *0x20E00050 = 0x00000000
  set *0x20E00054 = 0x00000000
  set *0x20E00058 = 0x00000000
  set *0x20E0005C = 0x00000000
  set *0x20E00060 = 0x00000000
  set *0x20E00064 = 0x00000000

  # Reset peripherals (using RSTC_CR)
  #set *0xF8048000 = 0xA5000004

  # Reset L2 Cache controller
  #set *0x00A00100 = 0x0

  # Disable Watchdog (using WDT_MR)
  #set *0xF8048044 = 0x00008000

  # Disable D-Cache, I-Cache and MMU
  mon cp15 1 0 0 0 = 0x00C50078
end

# Disable DDR clock and MPDDRC controller
# to avoid corrupted RAM data on soft reset.
define disable_ddr
  #set *0xF0014004 = 0x4
  #set *0xF0014014 = (1 << 13)  
end

define reset_registers
  # Zero registers (cannot reset core because it will disable JTAG)
  mon reg r8_fiq = 0
  mon reg r9_fiq = 0
  mon reg r10_fiq = 0
  mon reg r11_fiq = 0
  mon reg r12_fiq = 0
  mon reg sp_fiq = 0
  mon reg lr_fiq = 0
  mon reg spsr_fiq = 0
  mon reg sp_irq = 0
  mon reg lr_irq = 0
  mon reg spsr_irq = 0
  mon reg sp_abt = 0
  mon reg lr_abt = 0
  mon reg spsr_abt = 0
  mon reg sp_und = 0
  mon reg lr_und = 0
  mon reg spsr_und = 0
  mon reg sp_svc = 0
  mon reg lr_svc = 0
  mon reg spsr_svc = 0
  mon reg r0 = 0
  mon reg r1 = 0
  mon reg r2 = 0
  mon reg r3 = 0
  mon reg r4 = 0
  mon reg r5 = 0
  mon reg r6 = 0
  mon reg r7 = 0
  mon reg r8_usr = 0
  mon reg r9_usr = 0
  mon reg r10_usr = 0
  mon reg r11_usr = 0
  mon reg r12_usr = 0
  mon reg sp_usr = 0
  mon reg lr_usr = 0  
end

define init_ddr  

  reset_registers
  
  #load target/bootstrap.elf
  
  # Initialize PC
  #mon reg pc = 0x00200000
  #mon reg pc = 0x00200000

  continue
end

define load_in_ddr

  reset_registers

  #load
  
  # Initialize PC
  #mon reg pc = 0x20000000
end

define load_in_sram
  reset_registers

  #load

  # Initialize PC
  #mon reg pc = 0x00200000
end

#define memory_to_file
    #if $argc != 3
        #echo memory_to_file [filename] [start_addr] [length]\n
    #else
        #set $start_addr = $arg1
        #set $end_addr   = $arg1 + $arg2
        #printf "Dump memory(0x%x ~ 0x%x) to %s\n", $start_addr, $end_addr, $arg0
        #eval "dump binary memory %s %d %d", $arg0, $start_addr, $end_addr
    #end
#end

#define file_to_memory
    #if $argc != 3
        #echo file_to_memory [filename] [start_addr] [length]\n
    #else
        #set $start_addr = $arg1
        #set $end_addr   = $arg1 + $arg2
        #printf "Load %s to memory(0x%x ~ 0x%x)\n", $arg0, $start_addr, $end_addr
        #eval "restore %s binary %d 0 %d", $arg0, $start_addr, $arg2
    #end
#end

define memory_to_file
    if $argc != 2
        echo memory_to_file [start_addr] [length]\n
    else
        set $start_addr = $arg0
        set $end_addr   = $arg0 + $arg1
        printf "Dump memory(0x%x ~ 0x%x) to dump.bin\n", $start_addr, $end_addr
        eval "dump binary memory dump.bin %d %d", $start_addr, $end_addr
    end
end

define file_to_memory
    if $argc != 2
        echo file_to_memory [start_addr] [length]\n
    else
        set $start_addr = $arg0
        set $end_addr   = $arg0 + $arg1
        printf "Load load.bin to memory(0x%x ~ 0x%x)\n", $start_addr, $end_addr
        eval "restore load.bin binary %d 0 %d", $start_addr, $arg1
    end
end


define dump_bin
    if $argc != 3
        echo dump_bin [filename] [start_addr] [length]\n
    else
        set $start_addr = $arg1
        set $end_addr   = $arg1 + $arg2
        printf "dump memory(0x%x ~ 0x%x) to %s\n", $start_addr, $end_addr, $arg0
        eval "dump binary memory %s 0x%x 0x%x", $arg0, $start_addr, $end_addr 
    end
end


define restore_bin
    if $argc != 3
        echo restore [filename] [start_addr] [length]\n
    else
        set $filename = $arg0
        set $start_addr = $arg1
        set $length = $arg2
        printf "restore file(%s) to (0x%x ~ 0x%x)\n", $filename, $start_addr, $start_addr+$length
        eval "restore %s binary 0x%x 0 0x%x", $filename, $start_addr, $length
    end
end

define print-list-4
    if $argc != 4
        echo print-list-4 [LIST] [COUNT] [NOTE_TYPE] [NEXT_FIELD]\n
    else 
        set $list = ($arg2*)$arg0
        set $count = $arg1
        while $list
            if $count <= 0
                loop_break
            end
            print *$list
            set $list = $list->$arg3
            set $count = $count - 1
        end
    end
end

macro define offsetof(t, f) &((t *) 0)->f

define print-list-linux
    if $argc != 3
        echo print-list-linux [LIST] [COUNT] [CONTAINER_TYPE]\n
    else 
        set $list = (list_head*)$arg0
        set $pre_list = $list
        set $count = $arg1
        while $list
            if $count <= 0
                loop_break
            end
            set $list = $list->next
            if $list == $pre_list
                printf "!!! Circular list, reach the same node again !!!\n"
                loop_break
            end
            #set $container = ($arg2*)((unsigned int)$list - (unsigned int)offsetof($arg2, list))
            set $container = ($arg2*)((char *)$list - (char *)offsetof($arg2, list))
            p/x *$container
            set $count = $count - 1
        end
    end
end

