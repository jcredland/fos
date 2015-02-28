

enum KLogLevel
{
    KL_ERROR,
    KL_DEBUG
};

inline void klog (KLogLevel level, const KString& log)
{
    vga.writeln (log);
}

inline void kdebug (const KString& log)
{
    vga.set_colour (VgaColours::light_gray);
    klog (KL_DEBUG, log);
}

inline void kerror (const KString& log)
{
    vga.set_colour (VgaColours::yellow);
    klog (KL_ERROR, log);
}

inline void khex_dump (const char* buffer, uint16 bytes_to_show)
{
    int counter = 0;

    while (bytes_to_show--)
    {
        vga.write (KString ( (uint8) *buffer));
        vga.write (" ");

        if (++counter == 16)
            vga.writeln ("");

        ++buffer; 
    }
}


