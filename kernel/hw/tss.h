#pragma once
#include <std_types.h>


/** Used to find the kernel stack location when entering Ring 0. 
 *
 * Looks like we don't need this with SYSENTER/SYSEXIT
 */ 
struct F_PACKED TssEntry
{
    uint32 prev_tss;   
    uint32 esp0;       // The stack pointer to load when we change to kernel mode.
    uint32 ss0;        // The stack segment to load when we change to kernel mode.

    uint32 unused[33];
};

class X86TSS
{
public:
    


private:


};
