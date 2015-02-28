
UkKeyboardDriver keyboard_driver;

void interrupt_keyboard(int)
{
    uint8_t scan_code = inb(0x60);
    
    Event e;

    if (scan_code & 0x80)
    {
        e.type = Event::KEYUP;
        e.data = scan_code & 0x7F;
    }
    else
    {
        e.type = Event::KEYDOWN;
        e.data = scan_code;
    }

    event_queue.add(e); 

    Interrupt8259PIC::send_eoi(Interrupt8259PIC::IRQ::KEYBOARD); 
}

