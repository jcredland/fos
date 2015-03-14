#include <proc/process.h>
#include <proc/proc_man.h>
#include <mem/process_memory_map.h>
#include <interrupt/stack_frame.h>

ProcessManager::ProcessManager()
    :
        current_pid(1),
        next_pid(2)
{
}

extern VirtualMemoryManager vmem; 

int ProcessManager::launch_kthread(void(*new_process_function)(void), const KString & name)
{
    Process t(proc_table[get_process_index(current_pid)], name, next_pid++, Process::kKernelThread);

    /* For threads, we just take a reference to the existing process map.*/
    t.memory_map = proc_table[get_process_index(current_pid)].memory_map;

    SavedProcessContext * spc = t.create_thread_stack(kThreadStackSize); 
    /* Set up the registers for when the thread starts running after our IRET 
     * Kernel segment pointer is 0x10. */
    spc->ds = 0x10;
    spc->es = 0x10;
    spc->fs = 0x10;
    spc->gs = 0x10;
    spc->cs = 0x8;
    spc->eflags = 0x200; /* IF flag. */

    spc->eip = reinterpret_cast<uintptr_t>(new_process_function); 

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
    if (!scheduler_ready) 
        return; 

    if (proc_table.size() == 0)
        return;

    current_process().state = Process::kRunnable;
    switch_to_scheduler(); 
}

void ProcessManager::start_scheduler()
{
    while (1)
    {
        for (auto p: proc_table)
        {
            if (p.state == Process::kRunnable)
            {
                p.state = Process::kRunning;
                current_pid = p.pid;
                /* This swaps the stack to one set up for a process, where the 
                 * return pointer on the stack points either to after switch_conext in
                 * switch_to_scheduler or, for new processes, straight into the
                 * magic_return_function.  
                 *
                 * Either way, the next things that happen are an IRET call and the 
                 * process runs for a while.
                 *
                 * Note that the context variable points to the top of the stack 
                 * in the new process (where the first thing on that stack is the
                 * SavedKernelContext).
                 */
                scheduler_ready = true;
                switch_kernel_context(p.kcontext, &scheduler_context); 
            }
        }
    }
}

Process & ProcessManager::current_process()
{
    return proc_table[get_process_index(current_pid)];
}

void ProcessManager::switch_to_scheduler()
{
    /* XV6 makes a point of doing lots of error checking here. */

    /* We need to save the state of interrupt-enablement here. */

    /* Switch from the processes context to the scheduler. Next instruction
     * to be executed will be in the scheduler function directly after its 
     * switch_kernel_context call */
    switch_kernel_context(scheduler_context, &(current_process().kcontext));
    /* And restore it for when the process runs again. */
}


void magic_return_function()
{
    kdebug("magic_return_function: passing through"); 
    /* The next hop should be the interrupt_return symbol where we pop off the 
     * new context. */
}

extern VirtualMemoryManager vmem;

void ProcessManager::setup_initial_kernel_thread(void(*entry_point)(void))
{
    kdebug("setup_initial_kernel_thread: started"); 

    Process t; 

    t.memory_map = vmem.get_kernel_memory_map();
    t.state = Process::kRunnable; 
    t.type = Process::kKernelThread;
    t.name = "init";
    t.pid = next_pid++;

    SavedProcessContext * spc = t.create_thread_stack(kThreadStackSize); 

    spc->ds = 0x10;
    spc->es = 0x10;
    spc->fs = 0x10;
    spc->gs = 0x10;
    spc->cs = 0x8;
    spc->eflags = 0x200; /* IF flag. */

    spc->eip = reinterpret_cast<uintptr_t>(entry_point);

    proc_table.push_back(t); 
}

