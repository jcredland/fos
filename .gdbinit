#;set architecture i8086
target remote localhost:1234
set history filename .gdb_history
set history save on
layout asm
#;b *0x7c00
b *0x8204
c
