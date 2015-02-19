ENTRY(main);
SECTIONS
{
    . = 0x7C00;
    .boot : AT(0x7c00)
    {
        bootloader.elf ( .text )
    }
    .text : ALIGN(0x1000)
    {
        _text = .;
        *(.text);
        _text_end = .;
        . = ALIGN(4096);
    }
    .data :
    {
            start_ctor_sec = .;
         __CTOR_LIST__ = .;
          LONG((__CTOR_END__ - __CTOR_LIST__) / 4 - 2);
          *(.ctors);
          LONG(0);
          __CTOR_END__ = .;

          LONG(0xDEADAAAA);

          __DTOR_LIST__ = .;
          LONG((__DTOR_END__ - __DTOR_LIST__) / 4 - 2);
          *(.dtors);
          LONG(0);
          __DTOR_END__ = .; 
          LONG(0xDEADAAAA);
        _data = .;
        *(.bss);
        *(.bss*);
        *(.data);
        *(.rodata*);
        *(COMMON)
        _data_end = .;
    }
/*    .sig : AT(0x7DFE)
    {
        SHORT(0xaa55);
    } 
    /DISCARD/ :
    {
        *(.note*);
        *(.iplt*);
        *(.igot*);
        *(.rel*);
        *(.comment);
    }  */
}
