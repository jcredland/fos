/* Kernel. */

VgaDriver vga;

void default_interrupt_handler(int interruptNumber)
{
    KernelString message = KernelString("Unhandled interrupt ");
    message.appendHex(interruptNumber);
    vga.write(1, 16, message.get());
}

InterruptDriver interruptDriver(default_interrupt_handler); 

class KeyboardDriver
{
public:
    KeyboardDriver()
    {
        buffer[0] = 0; 
    }
    
private:
    char buffer[16];
};

KeyboardDriver keyboardDriver;

class System
{
    void delay(int ms)
    {
        
    }
};

extern "C"
{
    extern void interrupt_handler_wrap_noerr_49();
}

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

void interrupt_keyboard(int)
{
    vga.write("Key ");
    KernelString s;
    s.appendHex(inb(0x60));
    vga.write(s.get());
    Interrupt8259PIC::sendEOI(Interrupt8259PIC::IRQ::KEYBOARD); 
}

void setupInterrupts()
{
    Interrupt8259PIC::remapInterrupts();

    interruptDriver.setHandler((int) Interrupt8259PIC::Interrupt::SYSTEM_TIMER, interrupt_system_timer);
    interruptDriver.setHandler((int) Interrupt8259PIC::Interrupt::KEYBOARD, interrupt_keyboard);

    Interrupt8259PIC::enableAll(); 
    InterruptDriver::enableHardwareInterrupts();
}

int main()
{
    vga.write(1, 10, "Welcome to the kernel");

    setupInterrupts(); 

    while (1)
    {}
}


