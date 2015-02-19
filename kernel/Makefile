
CC = i686-elf-gcc
CXX = i686-elf-g++
CXXFLAGS = -g -O0 -ffreestanding -std=c++11

all: kernel.o kernel_asm.o interrupt.o

kernel.o: $(wildcard *.cpp) $(wildcard *.h)
	$(CXX) $(CXXFLAGS) -c kernel.cpp -o kernel.o

kernel_asm.o: kernel_asm.asm
	nasm -f elf -o kernel_asm.o kernel_asm.asm

interrupt.o: interrupt.asm
	nasm -f elf -o interrupt.o interrupt.asm

