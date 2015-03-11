/***********/
/* Kernel. */
#include <std_headers.h>

/** Drivers. */
#include <hw/device.h>
#include <interrupt/interrupt.h>
#include <keyboard/keyboard.h>
#include <hw/pci_devices.h>
#include <hw/pci.h>
#include <mem/mem.h>
#include <cli/cli.h>

#include <disk/disk.h>

/** Applications and utilities. */

void halt ()
{
    kerror ("halted");
    while (1) {}
}

void default_interrupt_handler (int interruptNumber, uint32 error_code)
{
    KString message ("interrupt: unhandled interrupt ");
    message.append_hex ( (uint32_t) interruptNumber);
    kdebug("err code: "); 
    message.append_hex ( (uint32_t) error_code);
    kerror (message);
    halt();
}

void interrupt_generic (int, uint32)
{
    vga.write (1, 15, "Interrupt Handler Called");
}

void interrupt_system_timer (int, uint32)
{
    timer.timer++;
    Interrupt8259PIC::send_eoi (Interrupt8259PIC::IRQ::SYSTEM_TIMER);
}

void setup_interrupts()
{
    Interrupt8259PIC::remap_interrupts();

    interrupt_driver.set_handler ( (int) Interrupt8259PIC::Interrupt::SYSTEM_TIMER, interrupt_system_timer);
    interrupt_driver.set_handler ( (int) Interrupt8259PIC::Interrupt::ATA1, interrupt_generic);

    Interrupt8259PIC::enable_all();
    InterruptDriver::enable_hardware_interrupts();
}

/**
 * To construct the kernel we build some basic systems in the following order: 
 * Global objects:
 * - construct vga text mode driver
 * - construct interrupt handler
 * - add the timer driver
 * - enable interrupts
 * - construct physical memory manager
 * - construct kernel heap
 *
 * Then we call the Kernel's constructor which now has the tools it needs to 
 * register device drivers and display diagnostic messages as required. 
 */ 

VgaDriver                   vga; 
InterruptDriver             interrupt_driver (default_interrupt_handler);
Timer                       timer;
PhysicalMemoryManager       pmem; 
VirtualMemoryManager        vmem; // this starts paging up.
MemoryPool<MemoryRegion>    kheap(*vmem.get_kernel_heap_region());

/* Things that require the heap to be available can now be started. */
DeviceManager               device_manager;


class Kernel
{
public:
    /** Construct the kernel. */
    Kernel()
        :
        ata_drive_kern (ata_controller, 0),
        ata_drive_data (ata_controller, 1),
        fat_fs2 (ata_drive_data, 2, 100)
    { 
        pci.search_for_devices();
        device_manager.register_device(new KeyboardHardwareDriver()); 
    }

    void run()
    {
        kdebug("Launching CLI.");
        cli_register_command(&pmem); 
        cli_register_command(&vmem); 
        cli_main(); 
    }


private:
//    EventWatcher event_watcher (event_queue, vga);
    PciBus pci;
    ATAControllerPIO ata_controller;
    ATADrive ata_drive_kern;
    ATADrive ata_drive_data;
    Fat16 fat_fs2;
};

int main()
{
    kdebug ("Welcome to the kernel");

    setup_interrupts(); 
    //test_memory_pool();

    Kernel kernel;

    kernel.run();


//    /** Read the root directory of the file system. */
 //   Fat16::Directory root_dir (fat_fs2);
  //  Fat16::DirectoryEntry entry;

/*
    while (root_dir.next (&entry) == DiskResultCode::SUCCESS)
    {
        KString name (entry.name, 8);
        kdebug (KString ("name:") + name + KString ("<:"));
    }

    kdebug ("done.");
    */

    return 0;
}



