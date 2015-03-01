
#include "kernel.h"

/* This file lets us control the initalization order of global objects. */
#include "interrupt.cpp"
#include "events.cpp"
#include "vga.cpp"
#include "memory_physical.cpp"
#include "memory_kmalloc.cpp"
#include "keyboard.cpp"
#include "pci_devices.cpp"
#include "main.cpp"

extern "C" void __cxa_pure_virtual()
{
    klog(KL_ERROR, "Pure virtual function call"); 
}
