TOOLHOME = /opt/riscv-elf/bin
#DEBUG_FLAGS = -DDEBUG
DEBUG_FLAGS = 
CFLAGS = -fPIC 
all: monitor_all tasklet_all monitorx86 memory_map
#monitor1 monitor2 hello test datagen myprintx86 monitor_mainx86 # hello32

memory_map: memory_map.c tasklet_config.h
	gcc -o memory_map memory_map.c

myprint.o: myprint.c
	$(TOOLHOME)/riscv64-unknown-elf-gcc -c -o myprint.o myprint.c
	$(TOOLHOME)/riscv64-unknown-elf-objdump -D myprint.o > myprint.dis

tasklet_all: tasklet_main.o tasklet_config.s ascii_tbl.s hw_info.s myprint.o linkt.lds utility.o
	$(TOOLHOME)/riscv64-unknown-elf-ld -T linkt.lds -o tasklet_all tasklet_main.o myprint.o utility.o
	$(TOOLHOME)/riscv64-unknown-elf-objcopy --gap-fill=0x00 -O binary tasklet_all tasklet_all.bin
	$(TOOLHOME)/riscv64-unknown-elf-objdump -D tasklet_all > tasklet_all.dis
	hexdump tasklet_all.bin  > tasklet_all.bin.dis

tasklet_main.o: tasklet_main.c tasklet_config.h utility.h
	$(TOOLHOME)/riscv64-unknown-elf-gcc $(CFLAGS) $(DEBUG_FLAGS) -c -o tasklet_main.o tasklet_main.c
	$(TOOLHOME)/riscv64-unknown-elf-objdump -D tasklet_main.o > tasklet_main.dis

boot.o: boot.s 
	$(TOOLHOME)/riscv64-unknown-elf-as -o boot.o boot.s

utility.o: utility.c utility.h tasklet_config.h
	$(TOOLHOME)/riscv64-unknown-elf-gcc $(CFLAGS) $(DEBUG_FLAGS) -c -o utility.o utility.c

tasklet.o: tasklet.s ascii_tbl.s hw_info.s tasklet_config.s
	$(TOOLHOME)/riscv64-unknown-elf-as -o tasklet.o tasklet.s

monitor.o: monitor.s ascii_tbl.s hw_info.s tasklet_config.s
	$(TOOLHOME)/riscv64-unknown-elf-as -o monitor.o monitor.s

monitor_all: monitor.o tasklet.o linkn0.lds monitor_main.o myprint.o utility.o
	$(TOOLHOME)/riscv64-unknown-elf-ld -T linkn0.lds -o monitor_all monitor.o monitor_main.o myprint.o utility.o tasklet.o
	$(TOOLHOME)/riscv64-unknown-elf-objcopy --gap-fill=0x00 -O binary monitor_all monitor_all.bin
	$(TOOLHOME)/riscv64-unknown-elf-objdump -D monitor_all > monitor_all.dis
	hexdump monitor_all.bin  > monitor_all.bin.dis

monitor_main.o: monitor_main.c tasklet_config.h utility.h
	$(TOOLHOME)/riscv64-unknown-elf-gcc $(CFLAGS) $(DEBUG_FLAGS) -c -o monitor_main.o monitor_main.c
	$(TOOLHOME)/riscv64-unknown-elf-objdump -D monitor_main.o > monitor_main.dis

monitorx86: monitor_main.c tasklet_config.h
	gcc -DINTEL monitor_main.c -o monitorx86

clean:
	rm *.o *.dis monitorx86 monitor_all *.bin memory_map tasklet_all
