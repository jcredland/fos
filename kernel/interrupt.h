
/** Raise an interrupt manually.  Useful for testing. */
template <int N> static inline void raiseInterrupt (void)
{
    asm volatile ("int %0\n" : : "N"(N));
}

class InterruptDriver
{
public:
    InterruptDriver(void(*default_handler)(int));
    
    enum
    {
        GATE_TYPE_TRAP_16BIT = 0x7,
        INTERRUPT_TYPE_TRAP_32BIT = 0xE,
        GATE_TYPE_TRAP_32BIT = 0xF,
        INTERRUPT_ENABLED = 0x80
    };

    enum
    { 
        MAX_INTERRUPT_VECTORS = 256
    };

    void set_handler(int interruptNumber, void(*newHandler)(int))
    {
        handler[interruptNumber] = newHandler;
    }

    static void enable_hardware_interrupts()
    {
        asm volatile ( "sti" );
    }

    static void disable_hardware_interrupts()
    {
        asm volatile ( "cli" );
    }

    void call_handler(uint8_t interruptNumber)
    {
        (*handler[interruptNumber])(interruptNumber);
    }
    
private:
    void register_all_handlers();
    /** Sets up an entry in the IDT. */
    void set_vector_stub(uint8_t interruptNumber, char *asmInterruptHandler) 
    {
        auto & i = interruptNumber;
        kassert(i > 0 && i < 256);
        
        idt[i].baseLow  = (ptrdiff_t) asmInterruptHandler & 0xFFFF;
        idt[i].baseHi   = ((ptrdiff_t) asmInterruptHandler >> 16) & 0xFF;
        idt[i].segment  = 0x08;
        idt[i].flags    = INTERRUPT_TYPE_TRAP_32BIT | INTERRUPT_ENABLED;
        idt[i]._reserved = 0;
    }

    struct F_PACKED Interrupt
    {
        uint16_t baseLow;
        uint16_t segment;
        uint8_t _reserved;
        uint8_t flags;
        uint16_t baseHi;
    };
    
    typedef void (*Handler)(int);
    
    Handler handler[256];
    Interrupt idt[256]; /* Align to 8 bytes. */
    
    struct F_PACKED InterruptPointer
    {
        uint16_t length;
        uint32_t base;
        /* 48 bits. */
    };
    
    InterruptPointer idtr;
};

extern InterruptDriver interrupt_driver;

extern "C"
{
/** This function is called by the interrupt handler stub when an interrupt occurs. 
 * It may or may not include the error code. If there is no error code then errorCode will
 * be set to zero. 
 */
void interrupt_handler(uint8_t interruptNumber, uint16_t errorCode)
{
    interrupt_driver.call_handler(interruptNumber);
}
}

