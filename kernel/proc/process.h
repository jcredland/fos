#include <klibrary/klibrary.h>

struct Process
{
    Process()
        :
            number(0),
            entry_point(nullptr),
            state(Invalid),
            stack_pointer(0),
            stack(nullptr),
            memory_map(nullptr)
    {}
    KString name;
    uint16 pid; 
    uint16 parent_pid;
    void(*entry_point)(void);

    enum State
    {
        Invalid,
        ReadyToPlay,
        Running,
        Finished
    } state;

    enum Type
    {
        Thread, 
        UserProcess,
        KernelProcess
    } type;

    uintptr_t stack_pointer;

    MemoryRegion * stack; /* Used for threads. */
    ProcessMemoryMap * memory_map; /* Used for processes. */

    bool is_void() const { return state == Invalid; }
};

class ProcessTable
{
    public:
        ProcessTable(); 
        /** Start a new kernel thread. This function puts the new thread in queue and it'll 
         * normally be started by the scheduler within a few 10s of milliseconds. */
        int launch_kthread(void(*new_process_function)(void), const KString & thread_name);
        int get_current_pid(); 

    private:
        Process get_process(uint16 pid);
        kstd::kvector<Process> proc_table;
        uint16 current_pid;
        uint16 next_pid;
        Process invalid_process;
};

extern "C" 
{
    void switch_process(const uint32 stack_ptr_to_load, uint32 * stack_ptr_save_location);
};
