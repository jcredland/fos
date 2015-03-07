all: uk.h

keyboard_data_gen: keyboard_data_gen.cpp
	g++ -g -lc++ -std=c++11 -o keyboard_data_gen keyboard_data_gen.cpp

uk.h: keyboard_data_gen uk.keyscript
	./keyboard_data_gen uk.keyscript uk.h
