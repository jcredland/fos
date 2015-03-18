#pragma once
#include <klibrary/klibrary.h>

/** An interface which classes implement if they want to accept a CliCommand. */
class CliCommand
{
    public:
        virtual ~CliCommand() {} 
        /** Execute a command.  Must return an error code, 0 for success. */
        virtual bool is_command_supported(const KString & command) = 0; 
         
        virtual int execute_cli_command(const kvector<kstring> & parameter_list) = 0; 
};


void cli_register_command(CliCommand * cli_command); 
void cli_main();

