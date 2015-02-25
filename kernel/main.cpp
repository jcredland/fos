/* Kernel. */

void default_interrupt_handler(int interruptNumber)
{
    KString message = KString("Unhandled interrupt ");
    message.appendHex((uint32_t) interruptNumber);
    vga.writeln(message);
}

InterruptDriver interruptDriver(default_interrupt_handler); 

void interrupt_generic(int)
{
    vga.write(1, 15, "Interrupt Handler Called");
}

class Timer 
{
public:
    uint32_t timer; 
} timer;

void interrupt_ata(int)
{
    vga.write(1, 15, "ATA Interrupt Handler Called");
    Interrupt8259PIC::sendEOI(Interrupt8259PIC::IRQ::ATA1); 
}

void interrupt_system_timer(int)
{
    timer.timer++;
    Interrupt8259PIC::sendEOI(Interrupt8259PIC::IRQ::SYSTEM_TIMER); 
}

void setupInterrupts()
{
    Interrupt8259PIC::remapInterrupts();

    interruptDriver.setHandler((int) Interrupt8259PIC::Interrupt::SYSTEM_TIMER, interrupt_system_timer);
    interruptDriver.setHandler((int) Interrupt8259PIC::Interrupt::KEYBOARD, interrupt_keyboard);
    interruptDriver.setHandler((int) Interrupt8259PIC::Interrupt::ATA1, interrupt_ata);

    Interrupt8259PIC::enableAll(); 
    InterruptDriver::enable_hardware_interrupts();
}

int main()
{
    vga.set_mode(0x17); 
    vga.write(1, 6, "Welcome to the kernel");
    vga.set_pos(0, 7); 

    setupInterrupts(); 

    EventWatcher event_watcher(event_queue, vga); 

    pmem.print_debug(vga); 

    PciBus pci;

    pci.search_for_devices();

    AtaPioDriver ata_pio_driver;

    while (1)
    {
        event_watcher.poll(); 
    }
}


