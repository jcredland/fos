#pragma once
#include <klibrary/klibrary.h>
#include <mem/mem.h>
#include <proc/process.h>

/** Management of threads and processes.  This includes scheduling and the table 
 * of running proceses. 
 */
class ProcessManager
{
    public:
        ProcessManager(); 

        /** Start-up the scheduler. */
        void start_scheduler(); 

        /** Start a new kernel thread. This function puts the new thread in queue and it'll 
         * normally be started by the scheduler within a few 10s of milliseconds. */
        int launch_kthread(void(*new_process_function)(void), const KString & thread_name);

        int get_current_pid(); 

        /** Called on each timer tick. May not return normally. */
        void timer_interrupt();

    private:
        void add_kernel_process(); 
        int get_process_index(uint16 pid) const;
        kvector<Process> proc_table;
        uint16 process_sequence; /**< The index into the proc_table for the currently running process. */
        uint16 current_pid;
        uint16 next_pid;
        Process invalid_process;
};

extern "C" 
{
    void switch_process(const uint32 * stack_ptr_to_load, uint32 * stack_ptr_save_location);
};

