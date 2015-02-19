/* Kernel. */

VgaDriver vga;
InterruptDriver interruptDriver; 


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

void interrupt_generic()
{
    vga.write(1, 15, "Interrupt Handler Called");
}

int main()
{

    vga.write(1, 10, "Welcome to the kernel");

    interruptDriver.setHandler(49, interrupt_handler_wrap_noerr_49, interrupt_generic);
    interruptDriver.raiseInterrupt<49>();
    
    while (1)
    {}
}
