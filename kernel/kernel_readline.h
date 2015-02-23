
class InputStream
{
public:
    void handle_event(const Event & e)
    {
        if (e.type == Event::KEYDOWN)
        {

        }
        else if (e.type == Event::KEYUP)
        {
            c = e.value;
        }
    }
    /** Returns the next character from the input stream. */
    char next()
    {
        char cx = c; 
        c = 0; 
        return cx; 
    }
private:
    char c;
};

class InputString 
{
public:
    void handle_event(const Event & e)
    {
        in_stream.handle_event(e); 
    }
private:
    InputStream in_stream;
    KernelString data;
};
