#pragma once
#include <klibrary/klibrary.h>
#include <mem/mem.h>
#include <proc/process.h>
#include <klibrary/locks.h>
#include <cli/cli.h>

/** Management of threads and processes.  This includes scheduling and the table
 * of running proceses.
 */
class ProcessManager
:
    public CliCommand
{
    public:
        ProcessManager();

        /** Start-up the scheduler. */
        void start_scheduler();

        /** Start a new kernel thread. This function puts the new thread in queue and it'll
         * normally be started by the scheduler within a few 10s of milliseconds. */
        int launch_kthread(void(*new_process_function)(void), const KString & thread_name);

        /** Stop a thread or process (and release its associated memory). */
        void kill(uint16 pid); 

        /** Called on each timer tick. May not return normally. */
        void timer_interrupt();

        /* Sets up the first process for the OS to run. */
        void setup_initial_kernel_thread(void(*entry_point)(void));

        /** Only called from the magic return function. */
        void release_lock(); 

        /* Cli commands. */
        bool is_command_supported(const kstring &) override;
        int execute_cli_command(const kvector<kstring> &) override;

    private:
        /** This function is called when a process needs to stop running and switch back
         * to the scheduler.  The process will eventually return from this function when
         * it's time comes around again. */
        void switch_to_scheduler(); 
        /** Get the PID of the process currently executing. */
        int get_current_pid();

        void add_kernel_process();

        int get_process_index(uint16 pid) const;
        Process & current_process();

        /** This is the all important table of running processes. */
        kvector<Process> proc_table;
        SpinLock proc_table_lock;

        uint16 current_pid;
        uint16 next_pid;
        Process invalid_process;

        bool scheduler_ready = false;

        SavedKernelContext * scheduler_context;
};

extern "C"
{
    void switch_kernel_context(SavedKernelContext * new_stack, 
                               SavedKernelContext ** old_stack_save);
    /** This function carrys out any necessary changes after we've switched to our 
     * new context and before we return to the start of the a new process. */
    void magic_return_function(); 
};

