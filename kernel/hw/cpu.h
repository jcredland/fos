#pragma once
#include <std_types.h>

class CPU
{
public:
    enum FLAGS
    {
        IF = 0x200
    };
    static uint32 get_eflags()
    {
        uint32 eflags;
        asm volatile ("pushfl; pop %0" : "=r" (eflags));  /* Why does this need volatile? */
        return eflags;
    }

    static bool get_interrupt_flag_state() 
    { 
        return FLAGS::IF == (get_eflags() & FLAGS::IF); 
    }

    static void set_interrupt_flag(bool s)
    {
        if (s)
            asm volatile ("sti"); 
        else
            asm volatile ("cli"); 
    }
};

