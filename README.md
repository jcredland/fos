# FOS
## What?
It's a toy operating system.  The goal is to have it capable of loading and playing more than one copy of PONG.  

## Why?
It's good to have a deep understanding of how things work. And what better way to understand operating systems, memory allocation, debuggers and so on than to write an OS from scratch.  I can already do things with linker scripts that I had never imagined ...

## Status
In development. Boots and functions on QEMU. Not yet tested on real hardware. 

It has:
-   A read only FAT16 file system. 
-   Memory management
-   Text mode VGA driver
-   A custom boot loader
-   Interrupts
-   The beginnings of a keyboard driver

And more.
## Milestones
Big milestones will be:
-   VGA graphics driver (or SVGA). 
-   libc
-   user processes
-   SMP
-   IPv6 stack

## Documentation

Status and documentation:
https://docs.google.com/presentation/d/1JLeGdG1nwIE328pD22edmPHvNbw9Wb6zIGZjvydZbrk/edit#slide=id.g7ac98eeed_030

