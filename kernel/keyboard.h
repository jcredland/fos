
enum Key
{
            K_NONE = -1,
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
            K_RIGHT
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

private:
    KeyboardEvent keyEventQueue;
};

extern UkKeyboardDriver keyboard_driver;

/** Function called by the interrupt stub. */
void interrupt_keyboard(int);

