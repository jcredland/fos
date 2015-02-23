
class EventWatcher
{
public:
    EventWatcher(EventQueue & source, VgaDriver & vga)
        :
            source(source),
            vga(vga)
    {
        vga.writeln("EventWatcher started"); 
    }

    void poll()
    {
        Event e = source.next(); 

        if (e.type != Event::VOID)
        {
            KString info = e.to_debug_string(); 
            vga.writeln(KString("event: ") + info); 
        }
    }
private:
    EventQueue & source; 
    VgaDriver & vga;
};

