
all:	fos.elf fos.sym fos.bin

fos.elf: bootloader.asm fos.ld kernel
	nasm -g -f elf -o bootloader.elf bootloader.asm
	i686-elf-ld -T fos.ld bootloader.elf kernel/kernel.o kernel/interrupt.o kernel/kernel_asm.o --oformat elf32-i386 -o fos.elf

fos.sym: fos.elf
	i686-elf-objcopy --only-keep-debug fos.elf fos.sym

fos.bin: fos.elf
	i686-elf-objcopy -O binary fos.elf fos.bin
		
#	i686-elf-g++ -T fos.ld -o fos.bin -ffreestanding -nostdlib -gsplit-dwarf kernel/kernel.o kernel/interrupt.o kernel/kernel_asm.o 

KERNEL_DIR = kernel
.PHONY: kernel
kernel:
	       $(MAKE) -C $(KERNEL_DIR)

	#i686-elf-ld -T fos.ld fos.elf kernel/kernel.o kernel/interrupt.o kernel/kernel_asm.o --oformat binary -o fos.bin
