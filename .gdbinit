#;set architecture i8086
set disassembly-flavor intel
target remote localhost:1234
set history filename .gdb_history
set history save on
set history expansion on
symbol-file build/fos.sym
#alias stk = x/20w $esp
source bps
