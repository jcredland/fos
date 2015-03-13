#include <proc/process.h>
#include <mem/process_memory_map.h>

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

ProcessManager::ProcessManager()
    :
        current_pid(1),
        next_pid(2)
{
    add_kernel_process(); 
}

extern VirtualMemoryManager vmem; 

void ProcessManager::add_kernel_process()
{
    Process kernel_process(vmem.get_kernel_memory_map()); 
    proc_table.push_back(kernel_process); 
}

int ProcessManager::launch_kthread(void(*new_process_function)(void), const KString & name)
{
    Process t(proc_table[get_process_index(current_pid)], name, next_pid++, Process::Thread);

    t.entry_point = new_process_function;
    t.name = name; 

    /* For threads, we just take a reference to the existing process map.*/
    t.memory_map = proc_table[get_process_index(current_pid)].memory_map;

    t.create_thread_stack(kThreadStackSize); 

    proc_table.push_back(t); 
    
    return t.pid; 
}

int ProcessManager::get_process_index(uint16 pid) const
{
    size_t end = proc_table.size(); 
    for (size_t i = 0; i < end; ++i)
    {
        if (proc_table[i].pid == pid)
            return (int) i;
    }
    return -1;
}

void ProcessManager::timer_interrupt()
{
    if (proc_table.size() == 0)
        return;

    /* On each interrupt tick move to the next process in the queue to run,
     * load the processes stack and return. If there's a new process then
     * call that process. */
    uint16 old_proc_index = process_sequence;

    process_sequence++;

    if (process_sequence >= proc_table.size())
        process_sequence = 0; 

    Process * p = &proc_table[process_sequence];

    if (p->state == Process::ReadyToPlay)
    {
        p->state = Process::Running;
        p->entry_point();
    }
    else if (p->state == Process::Running)
    {
        switch_process((uint32 *) &p->stack_pointer, 
                (uint32 *) &(proc_table[old_proc_index].stack_pointer));
    }
}

