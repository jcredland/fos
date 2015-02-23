
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
    void reserve_range(uintptr_t start, uintptr_t size); 
    uintptr_t round_up_to_4k(uintptr_t address);
    int get_page_number_greater_or_equal(uint64_t address);
    /** Forces a page number to be marked as available. */
    void mark_available(int page_number); 
    /** Marks a page as in use. */
    void mark_used(int page_number); 

    uintptr_t get_highest_memory_address(); 
    void * get_internal_allocation(int bytes);
    void reserve_space_for_memory_allocation(); 

    void assert_page_boundary(uintptr_t ptr);

    static const int PAGE_SIZE = 4096;
    /* Stores memory allocations, one bit per page. Approximately 128k. A 
     * 1 means the page is in use. */
    /* @todo: only reserves enough for 1 gig. */
    char * memory_allocations;
    int memory_allocation_size; 

    uint64_t total_memory; 
    uint64_t available_memory;
    uintptr_t highest_address;

    const int NORMAL_MEMORY = 1; 
};

extern PhysicalMemoryManager pmem; 
