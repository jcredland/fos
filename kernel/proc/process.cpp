#include <proc/process.h>
#include <mem/process_memory_map.h>


Process::Process (Process& parent_process, const kstring& _name, uint16 pid_number, Type _type)
    :
    name (_name),
    pid (pid_number),
    state (kRunnable),
    type (_type),
    memory_map (nullptr),
    stack (nullptr)
{
    parent_pid = parent_process.pid;
}

Process::Process()
    :
    state (kInvalid)
{}

extern "C" {
    void magic_return_function();
    extern void interrupt_return();
};

SavedProcessContext* Process::create_thread_stack (size_t size_of_stack)
{
    if (size_of_stack == 0 || memory_map == nullptr)
    {
        kerror ("process: problem with create_thread_stack.");
        return nullptr;
    }

    MemoryRange stack_range;

    if (type == kKernelThread)
        stack_range = memory_map->get_free_memory_range (kThreadStackSize, kVRangeKernelThreadStacks);
    else
        stack_range = memory_map->get_free_memory_range (kThreadStackSize, kVRangeUser);

    if (stack_range.size() == 0)
    {
        kerror ("process: no free memory for thread stack");
        return nullptr;
    }

    stack = memory_map->create_memory_region();

    stack->set_name ("thread stack");
    stack->set_privileged (true);
    stack->set_range (stack_range);
    stack->set_initial_size (stack_range.size());
    stack->allocate_and_map();

    char* stack_pointer = ( (char*) stack_range.end_addr()) - sizeof (uintptr_t);

    /* Populate initial values on the stack. */

    /* The SavedProcessContext contains all the values our interrupt
     * handler stub expects to exist. */
    stack_pointer -= sizeof (SavedProcessContext);
    SavedProcessContext* spc = (SavedProcessContext*) stack_pointer;
    spc->ebx = 0xdeadbeef;

    spc->esp = stack_range.end_addr() - sizeof (uintptr_t);
    /* Put the return address for the interrupt handler stub end
     * routine on the stack too. */
    stack_pointer -= sizeof (uintptr_t);
    * (uint32*) stack_pointer = (uint32) interrupt_return;

    /* Now put the data required for the return from switch process onto
     * our new stack as well. */

    stack_pointer -= sizeof (SavedKernelContext);
    kdebug("esp should be " + kstring((uint32)stack_pointer)); 
    kcontext = (SavedKernelContext*) stack_pointer;

    /* As the process will be just starting we don't need to worry too
     * much about the contents of some of the registers. */
    kcontext->ebp = 0;
    kcontext->ebx = 0;
    kcontext->esi = 0;
    kcontext->edi = 0xbeef; // just for debugging
    kcontext->eip = (uintptr_t) magic_return_function;
    return spc;
}


