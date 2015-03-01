
/** @file Kernel memory allocation routines. 
 *
 * @class MemoryPool provides a memory block directly from physical memory with 
 * malloc and free functions to allow it ot be subdivided. 
 * */
template <int size_to_allocate_in_4kb_pages, int alignment = 16>
class MemoryPool
{
public:
    MemoryPool()
    {
        memory_pool = (char*) pmem.get_multiple_4k_pages(size_to_allocate_in_4kb_pages);
        memory_pool_size = 4096 * size_to_allocate_in_4kb_pages;

        FreeListNode * first_node = (FreeListNode *) memory_pool; 
        first_node->size = memory_pool_size;
        first_node->next = nullptr;
    }

    ~MemoryPool()
    {
        pmem.free_multiple_4k_pages(memory_pool, size_to_allocate_in_4kb_pages); 
    }
    /** 
     * Allocate memory.  The memory will not be initialized. 
     *
     * The minimum size actually allocated is at least twice the allocation 
     * alignment requirement. 
     *
     * @returns nullptr on failure. 
     * */
    void * malloc(size_t size_required)
    {
        /* Fix up the size we need to include an aligned header and 
         * ensure that the block ends on an aligned boundary. */
        size_required += block_header_size;
        size_required = align_up(size_required); 

        /* Find some free space big enough. There are different strategies possible here.
         * This one is the first-fit strategy which is easy. See also 'best-fit'.*/
        FreeListNode * node = free_list_start;
        FreeListNode * previous_node = nullptr;

        while (node->size < size_required)
        {
            previous_node = node;
            node = node->next;

            if (node == nullptr)
            {
                kerror("malloc: shucks, no space.");
                return nullptr; 
            }
        }

        /* Now create a new free node entry and remove the old one from the list. 
         * Do the pointer arithmetic in bytes by casting to a uintptr_t. */
        uintptr_t base_addr_of_space = static_cast<uintptr_t>(node); 
        FreeListNode * new_node = base_addr_of_space + size_required; 

        new_node->next = node->next;
        new_node->size = node->size - size_required;

        if (previous_node)
            previous_node->next = new_node;
        else
            free_list_start = new_node;

        /* And write a new header to the start of our free space with size information
         * and a security / error checking flag. */
        BlockHeader * header = (BlockHeader *) node;
        header->canary_flag = kCanaryFlagValue;
        header->size = size_required; 
        return (void*)(static_cast<uintptr_t>(header) + block_header_size);
    }


    void free(void * address_to_free)
    {
        BlockHeader * header = static_cast<uintptr_t>(address_to_free) - block_header_size;
        /* Save the size as we will overwrite the BlockHeader with a new FreeListNode. */
        size_t size_of_block = header->size_including_header;

        if (header->canary_flog != kCanaryFlagValue)
            kerror("free: invalid canary flag.  programming error."); 

        /* Find a location in the free list where we should insert this block. We keep the 
         * free list sorted so we can merge free blocks together easily. */
        FreeListNode * node = free_list_start;
        FreeListNode * previous_node = nullptr;

        while ((node < header) && (node != nullptr))
        {
            previous_node = node;
            node = node->next;
        }

        /* Insert the block. */
        FreeListNode * new_node = header; 
        new_node->size = size_of_block;
        new_node->next = node;

        if (previous_node == nullptr)
            free_list_start = new_node; 
        else
            previous_node->next = new_node;

        /* Collapse neighbouring blocks on the free list. This can be optimised 
         * later by not writing the original block. TODO!! */
    }

    size_t size_free();
private:
    struct FreeListNode
    {
        size_t size;
        FreeListNode * next;
    };

    struct BlockHeader
    {
        uint16_t canary_flag; 
        size_t size_including_header;
    };

    /* This could be based on a random number generator to 
     * better detect malicious heap overflows. */
    const uint16 kCanaryFlagValue = 0xF1A6;

    constexpr size_t align_up(size_t size) 
    {
        return (size + alignment_minus_one) & alignment_mask;
    }

    constexpr static size_t block_header_size = align_up(sizeof(BlockHeader));
    constexpr static size_t alignment_minus_one = alignment - 1;
    constexpr static size_t alignment_mask = ~ alignment_minus_one;
    char * memory_pool;
    size_t memory_pool_size;
    FreeListNode * free_list_start = nullptr;
};

