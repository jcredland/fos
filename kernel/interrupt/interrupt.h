#pragma once

#include <std_headers.h>
#include <hw/device.h>
#include <interrupt/interrupt_pic.h>

/** Raise an interrupt manually.  Useful for testing. */
template <int N> static inline void raiseInterrupt (void)
{
    asm volatile ("int %0\n" : : "N"(N));
}

/**
 * Interrupts are managed as follows: 
 *
 * - InterruptDriver registers the IDT in it's constructor.
 * - The IDT points to a short series of assembler stubs. 
 * - The assembler stubs call a CDECL function interrupt_handler(int, int) putting the appropriate 
 *   interrupt number and error code into the function parameters via the stack.
 * - The interrupt_handler looks up the InterruptDriver object and calls the call_handler method on it. 
 * - The call_handler looks up a table of registered interrupt handlers and hands off to the appropriate one. 
 */
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

    /** Adds a bare C function as an interrupt handler. */
    void set_handler(int interrupt_num, void(*new_handler)(int))
    {
        handler[interrupt_num] = new_handler;
    }
    
    /** Adds a DeviceInterruptHandler object, which overrides any bare C handlers
     * that may have been registered for this interrupt number. */
    void set_handler(int interrupt_num, DeviceInterruptHandler * device_handler)
    {
        device_handlers[interrupt_num] = device_handler;
    }

    static void enable_hardware_interrupts()
    {
        asm volatile ( "sti" );
    }

    static void disable_hardware_interrupts()
    {
        asm volatile ( "cli" );
    }

    void call_handler(uint8_t interrupt_num)
    {
        if (device_handlers[interrupt_num] != nullptr)
            device_handlers[interrupt_num]->handle_interrupt(interrupt_num); 
        else 
            (*handler[interrupt_num])(interrupt_num);
    }
    
private:
    void register_all_handlers();
    /** Sets up an entry in the IDT. */
    void set_vector_stub(uint8_t interrupt_num, char *asm_interrupt_handler) 
    {
        idt[interrupt_num].baseLow  =  (ptrdiff_t) asm_interrupt_handler & 0xFFFF;
        idt[interrupt_num].baseHi   = ((ptrdiff_t) asm_interrupt_handler >> 16) & 0xFF;
        idt[interrupt_num].segment  = 0x08;
        idt[interrupt_num].flags    = INTERRUPT_TYPE_TRAP_32BIT | INTERRUPT_ENABLED;
        idt[interrupt_num]._reserved = 0;
    }

    /**
     * Intel data structure.  The format of an entry in the interrupt descriptor table.
     */
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
    DeviceInterruptHandler * device_handlers[256];
    Interrupt idt[256]; /* Align to 8 bytes. */
   
   /* Intel data structure.  LIDT instruction points to this data structure in memory, which
    * in turn points to the Interrupt Descriptor Table itself. */ 
    struct F_PACKED InterruptPointer
    {
        uint16_t length;
        uint32_t base;
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
void interrupt_handler(uint8_t interrupt_num, uint16_t /* err_code */);
}

