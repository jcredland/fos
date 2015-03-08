#pragma once
#include <interrupt/interrupt.h>

/**
 * The keyboard hardware driver takes scan codes and provides a stream of 
 * KeyboardKeyCodes. 
 *
 * Right now it assumes a one to one mapping between scan codes and key codes.
 * However in future a look-up table will be provided which allows keyboards 
 * with different scan codes to work. 
 */
class KeyboardHardwareDriver
    :
    public Device,
    public DeviceInterruptHandler
{
public:
    KeyboardHardwareDriver()
        :
        last_scan_code (0)
    {
        kerror ("registering keyboad interrupt handler");
        install_interrupt_handler ( (uint8) Interrupt8259PIC::Interrupt::KEYBOARD, this);
    }
    const char* get_device_name() const
    {
        return kDeviceName;
    }

    int read_char() override
    {
        int l = last_scan_code;
        last_scan_code = -1;
        return l;
    }

    void handle_interrupt (uint8 /* interrupt_number */) override
    {
        uint8_t scan_code = inb (0x60);

        /* TODO - introduce the mapping, and a buffer. */
        if ( (scan_code & 0x80) != 0x80)
            last_scan_code = (int) scan_code;

        Interrupt8259PIC::send_eoi (Interrupt8259PIC::IRQ::KEYBOARD);
    }

private:
    int last_scan_code; /* should be a buffer. */
    const char* kDeviceName = "keyboard";
};

/** Function called by the interrupt stub. */
void interrupt_keyboard (int);


