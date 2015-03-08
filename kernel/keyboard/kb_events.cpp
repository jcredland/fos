#include <keyboard/keyboard.h>

const KeyEvent KeyEvent::invalid(KeyboardKeyCode(-1), Modifiers(), '\0');


KeyEvent KeyEventManager::next()
{
    if (! keyboard)
        return KeyEvent::invalid; 

    int c = keyboard->read_char();  

    if (c == -1)
        return KeyEvent::invalid; 

    /* TODO - This bit isn't efficient and we should move to a direct lookup. */

    for (int i = 0; i < keymap_uk_len; ++i)
    {
        auto & map_entry = keymap_uk[i];

        if (map_entry.key_code == (KeyboardKeyCode) c)
        {
            KeyEvent e {(KeyboardKeyCode) c, 
                Modifiers(), 
                map_entry.key_without_shift};

            return e; 
        }
    }
    return KeyEvent::invalid;
}
