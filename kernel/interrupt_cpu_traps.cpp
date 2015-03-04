
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
    "Page fault"",
    "reserved",
    "Math Fault",
    "Alignment Check",
    "Machine Check",
    "SIMD Floating-Point Exception"
};

void interrupt_cpu_trap(int trap_number)
{
    kerror("cpu: trap.  cpu halted. trap_number=" + KString(trap_number)); 

    KString error_message(interrupt_cpu_trap_names(trap_number)); 

    kerror(error_message); 

    while (1) {}
}
