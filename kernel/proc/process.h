#pragma once

#include <klibrary/klibrary.h>
#include <mem/mem.h>

/** State of a process. */
struct Process
{
    enum Type
    {
        Thread, 
        UserProcess,
        KernelProcess
    };

    /** Create an empty, invalid process. */
    Process();

    /** Create a process from an existing parent process. */
    Process(Process & parent_process, const kstring & _name, uint16 pid_number, Type _type);

    kstring name;
    uint16 pid; // pids are from 1 up.
    uint16 parent_pid;
    void(*entry_point)(void);

    enum State
    {
        Invalid,
        ReadyToPlay,
        Running,
        Finished
    } state;

    Type type;

    uintptr_t stack_pointer;

    ProcessMemoryMap * memory_map; 

    bool is_void() const { return state == Invalid; }
    void create_thread_stack(size_t size);

private:
    MemoryRegion * stack; 

    friend class ProcessManager;
    
    /** Construct the initial kernel process. */
    Process(ProcessMemoryMap * kernel_memory_map);
};
