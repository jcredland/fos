#include <std_headers.h>
#include <mem/process_memory_map.h>

void ProcessMemoryMap::display_debug() const
{
    kdebug("region count: " + KString((uint8) region_count)); 

    for (int i = 0; i < region_count; ++i)
        regions[i]->display_debug(); 
}

