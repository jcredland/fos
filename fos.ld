ENTRY(main);
SECTIONS
{
    . = 0x7C00;
    .boot : AT(0x7c00)
    {
        fos.elf ( .text )
    }
    .text : ALIGN(0x1000)
    {
        _text = .;
        *(.text);
        _text_end = .;
    }
    .data :
    {
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
