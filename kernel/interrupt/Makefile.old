include ../Makefile.inc

all: disk.o

disk.o: $(wildcard *.cpp) $(wildcard *.h)
	$(CXX) $(CXXFLAGS) -c disk.cpp -o disk.o

.PHONY: clean
clean:
	rm *.o

