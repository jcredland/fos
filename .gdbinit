#;set architecture i8086
set disassembly-flavor intel
target remote localhost:1234
set history filename .gdb_history
set history save on
set history expansion on
#layout asm
b *0x7c00
symbol-file build/fos.sym
b *0x8204
alias stk = x/20w $esp
c
