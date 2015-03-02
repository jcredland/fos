/* Kernel. */

Timer timer; 

void halt ()
{
    kerror("halted"); 
    while (1) {}
}

void default_interrupt_handler(int interruptNumber)
{
    KString message ("interrupt: unhandled interrupt ");
    message.append_hex((uint32_t) interruptNumber);
    kerror(message);
    halt();
}

InterruptDriver interrupt_driver(default_interrupt_handler); 

void interrupt_generic(int)
{
    vga.write(1, 15, "Interrupt Handler Called");
}

void interrupt_ata(int)
{
    static int notification = 0; 
    if (notification == 0)
        kdebug("interrupt: ata interrupt handler called");
    notification = 1;
    Interrupt8259PIC::send_eoi(Interrupt8259PIC::IRQ::ATA1); 
}

void interrupt_system_timer(int)
{
    timer.timer++;
    Interrupt8259PIC::send_eoi(Interrupt8259PIC::IRQ::SYSTEM_TIMER); 
}

void setupInterrupts()
{
    Interrupt8259PIC::remap_interrupts();

    interrupt_driver.set_handler((int) Interrupt8259PIC::Interrupt::SYSTEM_TIMER, interrupt_system_timer);
    interrupt_driver.set_handler((int) Interrupt8259PIC::Interrupt::KEYBOARD, interrupt_keyboard);
    interrupt_driver.set_handler((int) Interrupt8259PIC::Interrupt::ATA1, interrupt_ata);

    Interrupt8259PIC::enable_all(); 
    InterruptDriver::enable_hardware_interrupts();
}

int main()
{
    kdebug("Welcome to the kernel"); 

    setupInterrupts(); 

    EventWatcher event_watcher(event_queue, vga); 

    pmem.print_debug(vga); 

    PciBus pci;
    pci.search_for_devices();

    /** Now we setup the disk controller and two disks. */
    ATAControllerPIO ata_controller;

    ATADrive ata_drive_kern(ata_controller, 0); 
    ATADrive ata_drive_data(ata_controller, 1); 

    /** And attach a file system to our second disk. */
    Fat16 fat_fs2(ata_drive_data, 2, 100);

    /** Read the root directory of the file system. */
    Fat16::Directory root_dir(fat_fs2); 
    Fat16::DirectoryEntry entry;

    kdebug("Displaying root directory"); 

    while (root_dir.next(&entry) == DiskResultCode::SUCCESS)
    {
        KString name(entry.name, 8); 
        kdebug(KString("name:") + name + KString("<:")); 
    }
    kdebug("done.");

    test_memory_pool(); 

    while (1)
    {
        event_watcher.poll(); 
    }
}


