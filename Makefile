
all: os-image.bin

os-image.bin: boot/boot_sect.bin
	cat $^ > $@

run: all
	qemu-system-x86_64 -fda os-image.bin

%.o : %.c
	${CC} ${CFLAGS} -c $< -o $@

%.o : %.asm
	nasm $< -f elf -o $@

%.bin: %.asm
	nasm $< -f bin -o $@

clean:
	rm -rf os-image.bin boot/*.o boot/*.bin
