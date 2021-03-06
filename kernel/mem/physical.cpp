#include "physical.h"
/*
 * The start-up process here is to: 
 * - Find the highest address we need to cope with. 
 * - Find a block of memory that's bit enough to store the memory map in. 
 * - Somehow cope with any areas that need to be reserved ... are there 
 *   any? maybe where the kernel is loaded?
 * - Fill the memory map with in-use blocks. 
 * - Mark areas we have available as not in use. 
 * - Formally reserve the area we selected for our memory map. 
 */

PhysicalMemoryManager::PhysicalMemoryManager()
{

    highest_address = get_highest_memory_address();
    memory_allocation_size = (highest_address / PAGE_SIZE / 8) + 1; 
    uint64_t ma_upper_boundary = round_up_to_4k(memory_allocation_size); 

    /* Find space for our memory allocations. */
    MemoryMapEntry * ptr = &ram_data; 

    for (unsigned i=0; i < ram_data_size; ++i)
    {
        uint64_t base = ptr->base_addr; 
        uint64_t len = ptr->len;

        /* Best not to use memory location 0. We might want to make that invalid. */
        if (base == 0)
        {
            base += 0x1000; 
            
            if (len < 0x1000)
                len = 0; 
            else 
                len -= 0x1000;
        }

        if (ptr->type == NORMAL_MEMORY && len >= (ma_upper_boundary))
        {
            memory_allocations = (unsigned char *) base;
            vga.writeln(kstring("Allocated memory for memory allocation table at:") + kstring(uintptr_t(base))); 
            break;
        }
        ptr++;
    }

    if (memory_allocations == nullptr)
    {
        /* failed to find memory for the memory map! */
        vga.writeln("Serious Error"); 
        while (1) {}
    }

    /* mark our memory as in use. */
    memset((char*)memory_allocations, 0xFF, memory_allocation_size); 

    /* mark up what's actually available. */
    ptr = &ram_data; 

    for (unsigned i=0; i < ram_data_size; ++i)
    {
        if (ptr->type == NORMAL_MEMORY)
        {

            uint64_t end_addr = ptr->base_addr + ptr->len; 

            for (unsigned location = ptr->base_addr; location < end_addr; location += PAGE_SIZE)
                mark_available(location / 4096); 

            total_memory += ptr->len; /* @todo: doesn't work if the memory table has overlaps! */
        }

        ptr++;
    }

    available_memory = total_memory;

    /* Reserve this bit used by the BIOS. */
    reserve_range(0x0, 0x1000); 
    /* And finally reserve what we used at the start. */
    reserve_range((uintptr_t) memory_allocations, ma_upper_boundary); 
    /* And reserve our stack. */
    reserve_range(0x3000, 0x4000); 
    /* @todo: And, shit, we'd better reserve the kernel space too! (Pages 7 to 20)*/
    reserve_range(0x7000, 0x20000); 

    print_debug(vga);
}

uintptr_t PhysicalMemoryManager::get_highest_memory_address()
{
    uint64_t himem = 0; 

    MemoryMapEntry * ptr = &ram_data; 
    for (unsigned i=0; i < ram_data_size; ++i)
    {
        if (ptr->type == NORMAL_MEMORY)
        {
            uint64_t end_addr = ptr->base_addr + ptr->len;

            if (himem < end_addr)
                himem = end_addr;
        }
        ptr++;
    }

    /* We limit hi mem to 4G here. */
    himem = himem & 0xFFFFFFFF;
    return (uintptr_t) himem; 
}

void PhysicalMemoryManager::mark_available(int page_number)
{
    int index = page_number / 8; 
    int bit = page_number % 8;
    memory_allocations[index] = memory_allocations[index] & (~(1 << bit)); 
}

void PhysicalMemoryManager::mark_used(int page_number)
{
    int index = page_number / 8; 
    int bit = page_number % 8;
    memory_allocations[index] = memory_allocations[index] | (1 << bit); 
}

void * PhysicalMemoryManager::get_4k_page(const PhysicalMemoryRange & range)
{
    return get_pages(1, bytes_to_pages(range.base), bytes_to_pages(range.end)); 
}

MemoryRange PhysicalMemoryManager::get_multiple_4k_pages (const PhysicalMemoryRange & range, unsigned num_pages)
{
    void * ptr = get_pages(num_pages, bytes_to_pages(range.base), bytes_to_pages(range.end));  
    if (ptr == nullptr)
        return MemoryRange {0x0, 0x0};
    else
        return MemoryRange {(uintptr_t) ptr, num_pages * 0x1000}; 
}

void * PhysicalMemoryManager::get_pages(unsigned num_pages_required, unsigned lowest_page, unsigned highest_page)
{
    /* So, we:
     * - search for the correct number of available bits. 
     * - translate the first bit position to a pointer. 
     * - return the pointer.
     */
    unsigned num_pages_found = 0; 
    unsigned possible_first_page;
   
    kassert(lowest_page % 8 == 0); 
    unsigned highest_entry_to_search = highest_page / 8; 

    if (highest_page > memory_allocation_size)
        highest_entry_to_search = memory_allocation_size;

    unsigned mem_table_index = lowest_page / 8;
    while (num_pages_found < num_pages_required)
    {
        if (mem_table_index >= highest_entry_to_search)
        {
            kerror("pmem: allocation failed, not enough free space: " + kstring((uint32) mem_table_index) + " num:" + kstring((uint32) num_pages_required) + " pg range:" + kstring((uint16) lowest_page) + "."+ kstring((uint16) highest_page)); 
            while (1) {}
            return nullptr;
        }

        unsigned char mem_table_entry = memory_allocations[mem_table_index];

        if (num_pages_found > 0)
        {
            if (mem_table_entry == 0)
            {
                num_pages_found += 8;
            }
            else
            {
                num_pages_found += bit_scan_forward_32(mem_table_entry) + 1; /* find as many free bits as we can. */

                if (num_pages_found < num_pages_required)
                    num_pages_found = 0; 
            }
        }
        else
        {
            if (num_pages_required < 8) /* It might fit inside a byte. */
            {
                int index_of_free_space = get_index_of_continuous_clear_bits(mem_table_entry, num_pages_required);  /* -1 on not found. */

                if (index_of_free_space >= 0)
                {
                    possible_first_page = (mem_table_index * 8) + index_of_free_space;
                    num_pages_found = num_pages_required; 
                    break;
                }
            }

            if (mem_table_entry == 0)
                num_pages_found = 8;
            else
                num_pages_found = 7 - bit_scan_reverse_32(mem_table_entry);

            if (num_pages_found != 0)
                possible_first_page = (mem_table_index * 8) + num_pages_found;
        }

        mem_table_index++;
    }

    intptr_t p = PAGE_SIZE * possible_first_page;
    reserve_range(p, num_pages_required * PAGE_SIZE);
    return (void *) p; 
}

void PhysicalMemoryManager::free_multiple_4k_pages(void * pointer, 
                                                   unsigned number_of_pages_to_free)
{
    (void) pointer; 
    (void) number_of_pages_to_free;
    kerror("free_multiple_4k_pages is not yet implemented."); 
}

int PhysicalMemoryManager::get_index_of_continuous_clear_bits(unsigned char byte, 
                                                              unsigned length_required)
{
    unsigned bits_found = 0;
    for (int i = 0; i < 8; ++i)
    {
        unsigned bit = byte & 0x1;

        if (bit == 0)
            bits_found++;
        else 
            bits_found = 0; 

        if (bits_found == length_required)
            return (i - length_required + 1);

        byte = byte >> 1; 
    }
    return -1;
}
uintptr_t PhysicalMemoryManager::round_up_to_4k(uintptr_t  addr)
{
    uintptr_t a = addr & 0xFFF; 
    uintptr_t b = addr & ~0xFFF;

    if (a != 0) 
        b += 0x1000; 

    return b; 
}

void PhysicalMemoryManager::assert_page_boundary(uintptr_t ptr)
{
    if ((ptr & 0xFFF) != 0)
    {
        vga.writeln("Page boundary fault."); 
        while (1) {}
    }
}

void PhysicalMemoryManager::reserve_range(uintptr_t start, uintptr_t size)
{
   assert_page_boundary(start);  
   assert_page_boundary(size); 

   unsigned first_page = start / 4096; 
   unsigned end_page = (size + start) / 4096;

   for (unsigned i = first_page; i < end_page; ++i)
       mark_used(i); 
}

void PhysicalMemoryManager::print_debug(VgaDriver & vga)
{
    vga.writeln("Memory Map Entries"); 
    vga.writeln(kstring("Size: ") + kstring(ram_data_size));

    MemoryMapEntry * ptr = &ram_data; 
    for (unsigned i=0; i < ram_data_size; ++i)
    {
        kstring s = "base_addr="; 
        s += kstring(ptr->base_addr);
        s += " len=";
        s += kstring(ptr->len);
        s += " type=";
        switch (ptr->type)
        {
            case 1: 
                s += "Normal";
                break;
            case 2:
            case 4:
                s += "Reserved";
                break;
            case 3:
                s += "ACPI";
                break;
            case 5:
                s += "Unusable";
                break;
            case 6:
                s += "Disabled";
                break;
            default:
                s += "Undefined (error)";
                break;

        }
        vga.writeln(s);

        ptr++;
    }

    vga.writeln(kstring("Memory available ") + kstring(available_memory / 1024) + "k");
}

bool PhysicalMemoryManager::is_command_supported(const kstring & cmd)
{
    return (cmd == "pmem");
}

int PhysicalMemoryManager::execute_cli_command(const kvector<kstring> & )
{
    print_debug(vga); 
    return 0; 
}
