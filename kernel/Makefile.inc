INCLUDE_DIR = -I.. -I.
CC = i686-elf-gcc
CXX = i686-elf-g++
CXXFLAGS = -g -O0 -fno-rtti -ffreestanding -std=c++11  -fdiagnostics-color=auto -fno-exceptions -Wall -Wextra $(INCLUDE_DIR)

