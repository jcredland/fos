#pragma once
#include <klibrary/klibrary.h>
#include <keyboard/keyboard.h>
/** Turing Incomplete ... until bored. 
 * 
 * 
 */

/**
 * A basic, one-line, text editor. 
 */
class InputString 
{
public:
    InputString() 
    {}

    /** Waits for input from the user.  When the user presses enter 
     * returns the string to the user. */
    KString get() 
    {
        KeyboardEventManager key_events; 
        KeyboardEvent event; 

        while (event = key_event.next() && ! event.is_enter())
            process_event(event); 
       
        history.push_back(data);  
        limit_history_size(10); 
        return data; 
    }
private:
    void process_event(const KeyboardEvent & e)
    {
        if (! e.is_control_character())
            data.append_char(get_utf8_character());
        else if (e.is_delete())
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
    }

    InputStream in_stream;
    KString data;
    kstd::vector<KString> history;
};
