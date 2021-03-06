#pragma once
#include <mem/memory_region.h>
#include <cli/cli.h>

/** Kernel memory allocation functions. 
 *
 * @param takes a MemoryRegion to use for storage.
 */
template <typename Allocation>
class MemoryPool
:
    public CliCommand
{
public:
    /** Allocation is a type that has the following functions: 
     * base_addr() 
     * size()
     */
    MemoryPool(const Allocation & region_to_use)
    {
        memory_pool = (char*) region_to_use.base_addr(); 

        kdebug("memorypool: constructing with start_addr: " + KString((uint32) memory_pool)); 

        memory_pool_size = region_to_use.size();

        FreeListNode * first_node = (FreeListNode *) memory_pool; 
        first_node->size = memory_pool_size;
        first_node->next = nullptr;

        free_list_start = first_node;
    }

    ~MemoryPool()
    {
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
        kassert(size_required > 0); 
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
        uintptr_t base_addr_of_space = reinterpret_cast<uintptr_t>(node); 
        FreeListNode * new_node = (FreeListNode *) (base_addr_of_space + size_required); 

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
        header->size_including_header = size_required; 
        return (void*)(reinterpret_cast<uintptr_t>(header) + block_header_size);
    }


    void free(void * address_to_free)
    {
        if (address_to_free == nullptr)
            return;

        BlockHeader * header = (BlockHeader *)(reinterpret_cast<uintptr_t>(address_to_free) - block_header_size);
        /* Save the size as we will overwrite the BlockHeader with a new FreeListNode. */
        size_t size_of_block = header->size_including_header;

        if (header->canary_flag != kCanaryFlagValue)
            kerror("free: invalid canary flag.  programming error."); 

        /* Find a location in the free list where we should insert this block. We keep the 
         * free list sorted so we can merge free blocks together easily. */
        FreeListNode * node = free_list_start;
        FreeListNode * previous_node = nullptr;

        while ((node < (FreeListNode *)header) && (node != nullptr))
        {
            previous_node = node;
            node = node->next;
        }

        /* Insert the block. */
        FreeListNode * new_node = reinterpret_cast<FreeListNode*>(header); 
        new_node->size = size_of_block;
        new_node->next = node;

        if (previous_node == nullptr)
            free_list_start = new_node; 
        else
            previous_node->next = new_node;

        /* Collapse neighbouring blocks on the free list. This can be optimised 
         * later by not writing the original block. TODO!! */

        if (node != nullptr) 
        {
            /* Collapse upward. */
            if (uintptr_t(new_node) + new_node->size == (uintptr_t) node)
            {
                new_node->size += node->size;
                new_node->next = node->next;
            }
        }
        if (previous_node != nullptr)
        {
            /* Collapse downward. */
            if (uintptr_t(previous_node) + previous_node->size == (uintptr_t) new_node)
            {
                previous_node->size += new_node->size;
                previous_node->next = new_node->next;
            }
        }
    }

    /** Returns the total amount of free space.  This is not necessarily in a nice
     * continuous block, so memory allocations less than this amount are still likely
     * to fail.  But it may be useful for debugging.  It's also not terribly fast. 
     * @see size_largest_free_block */
    size_t size_free()
    {
        FreeListNode * n = free_list_start;
        size_t space_free = 0; 

        while (n != nullptr)
        {
            space_free += n->size;
            n = n->next;
        }
        return space_free;
    }

    size_t size_largest_free_block()
    {
        FreeListNode * n = free_list_start;
        size_t largest_block = 0;

        while (n != nullptr)
        {
            if (n->size > largest_block)
                largest_block = n->size;
            n = n->next;
        }
        return largest_block;
    }

    int count_of_free_blocks()
    {
        int count = 0;
        FreeListNode * n = free_list_start;
        //size_t largest_block = 0;

        while (n != nullptr)
        {
            count++;
            n = n->next;
        }
        return count;
    }

    void kprint_debug()
    {
        kdebug("kheap: size               :" + KString((uint32) memory_pool_size));
        kdebug("kheap: largest free block :" + KString(size_largest_free_block()));
        kdebug("kheap: total free space   :" + KString(size_free()));
        kdebug("kheap: free block count   :" + KString((uint32) count_of_free_blocks()));
    };

    
    bool is_command_supported(const kstring & cmd) override
    {
        return cmd == "kheap"; 
    }

    int execute_cli_command(const kstd::kvector<kstring> &) override
    {
        kprint_debug(); 
        return 0; 
    }

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

    constexpr static size_t alignment = 16;

    constexpr static size_t align_up(size_t size) 
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

    
