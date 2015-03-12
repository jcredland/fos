#include <klibrary/klibrary.h>
#include <mem/mem.h>

struct Process
{
    enum Type
    {
        Thread, 
        UserProcess,
        KernelProcess
    };
    Process(Process & parent_process, const kstring & _name, uint16 pid_number, Type _type)
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

    kstring name;
    /** PIDs start at 1.  PID 0 means no parent. */
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

    Type type;

    uintptr_t stack_pointer;

    ProcessMemoryMap * memory_map; 

    bool is_void() const { return state == Invalid; }
    void create_thread_stack(size_t size);

    Process()
        :
            state(Invalid)
    {}


private:
    MemoryRegion * stack; 

    friend class ProcessTable;
    /** Construct the root kernel process. */
    Process(ProcessMemoryMap * kernel_memory_map) 
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
        void add_kernel_process(); 
        int get_process_index(uint16 pid) const;
        kstd::kvector<Process> proc_table;
        uint16 current_pid;
        uint16 next_pid;
        Process invalid_process;
};

extern "C" 
{
    void switch_process(const uint32 stack_ptr_to_load, uint32 * stack_ptr_save_location);
};
