
#include <mem/kmalloc.h>

/** What do we need to test.  
 * - Blocks do not overlap and are the size requested. 
 * - Blocks are properly freed. 
 * - Some view on the performance. 
 * - That memory full is detected correctly. 
 * - That with random allocation and deallocation we don't get any corruption. 
 *
 * Once this is sorted we can implement kvector which may be very useful. 
 */
void kmalloc_test_memory_pool()
{
    /* a 32k MemoryPool. */
    MemoryPool<MemoryRange> pool(pmem.get_multiple_4k_pages(kPRangeGeneral, 32));
    const int kBlockCount = 85; 
    const int kBlockMultiplier = 40; 
    char * blocks[kBlockCount];

    pool.kprint_debug();

    for (int i = 1; i < kBlockCount; i++)
    {
        size_t block_size = kBlockMultiplier * i; 
        blocks[i] = (char*) pool.malloc(block_size);

        for (size_t j = 0; j < block_size; ++j)
            *(blocks[i] + j) = i; 
    }

    pool.free((void *) blocks[2]); 
    pool.free((void *) blocks[40]); 
    pool.free((void *) blocks[41]); 
    pool.free((void *) blocks[42]); 
    pool.free((void *) blocks[43]); 

    size_t biggest_block_size = pool.size_largest_free_block();

    pool.kprint_debug();

    pool.free((void *) blocks[44]); 

    kassert(pool.size_largest_free_block() > biggest_block_size); 

}

