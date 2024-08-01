TOOLHOME = /opt/riscv-elf/bin

all: monitor_all monitorx86
#monitor1 monitor2 hello test datagen myprintx86 monitor_mainx86 # hello32

myprint.o: myprint.c
	$(TOOLHOME)/riscv64-unknown-elf-gcc -c -o myprint.o myprint.c
	$(TOOLHOME)/riscv64-unknown-elf-objdump -D myprint.o > myprint.dis

boot.o: boot.s 
	$(TOOLHOME)/riscv64-unknown-elf-as -o boot.o boot.s

monitor.o: monitor.s
	$(TOOLHOME)/riscv64-unknown-elf-as -o monitor.o monitor.s

monitor_all: monitor.o linkn0.lds monitor_main.o myprint.o
	$(TOOLHOME)/riscv64-unknown-elf-ld -T linkn0.lds -o monitor_all monitor.o monitor_main.o myprint.o
	$(TOOLHOME)/riscv64-unknown-elf-objcopy --gap-fill=0x00 -O binary monitor_all monitor_all.bin
	$(TOOLHOME)/riscv64-unknown-elf-objdump -D monitor_all > monitor_all.dis
	hexdump monitor_all.bin  > monitor_all.bin.dis

monitor_main.o: monitor_main.c taskletconfig.h
	$(TOOLHOME)/riscv64-unknown-elf-gcc -c -o monitor_main.o monitor_main.c
	$(TOOLHOME)/riscv64-unknown-elf-objdump -D monitor_main.o > monitor_main.dis

monitorx86: monitor_main.c taskletconfig.h
	gcc -DINTEL monitor_main.c -o monitorx86

clean:
	rm *.o *.dis monitorx86 monitor_all *.bin
