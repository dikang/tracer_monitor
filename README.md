# tracer_monitor

'monitor_all': Elf file of the interrupt service routine and monitor program for all CAEs.

'tasklet_all': Elf file of Master program and tasklets 

'memory_map': x86 binary code which shows the memory map of monitor and tasklets

## Memory map
Memory map is printed using 'memory_map' code which is a x86 binary.

'''
$ ./memory_map
---- Monitor ----
CODE_ADDR(0) = 0x80000000
CONFIG_ADDR(0) = 0x80400000
GP_ADDR(0) = 0x80500000
TP_ADDR(0) = 0x80580000
SP_ADDR(0) = 0x80800000
CODE_ADDR(1) = 0x80000000
CONFIG_ADDR(1) = 0x80800000
GP_ADDR(1) = 0x80900000
TP_ADDR(1) = 0x80980000
SP_ADDR(1) = 0x80c00000
CODE_ADDR(2) = 0x80000000
CONFIG_ADDR(2) = 0x80c00000
GP_ADDR(2) = 0x80d00000
TP_ADDR(2) = 0x80d80000
SP_ADDR(2) = 0x81000000

---- Interrupt Msg Queues ----
INT_MSGQ_ADDR(0) = 0x803f0000
INT_MSGQ_ADDR(1) = 0x803f07f8
INT_MSGQ_ADDR(2) = 0x803f0ff0

---- Tasklet Master ----
CONFIG_MASTER_ADDR = 0x81000000
PC_MASTER_ADDR = 0x87000000
GP_MASTER_ADDR = 0x81100000
TP_MASTER_ADDR = 0x81180000
SP_MASTER_ADDR = 0x81400000

---- Tasklets ----
CONFIG_TASKLET_ADDR(0) = 0x81400000
TP_TASKLET_ADDR(0) = 0x81580000
SP_TASKLET_ADDR(0) = 0x81800000
CONFIG_TASKLET_ADDR(1) = 0x81800000
TP_TASKLET_ADDR(1) = 0x81980000
SP_TASKLET_ADDR(1) = 0x81c00000
CONFIG_TASKLET_ADDR(2) = 0x81c00000
TP_TASKLET_ADDR(2) = 0x81d80000
SP_TASKLET_ADDR(2) = 0x82000000
CONFIG_TASKLET_ADDR(3) = 0x82000000
TP_TASKLET_ADDR(3) = 0x82180000
SP_TASKLET_ADDR(3) = 0x82400000
'''

## How to run it (to test)

We used Qemu to run the code. As is shown in the command, monitor code is loaded at 0x80000000 while tasklet code is loaded at 0x81400000. The load address of monitor program is fixed when Qemu is used. You can change the load address of tasklet code, but you should update header files accordingly and rebuild the code.

'''
qemu-system-riscv64 -machine virt -smp cpus=3 -nographic \
	-kernel monitor_all.bin -device loader,file=monitor_all.bin,addr=0x80000000 \
	-kernel tasklet_all.bin -device loader,file=tasklet_all.bin,addr=0x81400000 \
	-bios none
'''
