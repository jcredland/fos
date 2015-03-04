/***********/
/* Kernel. */

void halt ()
{
    kerror ("halted");
    while (1) {}
}

void default_interrupt_handler (int interruptNumber)
{
    KString message ("interrupt: unhandled interrupt ");
    message.append_hex ( (uint32_t) interruptNumber);
    kerror (message);
    halt();
}


void interrupt_generic (int)
{
    vga.write (1, 15, "Interrupt Handler Called");
}

void interrupt_system_timer (int)
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

VgaDriver               vga; 
InterruptDriver         interrupt_driver (default_interrupt_handler);
Timer                   timer;
PhysicalMemoryManager   pmem; 
MemoryPool<64>          kheap;
/* Things that require the heap to be available can now be started. */
DeviceManager           device_manager;


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
        device_manager.register_device(new Keyboard()); 
    }

    void run()
    {
        kdebug("Entering main loop.");
        while (1) {}
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
    test_memory_pool();

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



