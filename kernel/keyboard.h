
/** @file
 * Concerned with getting interrupts from the keyboard and providing:
 * - Parsed information on key combinations to the event queue.
 * - Access to raw scan code information for applications that might want it.
 */

/**
 * All the possible key codes we have, at least in English.
 */
enum Key
{
    K_NONE = -1,
    K_BACKTICK, K_ESCAPE, K_1, K_2, K_3, K_4, K_5, K_6, K_7, K_8,
    K_9, K_0, K_MINUS, K_EQUALS, K_BACKSPACE, K_TAB, K_Q, K_W, K_E, K_R,
    K_T, K_Y, K_U, K_I, K_O, K_P, K_SQ_BRACKET_OPEN, K_SQ_BRACKET_CLOSE, K_ENTER, K_A,
    K_S, K_D, K_F, K_G, K_H, K_J, K_K, K_L, K_SEMI_COLON, K_APOS,
    K_BACK_SLASH, K_Z, K_X, K_C, K_V, K_B, K_N, K_M, K_COMMA, K_DOT,
    K_SLASH, K_SPACE, K_F1, K_F2, K_F3, K_F4, K_F5, K_F6, K_F7, K_F8,
    K_F9, K_F10, K_SCROLL_LOCK, K_PAD7, K_PAD_UP,
    K_PAD9, K_PAD_MINUS, K_PAD_LEFT, K_PAD5, K_PAD_RIGHT,
    K_PAD_PLUS, K_PAD1, K_PAD2, K_PAD3, K_PAD0, K_PAD_DOT,
    K_F11, K_F12, K_LSHIFT, K_RSHIFT,
    K_INSERT, K_HOME, K_PAGEUP, K_NUMLOCK, K_PAD_STAR,
    K_PRINT_SCREEN, K_CAPSLOCK, K_HASH, K_BACK_SLASH_B, K_LCTRL,
    K_RALT, K_DELETE, K_END, K_PAGEDOWN, K_PAD_SLASH, K_PAD_ENTER,
    K_LALT, K_RCTRL, K_LEFT, K_UP, K_RIGHT
};


class Keyboard
    :
    public Device,
    public DeviceInterruptHandler
{
public:
    Keyboard()
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

    char read_char() override
    {
        uint8 l = last_scan_code;
        last_scan_code = 0;
        return l;
    }

    void handle_interrupt (uint8 /* interrupt_number */) override
    {
        uint8_t scan_code = inb (0x60);

        kdebug ("keyboard event " + KString (scan_code));

        if ( (scan_code & 0x80) != 0x80)
        last_scan_code = scan_code;

        /*        Event e;

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

                keyboard_event_queue.add_without_cli(e);  */

        Interrupt8259PIC::send_eoi (Interrupt8259PIC::IRQ::KEYBOARD);
    }


private:
    uint8 last_scan_code;

    Key convert_scan_code_to_key (uint16_t scan_code)
    {
        return Key (mapping[scan_code]);
    }

    Key mapping[128] =
    {
        K_BACKTICK, K_ESCAPE, K_1, K_2, K_3, K_4, K_5, K_6, K_7, K_8,
        K_9, K_0, K_MINUS, K_EQUALS, K_BACKSPACE, K_TAB, K_Q, K_W, K_E, K_R,
        K_T, K_Y, K_U, K_I, K_O, K_P, K_SQ_BRACKET_OPEN, K_SQ_BRACKET_CLOSE, K_ENTER, K_A,
        K_S, K_D, K_F, K_G, K_H, K_J, K_K, K_L, K_SEMI_COLON, K_APOS,
        K_BACK_SLASH, K_Z, K_X, K_C, K_V, K_B, K_N, K_M, K_COMMA, K_DOT,
        K_SLASH, K_SPACE, K_F1, K_F2, K_F3, K_F4, K_F5, K_F6, K_F7, K_F8,
        K_F9, K_F10, K_SCROLL_LOCK, K_PAD7, K_PAD_UP, K_PAD9, K_PAD_MINUS, K_PAD_LEFT, K_PAD5, K_PAD_RIGHT,
        K_PAD_PLUS, K_PAD1, K_PAD2, K_PAD3, K_PAD0, K_PAD_DOT, K_F11, K_F12, K_LSHIFT, K_RSHIFT,
        K_INSERT, K_HOME, K_PAGEUP, K_NUMLOCK, K_PAD_STAR, K_PRINT_SCREEN, K_CAPSLOCK, K_HASH, K_BACK_SLASH_B, K_LCTRL,
        K_RALT, K_DELETE, K_END, K_PAGEDOWN, K_PAD_SLASH, K_PAD_ENTER, K_LALT, K_RCTRL, K_LEFT, K_UP,
        K_RIGHT, K_NONE, K_NONE, K_NONE, K_NONE, K_NONE, K_NONE, K_NONE, K_NONE, K_NONE,
        K_NONE, K_NONE, K_NONE, K_NONE, K_NONE, K_NONE, K_NONE, K_NONE, K_NONE, K_NONE,
        K_NONE, K_NONE, K_NONE, K_NONE, K_NONE, K_NONE, K_NONE, K_NONE
    };

    const char* kDeviceName = "keyboard";
};

extern Keyboard keyboard_device;

/** Function called by the interrupt stub. */
void interrupt_keyboard (int);


