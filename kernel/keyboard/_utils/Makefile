all: lang_uk.cpp mac_scancodes.cpp

keyboard_data_gen: keyboard_data_gen.cpp
	g++ -Wall -Wextra -g -lc++ -std=c++11 -o keyboard_data_gen keyboard_data_gen.cpp

lang_uk.cpp: keyboard_data_gen uk.keyscript
	./keyboard_data_gen -l uk.keyscript lang_uk.cpp

mac_scancodes.cpp: keyboard_data_gen mac.scancodes
	./keyboard_data_gen -s mac.scancodes mac_scancodes.cpp
