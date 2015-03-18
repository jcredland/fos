/***********/
/* Kernel. */
#include <std_headers.h>

/** Drivers. */
#include <hw/device.h>
#include <interrupt/interrupt.h>
#include <keyboard/keyboard.h>
#include <hw/pci_devices.h>
#include <hw/pci.h>
#include <hw/timer.h>
#include <proc/process.h>
#include <proc/proc_man.h>
#include <mem/mem.h>
#include <cli/cli.h>

#include <disk/disk.h>

/** It's important to get ready for interrupts and kernel heap allocation before
 * loading drivers that depend on these facilities. */

VgaDriver                   vga; 
InterruptDriver             interrupt_driver;
PhysicalMemoryManager       pmem; 
VirtualMemoryManager        vmem; // this starts paging up.
MemoryPool<MemoryRegion>    kheap(*vmem.get_kernel_heap_region());

/* Things that require the heap to be available can now be started. */
DeviceManager               device_manager;
Timer                       timer;
ProcessManager              procman;

void proc_counter()
{
    uint8 counter = 0; 
    while (1)
    {
        timer.delay_ms(500); 
        counter++;
        vga.write(73, 2, kstring(counter).get());
    }
}

void proc_cli()
{
    procman.launch_kthread(proc_counter, "counter"); 
    cli_register_command(&pmem); 
    cli_register_command(&vmem); 
    cli_register_command(&kheap); 
    cli_register_command(&procman); 
    cli_main(); 
}

/*
class Kernel
{
public:
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
        cli_register_command(&kheap); 
        cli_main(); 
    }


private:
    PciBus pci;
    ATAControllerPIO ata_controller;
    ATADrive ata_drive_kern;
    ATADrive ata_drive_data;
    Fat16 fat_fs2;
};
*/
int main()
{
    device_manager.register_device(new KeyboardHardwareDriver()); 

    Interrupt8259PIC::remap_interrupts();
    Interrupt8259PIC::enable_all();
    interrupt_driver.enable_hardware_interrupts(); 

    procman.setup_initial_kernel_thread(proc_cli); 

    kdebug("start scheduler...");     
    procman.start_scheduler(); // will never return.

    return 0;
}

