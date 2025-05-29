define connect
target remote localhost:2331
end
document connect
Connect to JLInkGDBServer.
end


define r4
printf "0x%X\n", *(unsigned int*)($arg0)
end
document r4
Read 32bits data.
Usage: r4 addr.
end




define w4
set (*(unsigned int*)($arg0))=($arg1)
end
document w4
Write 32bits data.
Usage: w4 addr value.
end


define dump_debug 
dump binary memory $arg0 0x60500000 0x60500000+$arg1
end


#connect

#load bin/isp_firmware.out
#symbol-file bin/isp_firmware.out

