
/** An event. */
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

/** 
 * The event manager takes incoming events and assigns them to
 * process event queues.
 */


/** A queue of events. QueueSize must be a power of two. */
template <size_t QueueSize, EventType = Event>
class EventQueue
{
public:
    EventQueue()
        :
            mask(QueueSize - 1),
            next_write(0),
            next_read(0)
    {}

    bool is_full()
    {
        return ((next_write + 1) & mask) == (next_read & mask);
    }

    void add(const EventType & e)
    {
        InterruptDriver::disable_hardware_interrupts(); 

        if (! is_full())
            queue[mask & (next_write++)] = e; 

        InterruptDriver::enable_hardware_interrupts(); 
    }

    /** Adds an event to the queue without disabling interrupts.  Use 
     * when interrupts are already disabled, usually from an interrupt 
     * handler.  */
    void add_without_cli(const EventType & e)
    {
        if (! is_full())
            queue[mask & (next_write++)] = e; 
    }


    /** Gets the next event in the queue.
     *
     * Not to be called from interrupt service routines. */
    TypeEvent next()
    {
        EventType e; 

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
    Event queue[QueueSize];

    F_DECLARE_NO_COPY(EventQueue)
};

extern EventQueue event_queue;

