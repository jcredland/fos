#include <keyboard/keyboard.h>

KeyEventManager::KeyEventManager()
    : 
    lctrl(false), 
    rctrl(false), 
    lshift(false), 
    rshift(false)
{
    keyboard = device_manager.get_device_by_name("keyboard"); 

    if (keyboard == nullptr)
        kerror("KeyEventManager: no keyboard driver found."); 
}

const KeyEvent KeyEvent::invalid(KeyboardKeyCode(-1), Modifiers(), '\0');

KeyEvent KeyEventManager::next()
{
    if (! keyboard)
        return KeyEvent::invalid; 

    int c = keyboard->read_char();  

    bool was_buffer_empty = (c & 0x8000) == 0x8000; 
    bool is_key_up = (c & 0x4000) == 0x4000; 

    if (was_buffer_empty)
        return KeyEvent::invalid; 

    uint16 key_code = (uint16) (c & 0x3FFF);


    /* TODO - This bit isn't efficient and we should move to a direct lookup. */
    for (size_t i = 0; i < keymap_uk_len; ++i)
    {
        auto & map_entry = keymap_uk[i];

        if (map_entry.key_code == key_code)
        {
            update_modifiers(KeyboardKeyCode(key_code), is_key_up); 

            if (! is_key_up)
            {
                char32_t character; 

                if (current_modifiers.shift)
                    character = map_entry.key_with_shift;
                else
                    character = map_entry.key_without_shift;

                return KeyEvent {KeyboardKeyCode(key_code), current_modifiers, character};
            }
            else
            {
                /* Really we should go back to the buffer and get another key 
                 * if one is available here. */
                return KeyEvent::invalid;
            }
        }
    }

    return KeyEvent::invalid;
}


void KeyEventManager::update_modifiers(KeyboardKeyCode key_code, bool is_key_up)
{
    bool b = ! is_key_up;
    switch (key_code) 
    {
        case K_LSHIFT:
            lshift = b;
            break;
        case K_RSHIFT:
            rshift = b;
            break;
        case K_LCTRL:
            lctrl = b; 
            break;
        case K_RCTRL:
            rctrl = b;
            break;
        default:
            break;
    }
    current_modifiers.shift = lshift || rshift; 
    current_modifiers.control = lctrl || rctrl; 
}
