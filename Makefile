C_SOURCES = $(wildcard kernel/*.c driver/*.c)

OBJ = ${C_SOURCES:.c=.o} 

all: os-image.bin

os-image.bin: boot/boot_sect.bin kernel.bin
	cat $^ > $@

kernel.bin: boot/boot_kernel.o ${OBJ}
	ld -Ttext 0x1000 $^ -o $@ --oformat binary

run: all
	qemu-system-x86_64 -fda os-image.bin

%.o : %.c
	gcc -c $< -o $@

%.o : %.asm
	nasm $< -f elf64 -o $@

%.bin: %.asm
	nasm $< -f bin -o $@

clean:
	rm -rf os-image.bin kernel.bin boot/*.o boot/*.bin kernel/*.o driver/*.o
