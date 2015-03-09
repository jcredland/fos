#include <proc/process.h>

ProcessTable::ProcessTable()
    :
        next_number(1)
{
}

int ProcessTable::launch_kthread(void(*new_process_function)(void), const KString & name)
{
    Process new_process;

    new_process.entry_point = new_process_function;
    new_process.name = name; 
    new_process.number = next_number++;
    new_process.state = Process::ReadyToPlay;

    proc_table.push_back(new_process); 
    
    return new_process.number; 
}
