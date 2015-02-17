/* Kernel. */



void memset(char* dest, int ch, size_t count)
{
    while (count--)
        *dest++ = ch;
}

class InterruptDriver
{
public:
    InterruptDriver()
    {
        memset((char*)idt, 0, sizeof(idt));
        
        asm volatile ( "lidt (%0)" : : "p"(&idtr) );
    }
    void setHandler(int i, void (*handler)(void))
    {
        idt[i].baseLow  = (ptrdiff_t) handler & 0xFFFF;
        idt[i].baseHi   = (ptrdiff_t) handler & 0x00FF000 >> 16;
        idt[i].segment  = 0x10;
        idt[i].flags    = 0x8;
    }
private:
    struct Interrupt
    {
        uint16_t baseLow;
        uint8_t segment;
        uint8_t _reserved;
        uint8_t flags;
        uint8_t baseHi;
    } F_PACKED_STRUCT;
    
    typedef void (*Handler)(void);
    
    
    Handler handler[256];
    Interrupt idt[256]; /* Align to 8 bytes. */
    
    struct InterruptPointer
    {
        uint16_t length;
        uint32_t base;
        /* 48 bits. */
    } F_PACKED_STRUCT;
    
    InterruptPointer idtr;
};

class KeyboardDriver
{
public:
    KeyboardDriver()
    {}
    
private:
    char buffer[16];
};

class System
{
    void delay(int ms)
    {
        
    }
};

int main()
{
    VgaDriver vga;
    vga.write(1, 10, "Welcome to the kernel");
    
    for (int i = 0; i < 10; ++i)
        vga.write("Some string..");
    
    while (1)
    {}
}
