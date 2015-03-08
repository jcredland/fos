#pragma once
#include <klibrary/inline_asm_utils.h>

class Timer 
{
public:
    Timer()
    {
        const int hz = 1000;
        int divisor = 1193180 / hz;   /* Calculate our divisor */
        outb(0x43, 0x36);             /* Set our command byte 0x36 */
        outb(0x40, divisor & 0xFF);   /* Set low byte of divisor */
        outb(0x40, divisor >> 8);     /* Set high byte of divisor */
    }

    volatile uint64_t timer; 

    void delay_ms(int ticks)
    {
        uint64_t t1 = timer; 
        while (timer < (t1 + ticks))
        {}
    }
};

extern Timer timer;
