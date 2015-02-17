
CC = i686-elf-gcc
CXX = i686-elf-g++
CXXFLAGS = -ffreestanding -std=c++11

all: kernel.o

kernel.o: $(wildcard *.cpp) $(wildcard *.h)
	$(CXX) $(CXXFLAGS) -c kernel.cpp -o kernel.o

kernel_asm.o: kernel_asm.asm
	nasm -f elf -o kernel_asm.o kernel_asm.asm

