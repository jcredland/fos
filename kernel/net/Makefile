
CC = i686-elf-gcc
CXX = i686-elf-g++
CXXFLAGS = -g -O0 -ffreestanding -std=c++11

all: net.o

net.o: $(wildcard *.cpp) $(wildcard *.h)
	$(CXX) $(CXXFLAGS) -c net.cpp -o net.o

