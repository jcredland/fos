
/* see ACPIspec40a.pdf */
struct MemoryMapEntry
{
    uint64_t base_addr; 
    uint64_t len; 
    uint32_t type; 
    unsigned : 1;  /* reserved. */
    unsigned AddressRangeNonVolatile : 1;
    unsigned AddressRangeSlowAccess : 1;
    unsigned AddressRangeErrorLog : 1; 
    unsigned : 28; /* reserved. */
} F_PACKED_STRUCT;

extern uint32_t ram_data_size;
extern MemoryMapEntry ram_data; 

class PhysicalMemoryManager
{
public:
    PhysicalMemoryManager();

    /** Displays memory debug information. */
    void print_debug(VgaDriver & vga);

    /** Allocate page requests a 4kb RAM page.  
     * A nullptr return suggests that the memory couldn't be found. */
    void * get_page();
    /** Free up some memory pages previously allocated with allocate_page. 
     */
    void free_page(void * pointer);

private:
    int get_page_number_greater_or_equal(uint64_t address);
    /** Forces a page number to be marked as available. */
    void mark_available(int page_number); 
    /** Marks a page as in use. */
    void mark_used(int page_number); 

    static const int PAGE_SIZE = 4096;
    /* Stores memory allocations, one bit per page. Approximately 128k. A 
     * 1 means the page is in use. */
    static constexpr int size = 0xFFFFFFFF / PAGE_SIZE / 8;
    char memory_allocations[size];

    uint64_t total_memory; 
    uint64_t available_memory;

    const int NORMAL_MEMORY = 1; 
};

extern PhysicalMemoryManager pmem; 
