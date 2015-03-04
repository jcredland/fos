
#include "kernel.h"

/* This file lets us control the initalization order of global objects. */
#include "interrupt.cpp"
//#include "events.cpp"
#include "vga.cpp"
#include "memory_physical.cpp"
#include "keyboard.cpp"
#include "pci_devices.cpp"
#include "cpplib.cpp"
#include "device.cpp"
#include "main.cpp"

