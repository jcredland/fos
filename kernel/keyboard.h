
enum Key
{
    KEY_UNKNOWN,
    KEY_1 = 1, 
    KEY_2,
    KEY_3,
    KEY_4,
    KEY_5,
    KEY_6,
    KEY_7,
    KEY_8,
    KEY_9,
    KEY_0,
    KEY_BACKSPACE,
    KEY_SHIFT = 100
};

struct KeyboardEvent
{
    Key key;
    bool shiftPressed;
    bool ctrlPressed;
};

/** Keyboard drivers take scan codes from the actual keyboard and generate key events that represent
 * useful information like say the letters on the keyboard and whether shift is down. In the future 
 * we may introduce another layer to handle different types of keyboard.  This will just convert other
 * keyboards to a standard set of scan_codes. */
class UkKeyboardDriver
{
public:
    void handle_event(const Event & e)
    {
        Key k = convert_scan_code_to_key(e.data);
        if (e.type == Event::KEYUP)
        {

        }
    }

    Key convert_scan_code_to_key(uint16_t scan_code)
    {
        return Key(mapping[scan_code]);
    }

    Key mapping[128] = { 
        KEY_UNKNOWN, KEY_1, KEY_2, KEY_3, KEY_4, KEY_5, KEY_6, KEY_7, KEY_8, KEY_9, KEY_0 };

private:
    KeyboardEvent keyEventQueue;
};

extern UkKeyboardDriver keyboard_driver;

/** Function called by the interrupt stub. */
void interrupt_keyboard(int);

