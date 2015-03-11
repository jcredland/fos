#include <klibrary/klibrary.h>
#include <mem/vmm.h>


const unsigned kVMMHeapPages = 64;
VirtualMemoryManager::VirtualMemoryManager()
    :
        vmm_heap_range(pmem.get_multiple_4k_pages (kPRangePageDirectory, kVMMHeapPages)),
        vmm_heap (vmm_heap_range)
{
    /* It'd be nice to introduce a full memory test whenever we setup a heap, where
     * we create blocks of various sizes, fill them with a a block specific number,
     * randomly delete and recreate other blocks and make sure we can read back the
     * correct numbers each time. */

    kdebug ("vmm: start up");

    build_kernel_memory_map();
    kernel_memory_map->set_active();
    kernel_memory_map->enable_paging();
}

void VirtualMemoryManager::build_kernel_memory_map()
{
    kernel_memory_map = create_process_memory_map();

    {
        MemoryRegion* r = kernel_memory_map->create_memory_region();
        r->set_range (kIdentityMappedRange);
        r->set_privileged (true);
        r->apply_identity_mapping(); 
    }

    {
        MemoryRegion* r = kernel_memory_map->create_memory_region();
        r->set_range (kVRangeKernelHeap);
        r->set_initial_size (kVRangeKernelHeap.size());
        r->set_privileged (true);
        r->allocate_and_map();

        kernel_heap_region = r;
    }
}

ProcessMemoryMap* VirtualMemoryManager::create_process_memory_map()
{
    kdebug ("vmm: creating process map");
    ProcessMemoryMap* new_pmm = (ProcessMemoryMap*) vmm_heap.malloc (sizeof (ProcessMemoryMap));
    new_pmm = new (new_pmm) ProcessMemoryMap(vmm_heap);
    apply_default_configuration (new_pmm);
    return new_pmm;
}

void VirtualMemoryManager::destroy_process_memory_map (ProcessMemoryMap* map)
{
    kdebug ("vmm: destroying process map");
    map->~ProcessMemoryMap();
    vmm_heap.free (map);
}


void VirtualMemoryManager::apply_default_configuration (ProcessMemoryMap* pmm)
{
}

bool VirtualMemoryManager::is_command_supported (const KString& command)
{
    return (command == "vmem");
}

int VirtualMemoryManager::execute_cli_command (const kstd::kvector<KString>& parameter_list)
{
    vga.write ("vmm test and info");
    return 0;
}

MemoryRegion* VirtualMemoryManager::get_kernel_heap_region()
{
    return kernel_heap_region;
}


