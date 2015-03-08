
#include "kernel_log.h"
#include <display/vga.h>


enum KLogLevel
{
    KL_ERROR,
    KL_DEBUG
};

void klog (KLogLevel /* level */, const KString& log)
{
    vga.writeln (log);
}

void kdebug (const KString& log)
{
    vga.set_colour (VgaColours::blue);
    klog (KL_DEBUG, log);
}

void kerror (const KString& log)
{
    vga.set_colour (VgaColours::white);
    klog (KL_ERROR, log);
}

void khex_dump (const char* buffer, uint16 bytes_to_show)
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
    if (counter != 16)
        vga.writeln("");
}


