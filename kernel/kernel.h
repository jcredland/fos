#pragma once

#include "std_headers.h"


/** Drivers. */
#include "interrupt_pic.h"
#include "device.h"
#include "interrupt.h"
//#include "events.h"
#include "keyboard.h"
#include "pci_devices.h"
#include "pci.h"
#include "memory_physical.h"
#include "memory_kmalloc.h"

extern MemoryPool<64> kheap;


#include "disk/disk.h"

/** Applications and utilities. */

#include "keyboard_scancodes.h"
#include "keyboard_event_manager.h"
//#include "event_watcher.h"

