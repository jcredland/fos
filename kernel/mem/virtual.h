#pragma once

#include <klibrary/klibrary.h>
#include <mem/physical.h>
#include <mem/kmalloc.h>
#include <mem/page_table.h>
#include <cli/cli.h>

class VirtualMemoryManager
:
    public CliCommand
{
    public:
        VirtualMemoryManager()
        {
            kdebug("vmm: start up"); 
            setup_fos_paging(); 
        }
        /**
         * This function sets up the paging to support FOS.  It needs to take
         * the following major actions. 
         *
         * Identify the memory used by the kernel, video memory and so on.
         *
         * Build some page tables to implement identity mapping for those pages.
         *
         * Set those with the privileged bits.
         *
         * We then need to be able to request pages from virtual memory for the 
         * kernels heap.  In fact we won't operate a heap until after paging is 
         * configured.  So we'll need a vmem manager to go with our pmem object
         * that handles the request for pages. 
         *
         * MemoryPool will need adjusting to talk to vmem instead of pmem.  Perhaps
         * it'd be useful to have a general interface for memory managers - it
         * may be useful later?? Is it possible to abstract the whole thing so
         * a process could get something on disk, in memory or over the network?
         */
        void setup_fos_paging()
        {
           /* Our kernel is currently in the bottom 640k of RAM below video 
            * memory. So we'll just allocate the bottom 2Mb as privileged 
            * mode only identity paging. 
            */ 
            for (uintptr_t addr = 0; addr < 0x200000; addr += 0x1000)
                paging_man.map_page(addr, addr); 

            paging_man.enable_paging(); 
        }

        bool is_command_supported(const KString & command)
        {
            return (command == "vmem");
        }

        int execute_cli_command(const kstd::kvector<KString> & parameter_list);

    private:
        IntelPagingManager paging_man; 
        MemoryPool<32> page_table_pool;
};


/** 
 * A class that holds information on memory allocations for a process. One of these
 * also manages the memory for the kernel.  
 *
 * Something like: 
 *  ProcessVirtualMemoryMap pvmm(phys_mem_allocator, paging_directory, 0x0000, 0x1000);
 *
 * And a couple of the page_tables point to the kernel.  These are referred to from 
 * all the different paging_directories. 
 *
 * 
 */

class MemoryRegion
{
    public:
        MemoryRegion(Type type, KString name, uintptr_t start_addr, uintptr_t end_addr);
        MemoryRegion(const MemoryRegion & rhs); 
        MemoryRegion & operator=(const MemoryRegion &); 

        enum Type
        {
            kRegionReserved,
            kRegionKernelHeap,
            kRegionUser
        };
    private:
        Type type;
        KString name; 
        uintptr_t start_addr;
        uintptr_t end_addr;
};

class PageMapConfiguration 
{
    int number_of_shared_pages; 
};

class ProcessVirtualMemoryManager
{
    public:
        /** Create a ProcessVirtualMemoryMap that's permitted to allocate memory for a virtual system. */
        ProcessVirtualMemoryMap(const ProcessVirtualMemoryMap & map_to_clone); 

        /** Create an initial ProcessVirtualMemoryMap setting up with a number of shared pages tables
         * in the lower end which will be replicated across all other cloned ProcessVirtualMemoryMap 
         * objects. Used for kernel mapping into the low memory addresses. */
        ProcessVirtualMemoryMap(int number_of_shared_page_tables); 

        void * get_4k_pages(MemoryRegion::Type region_to_use, int num_pages); 
        void free_4k_pages(void *, int num_pages); 
        
        /** Create a region. */
        void add_region(const MemoryRegion &); 

    private:
        int shared_page_count;
        IntelPageMap page_map;
};

