#pragma once
#include <klibrary/inline_asm_utils.h>
#include <interrupt/interrupt.h>

class Timer 
:
    public DeviceInterruptHandler
{
public:
    Timer();
    
    void handle_interrupt(uint8 interrupt_number, uint32 error_code) override;
    volatile uint64_t timer; 

    void delay_ms(int ticks)
    {
        ticks /= 10; 
        uint64_t t1 = timer; 
        while (timer < (t1 + ticks))
        {}
    }
};

extern Timer timer;

