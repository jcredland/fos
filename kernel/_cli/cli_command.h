
/** A base class for kernel commands. */
class CliCommand 
{
public:
    virtual ~CliCommand() {}
    /** Returns the name of this command as a zero terminated char array. */
    virtual const char * get_command_name() = 0; 
    /** Executes the command. */
    virtual void execute_command() = 0;

};

