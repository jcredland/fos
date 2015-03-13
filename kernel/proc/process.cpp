#include <proc/process.h>
#include <mem/process_memory_map.h>


Process::Process(Process & parent_process, const kstring & _name, uint16 pid_number, Type _type)
    :
        name(_name),
        pid(pid_number),
        entry_point(nullptr),
        state(ReadyToPlay),
        type(_type),
        stack_pointer(0),
        memory_map(nullptr),
        stack(nullptr)
{
    parent_pid = parent_process.pid;
}

Process::Process()
    :
        state(Invalid)
{}

Process::Process(ProcessMemoryMap * kernel_memory_map) 
    :
        name("infernal kernel"),
        pid(1), 
        entry_point(nullptr),
        state(Running),
        type(KernelProcess),
        stack_pointer(0), 
        memory_map(kernel_memory_map),
        stack(nullptr)
{}

void Process::create_thread_stack(size_t size_of_stack)
{
    if (size_of_stack == 0 || memory_map == nullptr)
    {
        kerror("process: problem with create_thread_stack."); 
        return; 
    }

    MemoryRange stack_range;
   
    stack_range = memory_map->get_free_memory_range(kThreadStackSize, kVRangeUser);
    stack = memory_map->create_memory_region(); 

    stack->set_name("thread stack"); 
    stack->set_privileged (true);
    stack->set_range (stack_range);
    stack->set_initial_size (stack_range.size()); 
    stack->allocate_and_map();

    stack_pointer = stack_range.end_addr() - sizeof(uintptr_t);
}
