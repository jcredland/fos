all: run

fs_test: fs_test.cpp ../fat16.h ../fat16.cpp
	g++ -g -std=c++11 -o fs_test fs_test.cpp ../../_test/vga_mock.cpp 

run: fs_test
	./fs_test ../../../disk2.img

