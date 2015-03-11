#include <proc/process.h>

ProcessTable::ProcessTable()
    :
        current_pid(0),
        next_number(1)
{
    add_kernel_process(); 
}

void ProcessTable::add_kernel_process()
{
    Process p; 

    p.entry_point = nullptr;
    p.name = "kernel";
    p.pid = next_pid++;
    p.parent_pid = 0xFFFF;
    p.state = Process::Running;
    p.type = Process;
    p.memory_map = vmm.get_kernel_memory_map(); 
}

int ProcessTable::launch_kthread(void(*new_process_function)(void), const KString & name)
{
    Process t;

    t.entry_point = new_process_function;
    t.name = name; 
    t.pid = next_pid++;
    t.parent_pid = current_pid;
    t.state = Process::ReadyToPlay;

    /* For threads, we just take a reference to the existing process map.*/
    t.memory_map = proc_table[get_process_index(current_pid)].memory_map;

    MemoryRange stack_range;
   
    stack_range = t.memory_map->get_free_memory_range(kThreadStackSize, kVRangeUser);
    stack = t.memory_map->create_memory_region(); 

    t.memory_map->set_name("thread stack"); 
    t.memory_map->set_privileged (true);
    t.memory_map->set_range (stack_range);
    t.memory_map->set_initial_size (stack_range.size()); 
    t.memory_map->allocate_and_map();

    t.stack_pointer = stack_range.end_addr() - sizeof(uintptr_t);

    proc_table.push_back(t); 
    
    return new_process.number; 
}

int ProcessTable::get_process_index(uint16 pid) const
{
    size_t end = proc_table.size(); 
    for (size_t = 0; i < end; ++i)
    {
        if (proc_table[i].pid == pid)
            return (int) i;
    }
    return -1;
}
