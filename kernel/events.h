

struct Event 
{
    enum Type 
    {
        VOID,
        KEYDOWN,
        KEYUP
    };
    Type type; 

    KString to_debug_string()
    {
        KString s("Event:"); 
        s.append_hex(data); 
        return s;
    }

    uint32_t data; 
};

class EventQueue
{
public:
    EventQueue()
        :
            mask(0xF),
            next_write(0),
            next_read(0)
    {}

    bool is_full()
    {
        return ((next_write + 1) & mask) == (next_read & mask);
    }
    void add(Event e)
    {
        InterruptDriver::disable_hardware_interrupts(); 

        if (! is_full())
            queue[mask & (next_write++)] = e; 

        InterruptDriver::enable_hardware_interrupts(); 
    }

    /** Gets the next event in the queue.
     *
     * Not to be called from interrupt service routines. */
    Event next()
    {
        Event e; 

        if (next_write == next_read)
            e.type = Event::VOID; 
        else 
            e = queue[mask & next_read++];

        return e; 
    }
private:
    uint64_t mask;
    uint64_t next_write; 
    uint64_t next_read;
    Event queue[16];

    F_DECLARE_NO_COPY(EventQueue)
};

extern EventQueue event_queue;

