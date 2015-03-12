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

        /** Returns a free MemoryRange, that can be used for configuring a memory region, with the 
         * size required and constrained to the specified MemoryRange. This function could do best
         * fit but currently does first fit. */
        MemoryRange get_free_memory_range(size_t size_required, const MemoryRange & constrained_within)
        {
            uintptr_t ptr = constrained_within.base_addr();
            MemoryRegion * c; 

            while (nullptr != (c = get_region_at_or_above(ptr)))
            {
                if ((c->base_addr() - ptr) >= size_required)
                {
                    uintptr_t proposed_end_addr = ptr + size_required; 

                    if (proposed_end_addr < constrained_within.end_addr())
                        return MemoryRange {ptr, ptr + size_required}; 
                    else 
                        return MemoryRange::null;
                }
                ptr = c->end_addr(); 
            }
            return MemoryRange::null;
        }

        /** Because our array isn't sorted we use this horrible function.  It'd be nice to add allocator
         * selection to the vector implementation and then use some nice STL generic programming for this
         * task.  TODO over a coffee sometime. */
        MemoryRegion * get_region_at_or_above(uintptr_t more_than)
        {
            int region_number = -1;
            uintptr_t lowest_addr = 0xFFFFFFFF; 

            for (int i = 0; i < region_count; ++i)
            {
                uintptr_t addr = regions[i]->base_addr();
                if ((addr < lowest_addr) && (addr >= more_than))
                {
                    lowest_addr = addr; 
                    region_number = i;
                }
            }

            if (region_number != -1)
                return regions[region_number];
            else 
                return nullptr;
        }

        void display_debug() const;

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


