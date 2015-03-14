#include <hw/timer.h>
#include <proc/process.h>
#include <proc/proc_man.h>

extern ProcessManager procman;
extern InterruptDriver interrupt_driver;

Timer::Timer()
{
    const int hz = 100;
    int divisor = 1193180 / hz;   /* Calculate our divisor */
    outb(0x43, 0x36);             /* Set our command byte 0x36 */
    outb(0x40, divisor & 0xFF);   /* Set low byte of divisor */
    outb(0x40, divisor >> 8);     /* Set high byte of divisor */

    interrupt_driver.set_handler((uint8) Interrupt8259PIC::Interrupt::SYSTEM_TIMER, this);
}

void Timer::handle_interrupt(uint8 interrupt_number, uint32 error_code) 
{
    timer++;
    Interrupt8259PIC::send_eoi (Interrupt8259PIC::IRQ::SYSTEM_TIMER);
    procman.timer_interrupt();
}

