#include <klibrary/klibrary.h>
#include <keyboard/keyboard.h>
#include <cli/cli_inputstring.h>

InputString::InputString() 
    : 
        left_position(0),
        length_of_last_write(0)
    {}

KString InputString::get_line() 
{
    data.clear(); 
    length_of_last_write = 0; 

    left_position = vga.get_cursor_position(); 

    KeyEventManager key_events; 
    KeyEvent event; 

    bool should_finish = false;

    do
    {
        event = key_events.next(); 

        if (event.key_code == 0)
            continue;

        if (event != KeyEvent::invalid)
        {
            if (event.is_enter_or_return())
                should_finish = true;

            process_event(event); 
            paint(); 
        }

    } while (! should_finish); 
   
    history.push_back(data);  
    limit_history_size(10); 

    return data; 
}

void InputString::process_event(const KeyEvent & e)
{
    if (! e.is_control_character())
        data.append_char(e.character);
    else if (e.is_backspace())
        data.remove_last_character(); 
}

void InputString::limit_history_size(int size_to_limit_to)
{
    int excess = history.size() - size_to_limit_to; 

    if (excess > 0)
        history.erase(history.begin(), history.begin() + excess); 
}

void InputString::paint()
{
    vga.set_cursor_position(vga.get_cursor_position() - length_of_last_write); 
    for (uint32 i = 0; i < length_of_last_write; ++i)
        vga.write(" "); 

    vga.set_cursor_position(vga.get_cursor_position() - length_of_last_write); 
    vga.write(data); 
    length_of_last_write = data.length();
}
