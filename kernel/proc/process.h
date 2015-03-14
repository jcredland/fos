#pragma once

#include <klibrary/klibrary.h>
#include <mem/mem.h>
#include <interrupt/stack_frame.h>

/** State of a process. */
struct Process
{
    enum Type
    {
        kUserThread, 
        kUserProcess,
        kKernelThread
    };

    enum State
    {
        kInvalid,
        kRunnable,
        kRunning,
        kDead
    };

    /** Create an empty, invalid process. */
    Process();

    /** Create a process from an existing parent process. */
    Process(Process & parent_process, const kstring & _name, uint16 pid_number, Type _type);

    kstring name; // this could be on the heap to make the structure smaller.
    uint16 pid; // pids are from 1 up.
    uint16 parent_pid;
    SavedKernelContext * kcontext; // actually a pointer to the top of the proceses stack
    State state;
    Type type;

    ProcessMemoryMap * memory_map; 

    bool is_void() const { return state == kInvalid; }

    /** Creates a new stack for a new thread.  Returns a pointer to the
     * interrupt stack frame which can be used to set the registers for
     * the new function, and in particular the entry point. */
    SavedProcessContext * create_thread_stack(size_t size);
private:
    MemoryRegion * stack; 

    friend class ProcessManager;
};
