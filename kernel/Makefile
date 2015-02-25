include Makefile.inc

all: kernel.o kernel_asm.o interrupt.o disk net

clean:
	rm -r *.o

kernel.o: $(wildcard *.cpp) $(wildcard *.h) $(wildcard disk/*.h)
	$(CXX) $(CXXFLAGS) -c kernel.cpp -o kernel.o

kernel_asm.o: kernel_asm.asm
	nasm -f elf -o kernel_asm.o kernel_asm.asm

interrupt.o: interrupt.asm
	nasm -f elf -o interrupt.o interrupt.asm

.PHONY: disk
disk: 
	$(MAKE) -C ./disk

.PHONY: net
net:
	$(MAKE) -C ./net
