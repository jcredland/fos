

/** These methods handle the programmable interrupt controller. 
 *
 * It assumes a nice legacy 8259 PIC. 
 *
 * By default the hardware interrupts overlap with some CPU traps making it
 * difficult or impossible to tell what's happening in the interrupt handler. One
 * technique is to look at the instruction (!) at the return pointer in the 
 * stack and see if it's an INT instruction!
 *
 * On the other hand we can reprogram the PIC and move the interrupts to new 
 * values, so maybe we should do that :) 
 * 
 */
class Interrupt8259PIC
{
public:
    enum 
    {
        PIC1_COMMAND    = 0x20, 
        PIC1_DATA       = 0x21,
        PIC2_COMMAND    = 0xA0, 
        PIC2_DATA       = 0xA1
    }; 

    enum 
    {
        EndOfInterrupt = 0x20
    };

    /** Initalization command word 1. */
    enum ICW1 
    { 
        USE_ICW4    = 0x1,
        SINGLE      = 0x2, 
        INTERVAL4   = 0x4, 
        LEVEL_TRIGGER_MODE = 0x8,
        INIT        = 0x10
    };

    /** Initalization command word 4. */
    enum ICW4
    {
        MODE_8086       = 0x1,
        MODE_AUTO       = 0x2,
        MODE_BUF_SLAVE  = 0x8,
        MODE_BUF_MASTER = 0xC,
        MODE_SFNM       = 0x10
    };     

    /** Disables the PIC entirely by masking all interrupts.  Use if we want to use the APIC. 
     * Uses the OCW1 (operational command word) with the A0 bit set to say we are doing 
     * some masking.  
     *
     * (The A0 bit is set by using the _DATA register which as a 1 in the address 0 line). 
     * */
    static void disableAll()
    {
        outb(PIC1_DATA, 0xff); 
        outb(PIC2_DATA, 0xff); 
    }

    static void enableAll()
    {
        outb(PIC1_DATA, 0x00); 
        outb(PIC2_DATA, 0x00); 
    }

    enum class IRQ
    {
        SYSTEM_TIMER = 0,
        KEYBOARD = 1, 
        ATA1 = 14
    };

    enum class Interrupt
    {
        SYSTEM_TIMER = 0x20,
        KEYBOARD = 0x21,
        ATA1 = 0x2E
    };

    /** Reinitalizes the PIC chips with new offsets.  The new offsets are 0x20 and 0x28. 
     * IRQ0 then maps to 0x20. 
     *
     * The final arrangement means: 
     * 0x20 - IRQ0 System Timer
     * 0x21 - IRQ1 Keyboard Controller
     * 0x22
     * 0x23 - IRQ3 Serial port
     * 0x24 - IRQ4 Serial port
     * 0x25 - IRQ5 Serial port
     * 0x26 - IRQ6 Floppy disk
     * 0x27 - IRQ7 Parallel port
     * 0x28 - IRQ8 RTC
     * 0x29 - IRQ9 ACPI
     * 0x2A - IRQ10 
     * 0x2B - IRQ11 
     * 0x2C - IRQ12
     * 0x2D - IRQ13 
     * 0x2E - IRQ14 ATA controller
     * 0x2F - IRQ15 ATA controller 2
     *
     */
    static void remapInterrupts() 
    {
        uint8_t mask1 = inb(PIC1_DATA); 
        uint8_t mask2 = inb(PIC2_DATA); 

        const int offset1 = 0x20; 
        const int offset2 = 0x28;

        /* Putting the PIC into initalization mode makes it wait for three
         * more data bytes, ICW1, 3 and 4. */
        sendAndWait(PIC1_COMMAND, ICW1::INIT | ICW1::USE_ICW4);
        sendAndWait(PIC2_COMMAND, ICW1::INIT | ICW1::USE_ICW4);
       
        sendAndWait(PIC1_DATA, offset1);
        sendAndWait(PIC2_DATA, offset2); 
        
        sendAndWait(PIC1_DATA, 0x4); /* look out for a slave */
        sendAndWait(PIC2_DATA, 0x2); /* Slave id. */
        
        sendAndWait(PIC1_DATA, ICW4::MODE_8086);
        sendAndWait(PIC2_DATA, ICW4::MODE_8086); 
    }

    static void sendEOI(IRQ irq)
    {
        if ((int) irq >= 8) 
            outb(PIC2_COMMAND, EndOfInterrupt);

        outb(PIC1_COMMAND, EndOfInterrupt); 
    }

private:
    static void sendAndWait(uint16_t port, uint8_t byte)
    {
        outb(port, byte); 
        io_wait(); 
    }
};
    
