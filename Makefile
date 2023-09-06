C_SOURCES = $(wildcard kernel/*.c driver/*.c interrupt/*.c)

OBJ = ${C_SOURCES:.c=.o} 

CFLAGS= -g -fno-builtin -fno-stack-protector #  -nostdinc

all: os-image.bin

os-image.bin: boot/boot_sect.bin kernel.bin
	cat $^ > $@
 
kernel.bin: boot/boot_kernel.o interrupt/gdt_load.o interrupt/idt_load.o ${OBJ}
	ld -Ttext 0x1000 $^ -o $@ --oformat binary

kernel.elf: boot/boot_kernel.o interrupt/gdt_load.o interrupt/idt_load.o ${OBJ}
	ld -Ttext 0x1000 $^ -o $@

run: all
	qemu-system-x86_64 -fda os-image.bin # -d guest_errors,int

debug: os-image.bin kernel.elf
	qemu-system-x86_64 -s -S -fda os-image.bin -d guest_errors,int &
	gdb

%.o : %.c
	gcc ${CFLAGS} -c $< -o $@

%.o : %.asm
	nasm $< -f elf64 -o $@

%.bin: %.asm
	nasm $< -f bin -o $@

clean:
	rm -rf os-image.bin kernel.bin kernel.elf boot/*.o boot/*.bin kernel/*.o driver/*.o interrupt/*.o
