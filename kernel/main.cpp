/* Kernel. */

void default_interrupt_handler(int interruptNumber)
{
    KernelString message = KernelString("Unhandled interrupt ");
    message.appendHex(interruptNumber);
    vga.write(1, 16, message.get());
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

    Interrupt8259PIC::enableAll(); 
    InterruptDriver::enable_hardware_interrupts();
}

int main()
{
    vga.write(1, 10, "Welcome to the kernel");
    KernelString s; 
    s.appendHex(0x12345678); 
    vga.write(s.get());

    setupInterrupts(); 

    EventWatcher event_watcher(event_queue, vga); 

    while (1)
    {
        event_watcher.poll(); 
    }
}


