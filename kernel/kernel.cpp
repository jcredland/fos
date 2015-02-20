
#include "kernel.h"

/* This file lets us control the initalization order of global objects. */
#include "interrupt.cpp"
#include "events.cpp"
#include "vga.cpp"
#include "keyboard.cpp"
#include "main.cpp"

