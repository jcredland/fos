
#include <interrupt/interrupt.h>
#include <interrupt/stack_frame.h>

/*** CPU TRAPS ***/
const char * interrupt_cpu_trap_names[] = 
{
    "Division by zero",
    "Debugger",
    "NMI",
    "Breakpoint",
    "Overflow",
    "Bounds",
    "Invalid Opcode",
    "Coprocessor not available",
    "Double fault",
    "Coprocessor Segment Overrun (386 or earlier only)",
    "Invalid Task State Segment",
    "Segment not present",
    "Stack Fault",
    "General protection fault",
    "Page fault",
    "reserved",
    "Math Fault",
    "Alignment Check",
    "Machine Check",
    "SIMD Floating-Point Exception"
};

void interrupt_cpu_trap_handler(int trap_number, uint32 error_code)
{
    kerror("cpu: trap.  cpu halted. trap_number=" + KString( (uint16) trap_number)); 
    kerror("error code: " + KString(error_code)); 

    kerror("****** BLUE SCREEN OF DEAF! ****"); 
    KString error_message(interrupt_cpu_trap_names[trap_number]); 

    kerror(error_message); 
    if (trap_number == 0xE)
    {
        uint32 addr; 
        asm volatile ("mov %%cr2, %0" 
                        :"=r" (addr)
                        :
                        ); 

        kerror("Virtual Memeory Address: " + KString(addr)); 
    }

    while (1) {}
}

InterruptDriver::InterruptDriver(void(*default_handler)(int, uint32))
{
    memset((char*)idt, 0, sizeof(idt));

    register_all_handlers();

    for (unsigned i = 0; i < MAX_INTERRUPT_VECTORS; ++i)
    {
        if (i < sizeof(interrupt_cpu_trap_names))
            handler[i] = interrupt_cpu_trap_handler; 
        else
            handler[i] = default_handler;

        device_handlers[i] = nullptr;
    }

    idtr.base = (uint32_t) idt;
    idtr.length = sizeof(idt) - 1;

    asm volatile ( "lidt (%0)" : : "p"(&idtr) );
}

extern char interrupt_stub_1; 
extern char interrupt_stub_0;
extern uintptr_t interrupt_stub_vectors;

/*
 * This function reaches for our assembly interrupt handler stubs and 
 * populates the CPUs interrupt table with those addresses. 
 */
void InterruptDriver::register_all_handlers()
{
    uintptr_t * vectors = &interrupt_stub_vectors;
    for (int i = 0; i < MAX_INTERRUPT_VECTORS; ++i)
        set_vector_stub(i, (char*) *vectors++);
}



extern "C"
{
/** This function is called by the interrupt handler stub when an interrupt occurs. 
 * It may or may not include the error code. If there is no error code then errorCode will
 * be set to zero. 
 */
void interrupt_handler(SavedProcessContext * isf)
{
    interrupt_driver.call_handler(isf->interrupt_number, isf->error_code);
}
}

