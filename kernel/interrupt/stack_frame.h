#pragma once

/** 
 * This structure exists on the stack after calling switch process.  The original 
 * values are pushed onto the current processes stack, the stack is switched and then
 * the values are popped off the new stack.  This is a technique from xv6, and presumably
 * also BSD. 
 *
 * This must rely on the kernel having a per-process kernel stack that's always
 * mapped into memory?
 */

struct SavedKernelContext
{
    uintptr_t edi;
    uintptr_t esi;
    uintptr_t ebx;
    uintptr_t ebp;
    /* EIP is stored during the call to the process switching function by CALL. */
    uintptr_t eip;
};

/**
 * This structure is saved by the processor and then the interrupt handler stub
 * on the stack when an interrupt is called.
 */
struct SavedProcessContext
{
    // registers as pushed by pusha
    uint32 edi;
    uint32 esi;
    uint32 ebp;
//    uint32 oesp;      // useless & ignored
    uint32 ebx;
    uint32 edx;
    uint32 ecx;
    uint32 eax;

    // rest of trap frame
    uint16 gs;
    uint16 padding1;
    uint16 fs;
    uint16 padding2;
    uint16 es;
    uint16 padding3;
    uint16 ds;
    uint16 padding4;
    uint32 interrupt_number;

    // below here defined by x86 hardware
    uint32 error_code;
    uint32 eip;
    uint16 cs;
    uint16 padding5;
    uint32 eflags;

    // below here only when crossing rings, such as from user to kernel
    uint32 esp;
    uint16 ss;
    uint16 padding6;
};


