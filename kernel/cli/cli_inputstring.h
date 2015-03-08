#pragma once

#include <klibrary/klibrary.h>
#include <keyboard/keyboard.h>

/**
 * A basic, one-line, text editor. 
 */
class InputString 
{
public:
    InputString();
    /** Waits for input from the user.  When the user presses enter 
     * returns the string. */
    KString get_line();
private:
    void process_event(const KeyEvent & e);
    void limit_history_size(int size_to_limit_to);
    void paint();

    uint32 left_position;
    uint32 length_of_last_write; 
    KString data;
    kstd::kvector<KString> history;
};

