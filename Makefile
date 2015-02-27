KERNEL_OBJECTS=kernel/kernel.o kernel/interrupt.o kernel/kernel_asm.o kernel/net/net.o kernel/disk/disk.o

all:	fos.elf fos.sym fos.bin

clean:
	rm fos.elf fos.bin
	rm -r kernel/*.o
	rm -r kernel/*/*.o

fos.elf: bootloader.asm fos.ld kernel
	nasm -g -f elf -o bootloader.elf bootloader.asm
	i686-elf-ld -T fos.ld bootloader.elf $(KERNEL_OBJECTS) --oformat elf32-i386 -o fos.elf

fos.sym: fos.elf
	i686-elf-objcopy --only-keep-debug fos.elf fos.sym

fos.bin: fos.elf
	i686-elf-objcopy -O binary fos.elf fos.bin
		

KERNEL_DIR = kernel
.PHONY: kernel
kernel:
	       $(MAKE) -C $(KERNEL_DIR)

