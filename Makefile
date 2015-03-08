
#########################
# FOS is made of two disks at present: 
#########################

BOOT_DISKA = build/fos.bin
BOOT_DISKB = build/disk2.img

#########################

BOOT_OBJECT = bootloader.elf
KERNEL_OBJECTS = $(shell find kernel -name *.o) 

all:	kernel $(BOOT_DISKA) $(BOOT_DISKB)

$(BOOT_DISKA): build/fos.elf
	i686-elf-objcopy -O binary build/fos.elf build/fos.bin

build/fos.elf: $(KERNEL_OBJECTS) $(BOOT_OBJECT)
	i686-elf-ld -L build/ -T boot/fos.ld $(BOOT_OBJECT) $(KERNEL_OBJECTS) --oformat elf32-i386 -o build/fos.elf
	i686-elf-objcopy --only-keep-debug build/fos.elf build/fos.sym

# Build the boot strap stuff. 
$(BOOT_OBJECT): boot/bootloader.asm boot/fos.ld
	nasm -g -f elf -o $(BOOT_OBJECT) boot/bootloader.asm

# Build test disks.
build/disk2.img: boot/disk2.dmg
	hdiutil convert boot/disk2.dmg -format RdWr -o build/disk2.img

		
.PHONY: tests
tests:
	       $(MAKE) -C tests


.PHONY: kernel
kernel:
	       $(MAKE) -C kernel

.PHONY: clean
clean:
	rm $(KERNEL_OBJECTS) $(BOOT_OBJECT) 
