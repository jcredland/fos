#include <klibrary/klibrary.h>
#include <cli/cli_inputstring.h>

void cli_main()
{
    InputString command_line;
    KString input; 

    do 
    {
        vga.write("FING READY:");
        input = command_line.get_line(); 
        vga.writeln(input); 

    } 
    while (input != KString("quit")); 
};
