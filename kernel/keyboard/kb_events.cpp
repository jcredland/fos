#include <keyboard/keyboard.h>

const KeyEvent KeyEvent::invalid(KeyboardKeyCode(-1), Modifiers(), '\0');


KeyEvent KeyEventManager::next()
{
    if (! keyboard)
        return KeyEvent::invalid; 

    int c = keyboard->read_char();  

    if (c == -1)
        return KeyEvent::invalid; 

    KeyEvent e {(KeyboardKeyCode) c, 
        Modifiers(), 
        keymap_uk[c].key_without_shift};

    return e; 
}
