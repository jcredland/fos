/* Kernel. */


#include "vga.h"

int main()
{
//    char * video_memory = (char *) 0xb8000;
//    *video_memory = 'x';
    VgaDriver vga;
    vga.write(1, 10, "Welcome to the kernel");
    while (1)
    {}
}
