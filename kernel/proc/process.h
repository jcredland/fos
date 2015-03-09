#include <klibrary/klibrary.h>

struct Process
{
    KString name;
    uint16 number; 
    void(*entry_point)(void);

    enum State
    {
        ReadyToPlay,
        Running,
        Finished
    };

    State state;
};

class ProcessTable
{
    public:
        ProcessTable(); 
        /** Start a new kernel thread. This function puts the new thread in queue and it'll 
         * normally be started by the scheduler within a few 10s of milliseconds. */
        int launch_kthread(void(*new_process_function)(void), const KString & thread_name);

    private:
        kstd::kvector<Process> proc_table;
        uint16 next_number;
};

extern "C" 
{
    void switch_process(const uint32 stack_ptr_to_load, uint32 * stack_ptr_save_location);
};
