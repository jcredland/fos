#include <interrupt/interrupt.h>
#include <keyboard/kb_data.h>
#include <keyboard/kb_hardware_driver.h>

extern InterruptDriver interrupt_driver;

KeyboardHardwareDriver::KeyboardHardwareDriver()
    :
    last_scan_code (0)
{
    kerror ("registering keyboad interrupt handler");
    interrupt_driver.set_handler ( (uint8) Interrupt8259PIC::Interrupt::KEYBOARD, this);
}
const char* KeyboardHardwareDriver::get_device_name() const
{
    return kDeviceName;
}

int KeyboardHardwareDriver::read_char() 
{
    if (input_buffer.is_empty())
        return 0x8000; 
    else
        return input_buffer.next(); 
}

void KeyboardHardwareDriver::handle_interrupt (uint8 /* interrupt_number */, uint32 /* err code */) 
{
    uint8_t scan_code = inb (0x60);

    if ( (scan_code & 0x80) != 0x80)
    {
        input_buffer.push(mac_scancode_map[scan_code]);
    }
    else
    {
        input_buffer.push(mac_scancode_map[scan_code & 0x7F] | 0x4000); 
    }

    Interrupt8259PIC::send_eoi (Interrupt8259PIC::IRQ::KEYBOARD);
}



