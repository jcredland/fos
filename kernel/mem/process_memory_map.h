#pragma once
#include <klibrary/klibrary.h>
#include <mem/physical.h>
#include <mem/memory_region.h>
#include <mem/kmalloc.h>
#include <mem/intel_page_mapper.h>

/**
 * A ProcessMemoryMap is created for each process on the system.
 *
 * Each instance owns a Page Directory which is located in the kPRangePageDirectory
 * MemoryRange. 
 *
 * The ProcessMemoryMap itself cannot call our standard allocator, and is probably 
 * safest in identity mapped memory space. 
 */
class ProcessMemoryMap
{
    public:
        ProcessMemoryMap(MemoryPool<MemoryRange> & memory_pool_for_regions)
            :
            vmm_heap(memory_pool_for_regions)
        {}

        void set_active()
        {
            page_mapper.set_active();
        }

        void enable_paging()
        {
            page_mapper.enable_paging();
        }

        IntelPageMapper * get_page_mapper()
        { 
            return &page_mapper; 
        }

        enum {
            kMaxRegions = 16
        };

        MemoryRegion* create_memory_region()
        {
            kdebug ("pmap: creating memory region");
            MemoryRegion* new_region = (MemoryRegion*) vmm_heap.malloc (sizeof (MemoryRegion));
            new_region = new (new_region) MemoryRegion(this);

            if (region_count < kMaxRegions)
                regions[region_count++] = new_region;
            else
                kerror("regions full - need to put kvector in."); 

            return  new_region;
        }

    private:
        void destroy_memory_region(MemoryRegion * r)
        {
            r->~MemoryRegion();
            vmm_heap.free(r); 
        }

        MemoryPool<MemoryRange> & vmm_heap;
        IntelPageMapper page_mapper;
        MemoryRegion * regions[kMaxRegions];
        int region_count; 
};
