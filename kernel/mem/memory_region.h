#pragma once
#include <klibrary/klibrary.h>
#include <mem/physical.h>

/** 
 * A virtual memory region. 
 *
 * Regions can be copied carefully, when copying we need to decide whether to 
 * reduce the size to zero, to enable copy-on-write or to copy the ram.  So 
 * all the default copy constructors are deleted and specific functions must be
 * used.
 *
 * Regions are owned by ProcessMemoryMaps.
 *
 * Regions can be expanded and track their current size.
 *
 */
class ProcessMemoryMap;
class IntelPageMapper;

class MemoryRegion
{
    public:
        MemoryRegion(ProcessMemoryMap * owner); 

        void set_range(const MemoryRange & r)
        {
            range = r;
        }

        MemoryRegion(const MemoryRegion & rhs) = delete;
        MemoryRegion & operator=(const MemoryRegion &) = delete;

        KString to_debug_string();

        void set_initial_size(uintptr_t size) { current_size = size; }
        void set_privileged(bool is_privileged) { privileged = is_privileged; }
        void set_name(const KString & new_name) { name = new_name; }

        uintptr_t base_addr() const { return range.base; }
        uintptr_t size() const { return current_size; }

        /** Allocate physical memory to this region and map it 
         * into virtual memory. */
        void allocate_and_map();
        /** Identity map physical memory into this region. */ 
        void apply_identity_mapping();
        /** Sets the is_mapped flag to true without actually attempting to 
         * allocate any memory. */
        void set_reserve_only(); 

        MemoryRegion * clone_as_copy_on_write(ProcessMemoryMap * new_owner); 
        MemoryRegion * clone_with_zero_size(ProcessMemoryMap * new_owner); 
    private:
        friend class Allocator; 

        MemoryRange range; 
        uintptr_t current_size;
        bool mapped; 
        bool privileged; 
        KString name; 

        ProcessMemoryMap * owner; 
        IntelPageMapper * mapper;
};

