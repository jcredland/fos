
#include <klibrary/klibrary.h>
#include <mem/intel_page_mapper.h>
#include <mem/process_memory_map.h>
#include <mem/memory_region.h>

MemoryRegion::MemoryRegion(ProcessMemoryMap * _owner)
    :
        current_size(0),
        mapped(false),
        privileged(false)
{
    owner = _owner; 
    mapper = owner->get_page_mapper(); 
}

void MemoryRegion::allocate_and_map()
{
    if (mapped)
    {
        kdebug("memoryregion: already mapped"); 
        return;
    }

    uintptr_t e = current_size + range.base;
    kdebug("memoryregion: mapping " + KString(range.base)); 
    kdebug("memoryregion: current size " + KString(current_size)); 

    for (uintptr_t virt_addr = range.base; virt_addr < e; virt_addr += kPageSize)
    {
        uintptr_t phys_addr = (uintptr_t) pmem.get_4k_page(kPRangeGeneral); 
        
        if (phys_addr == 0)
            kerror("memory allocation failed.");  /* TODO this needs better handling. */

        mapper->map_page(virt_addr, phys_addr); 
    }

    mapped = true;
}

void MemoryRegion::apply_identity_mapping()
{
    kassert(! mapped); 

    for (uintptr_t addr = range.base; 
            addr < range.end; 
            addr += kPageSize)
        mapper->map_page(addr, addr); 

    mapped = true;
}

void MemoryRegion::set_reserve_only()
{
    kassert(! mapped); 
    mapped = true;
}

void MemoryRegion::display_debug() const
{
    kdebug(name + " " + range.to_string() + " current size: " + KString((uint32) current_size));
}
