#pragma once

struct InterruptStackFrame
{
    // registers as pushed by pusha
    uint32 edi;
    uint32 esi;
    uint32 ebp;
    uint32 oesp;      // useless & ignored
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


