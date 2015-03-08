#pragma once

#include <klibrary/klibrary.h>
#include <keyboard/keyboard.h>

/**
 * A basic, one-line, text editor. 
 */
class InputString 
{
public:
    InputString() 
    {}

    /** 
     * Waits for input from the user.  When the user presses enter 
     * returns the string to the user. */
    KString get_line() 
    {
        data.clear(); 

        left_position = vga.get_cursor_position(); 

        KeyEventManager key_events; 
        KeyEvent event; 

        bool should_finish = false;

        do
        {
            event = key_events.next(); 

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

private:
    void process_event(const KeyEvent & e)
    {
        if (! e.is_control_character())
            data.append_char(e.character);
        else if (e.is_backspace())
            data.remove_last_character(); 
    }

    void limit_history_size(int size_to_limit_to)
    {
        int excess = history.size() - size_to_limit_to; 

        if (excess > 0)
            history.erase(history.begin(), history.begin() + excess); 
    }

    void paint()
    {
        vga.set_cursor_position(left_position); 
        vga.write(data); 
    }

    uint32 left_position;

    KString data;
    
    kstd::kvector<KString> history;
};

