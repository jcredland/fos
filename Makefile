
all:	fos.bin

fos.bin: fos.asm fos.ld kernel/main.o
	nasm -g -f elf -o fos.elf fos.asm
	i686-elf-ld -T fos.ld fos.elf kernel/main.o --oformat binary -o fos.bin
#	i686-elf-ld -Ttext 0x7c00 fos.elf --oformat binary -o fos.bin
#;	gobjcopy -O binary fos fos.bin

