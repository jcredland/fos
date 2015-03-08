#include <klibrary/klibrary.h>
#include <cli/cli.h>
#include <cli/cli_inputstring.h>

static kstd::kvector<CliCommand *> cli_commands;

void cli_register_command(CliCommand * new_command)
{
    if (kstd::find(cli_commands.begin(), cli_commands.end(), new_command) == cli_commands.end())
        cli_commands.push_back(new_command); 
}

void cli_find_and_run(KString & command_name)
{
    for (auto c: cli_commands)
    {
        if (c->is_command_supported(command_name))
        {
            kstd::kvector<KString> params; 
            params.push_back(command_name); 
            c->execute_cli_command(params);
            return;
        }
    }
    vga.writeln("command not found"); 
};

void cli_main()
{
    InputString command_line;
    KString input; 

    do 
    {
        vga.writeln("_"); 
        vga.write("FING READY:");
        input = command_line.get_line(); 
        vga.writeln("_"); 
        vga.writeln(input); 
        cli_find_and_run(input); 

    } 
    while (input != KString("quit")); 
};
