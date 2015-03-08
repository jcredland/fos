#include <interrupt/interrupt.h>
#include <keyboard/kb_data.h>
#include <keyboard/kb_hardware_driver.h>

KeyboardHardwareDriver::KeyboardHardwareDriver()
    :
    last_scan_code (0)
{
    kerror ("registering keyboad interrupt handler");
    install_interrupt_handler ( (uint8) Interrupt8259PIC::Interrupt::KEYBOARD, this);
}
const char* KeyboardHardwareDriver::get_device_name() const
{
    return kDeviceName;
}

int KeyboardHardwareDriver::read_char() 
{
    if (input_buffer.is_empty())
        return -1; 
    else
        return input_buffer.next(); 
}

void KeyboardHardwareDriver::handle_interrupt (uint8 /* interrupt_number */) 
{
    uint8_t scan_code = inb (0x60);

    /* TODO - introduce the mapping, and a buffer. */
    if ( (scan_code & 0x80) != 0x80)
        input_buffer.push(scan_code);

    Interrupt8259PIC::send_eoi (Interrupt8259PIC::IRQ::KEYBOARD);
}



