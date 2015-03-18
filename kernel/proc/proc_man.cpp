#include <proc/process.h>
#include <proc/proc_man.h>
#include <mem/process_memory_map.h>
#include <interrupt/stack_frame.h>
#include <klibrary/locks.h>

ProcessManager::ProcessManager()
    :
        current_pid(1),
        next_pid(2)
{
}

extern VirtualMemoryManager vmem; 

void set_kernel_segment_registers(SavedProcessContext * spc)
{
    spc->ds = 0x10;
    spc->es = 0x10;
    spc->fs = 0x10;
    spc->gs = 0x10;
    spc->cs = 0x8;
    spc->eflags = 0x200; /* IF flag. */
};

int ProcessManager::launch_kthread(void(*new_process_function)(void), const KString & name)
{
    ScopedSpinLock lock(proc_table_lock); 

    Process t(proc_table[get_process_index(current_pid)], name, next_pid++, Process::kKernelThread);

    /* For threads, we just take a reference to the existing process map.*/
    t.memory_map = proc_table[get_process_index(current_pid)].memory_map;

    SavedProcessContext * spc = t.create_thread_stack(kThreadStackSize); 
    /* Set up the registers for when the thread starts running after our IRET 
     * Kernel segment pointer is 0x10. */
    set_kernel_segment_registers(spc);

    spc->eip = reinterpret_cast<uintptr_t>(new_process_function); 

    proc_table.push_back(t); 
    
    return t.pid; 
}

void ProcessManager::setup_initial_kernel_thread(void(*entry_point)(void))
{
    kdebug("setup_initial_kernel_thread: started"); 

    Process t; 

    t.memory_map = vmem.get_kernel_memory_map();
    t.state = Process::kRunnable; 
    t.type = Process::kKernelThread;
    t.name = "init";

    SavedProcessContext * spc = t.create_thread_stack(kThreadStackSize); 
    set_kernel_segment_registers(spc);

    spc->eip = reinterpret_cast<uintptr_t>(entry_point);

    {
        ScopedSpinLock lock(proc_table_lock); 

        t.pid = next_pid++;
        proc_table.push_back(t); 
    }
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

    if (proc_table_lock.is_locked())
        kpanic("process table was locked on entering interrupt!");

    proc_table_lock.lock(); 
    current_process().state = Process::kRunnable;
    switch_to_scheduler(); 
    proc_table_lock.release(); 
}


void ProcessManager::kill(uint16 pid)
{
    ScopedSpinLock lock(proc_table_lock); 

    int idx = get_process_index(pid); 
    
    p.state = Process::kKilled; 

    /* TODO - make this interesting and useful function work. */
}

void ProcessManager::start_scheduler()
{
    while (1)
    {
        proc_table_lock.lock(); 

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

                if (! proc_table_lock.is_locked())
                    kdebug("!");
            }
        }
        proc_table_lock.release(); 
    }
}

Process & ProcessManager::current_process()
{
    kassert(proc_table_lock.is_locked()); 
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

void ProcessManager::release_lock()
{
    proc_table_lock.release(); 
}

extern ProcessManager procman; 

void magic_return_function()
{
    procman.release_lock(); 
    /* The next hop should be the interrupt_return symbol where we pop off the 
     * new context. */
}

bool ProcessManager::is_command_supported(const kstring & cmd) 
{
    return (cmd == "ps");
}

int ProcessManager::execute_cli_command(const kvector<kstring> &) 
{
    ScopedSpinLock lock(proc_table_lock); 

    for (auto p: proc_table)
        kdebug(kstring((uint16) p.pid) + " " + p.name); 

    return 0; 
}



