#pragma once

#include <klibrary/klibrary.h>
#include <mem/physical.h>
#include <mem/kmalloc.h>
#include <mem/process_memory_map.h>
#include <mem/intel_page_mapper.h>
#include <cli/cli.h>

/**
 * VirtualMemoryManager
 *
 * Creates ProcessMemoryMaps, handles interrupts and provides debugging information. 
 *
 * On construction it also sets up an initial kernel process memory map, sets up the 
 * identity paging and enables virtual memory.
 */
class VirtualMemoryManager
:
    public CliCommand
{
    public:
        /** Start the VMM and switch to paged memory. */
        VirtualMemoryManager();
        /** Creates a new process memory map with its own PageDirectory and a 
         * default configuration. */
        ProcessMemoryMap * create_process_memory_map();
        /** And get rid of one. */
        void destroy_process_memory_map(ProcessMemoryMap *);
        /** CLI support. */
        bool is_command_supported(const KString & command);
        /** CLI support. */
        int execute_cli_command(const kstd::kvector<KString> & parameter_list);

        /** Returns the MemoryRegion for the kernel heap. */
        MemoryRegion * get_kernel_heap_region(); 
    private:
        void build_kernel_memory_map(); 
        void apply_default_configuration(ProcessMemoryMap *); 

        MemoryRange vmm_heap_range;
        MemoryPool<MemoryRange> vmm_heap;

        MemoryRegion * kernel_heap_region;
        ProcessMemoryMap * kernel_memory_map; 

        F_NO_COPY(VirtualMemoryManager)
};

