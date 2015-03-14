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
            vmm_heap(memory_pool_for_regions),
            regions(vmm_heap)
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

            regions.insert_sorted(new_region, 
                    [](const MemoryRegion * a, const MemoryRegion * b) 
                    { 
                        return a->base_addr() > b->base_addr();  /* Assumes regions don't overlap */
                    });

            return  new_region;
        }

        /** Returns a free MemoryRange, that can be used for configuring a memory region, 
         * with the size required and constrained to the specified MemoryRange. This 
         * function could do best fit but currently does first fit. */
        MemoryRange get_free_memory_range(size_t size_required, 
                                          const MemoryRange & constrained_within)
        {
            uintptr_t start_of_space = constrained_within.base_addr();
            uintptr_t end_of_space; 
            for (auto r: regions)
            {
                end_of_space = min(r->base_addr(), constrained_within.end_addr()); 

                kdebug("considering space from : " + kstring(start_of_space) + " to " + kstring(end_of_space)); 

                if (start_of_space < end_of_space && (end_of_space - start_of_space) >= size_required)
                    return MemoryRange { start_of_space, start_of_space + size_required }; 

                start_of_space = max(start_of_space, r->end_addr()); 
            }
            /* Consider the space between the end of the last region and the end of the possible space. */
            end_of_space = constrained_within.end_addr(); 
                
            kdebug("considering space from : " + kstring(start_of_space) + " to " + kstring(end_of_space)); 

            if (start_of_space < end_of_space && (end_of_space - start_of_space) >= size_required)
                return MemoryRange { start_of_space, start_of_space + size_required }; 
            else
                return MemoryRange::null;
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
        SortedList<MemoryRegion *, MemoryPool<MemoryRange> > regions;
        int region_count; 
};


