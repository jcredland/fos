#include <mem/mem_layout.h>

kstring MemoryRange::to_string() const
{
    return kstring((uint32)base) + ":" + kstring((uint32)end);
}

/* This will be a function to convert memory_size into a nicely formatted 
 * string, e.g. 1.2kb 102.2Mb.
 *
 * We might need to implement snprintf, or a C++ equivalent with streams 
 * first through.
 */
kstring mem_size_to_string(size_t memory_size)
{

}
        

const MemoryRange MemoryRange::null {0x0, 0x0};

