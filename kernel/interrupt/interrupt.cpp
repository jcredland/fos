
#include <interrupt/interrupt.h>

/*** CPU TRAPS ***/
const char * interrupt_cpu_trap_names[19] = 
{
    "Division by zero",
    "Debugger",
    "NMI",
    "Breakpoint",
    "Overflow",
    "Bounds"
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

    KString error_message(interrupt_cpu_trap_names[trap_number]); 

    kerror(error_message); 

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

/*
 * This function reaches for our assembly interrupt handler stubs and 
 * populates the CPUs interrupt table with those addresses. 
 */
void InterruptDriver::register_all_handlers()
{
    ptrdiff_t size = &interrupt_stub_1 - &interrupt_stub_0;
    char * h = &interrupt_stub_0;
    for (int i = 0; i < MAX_INTERRUPT_VECTORS; ++i)
    {
        set_vector_stub(i, h);
        h += size;
    }
}



extern "C"
{
/** This function is called by the interrupt handler stub when an interrupt occurs. 
 * It may or may not include the error code. If there is no error code then errorCode will
 * be set to zero. 
 */
void interrupt_handler(uint8_t interrupt_num, uint16_t error_code)
{
    interrupt_driver.call_handler(interrupt_num, error_code);
}
}

