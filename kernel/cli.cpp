

void cli_main()
{
    InputString command_line;
    do 
    {
        write("FING READY:");
        KString input = command_line.get_string(); 

    } while (input != "quit"); 
};
