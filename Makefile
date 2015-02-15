
all:	fos.bin 

fos.bin: fos.asm fos.ld kernel
	nasm -g -f elf -o fos.elf fos.asm
	i686-elf-ld -T fos.ld fos.elf kernel/main.o --oformat binary -o fos.bin

KERNEL_DIR = kernel
.PHONY: kernel
kernel:
	       $(MAKE) -C $(KERNEL_DIR)
