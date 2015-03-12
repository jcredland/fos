# FOS
## What?
It's a toy operating system.  The goal is to have it capable of loading and playing more than one copy of PONG.  

## Why?
It's good to have a deep understanding of how things work. And what better way to understand operating systems, memory allocation, debuggers and so on than to write an OS from scratch.  I can already do things with linker scripts that I had never imagined ...

## Status
In development. Boots and functions on QEMU. Not yet tested on real hardware. 

It has:
-   A read only FAT16 file system. 
-   Physical and virtual memory management
-   Text mode VGA driver
-   A custom boot loader
-   Interrupts
-   A crude CLI

Next: 
-   Kernel threads
-   User processes and threads.
-   Synchronisation primitives (std::thread, maybe?)

## Milestones
Big milestones will be:
-   VGA graphics driver (or SVGA). 
-   libc
-   SMP and 64-bit
-   IPv6 stack

## Documentation

Status and documentation:
https://docs.google.com/presentation/d/1JLeGdG1nwIE328pD22edmPHvNbw9Wb6zIGZjvydZbrk/edit#slide=id.g7ac98eeed_030

## Compiling and running

You will need a cross-compiler and the other GNU binutils kit.  Use the i686-elf configuration.  There are instructions here: http://wiki.osdev.org/GCC_Cross-Compiler

Also: 
    -   QEMU for testing
    -   NASM for assembly 

It runs on QEMU for testing.  The *execute* script in the root folder has a -d option that will allow you to attach a debugger.  

The .gdbinit script in the root folder contains a series of commands that will set gdb up properly.  My suggestion is: 
- Put *set auto-load local-gdbinit* in your ~/.gdbinit.
- Run i686-elf-gdb from the root folder of the FOS source. 

Then it'll automatically run the FOS specific commands.
