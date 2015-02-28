
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

//    vga.writeln(KString("Upper Boundary:") + KString(ma_upper_boundary)); 

    /* Find space for our memory allocations. */
    MemoryMapEntry * ptr = &ram_data; 

    for (unsigned i=0; i < ram_data_size; ++i)
    {
        uint64_t base = ptr->base_addr; 
        uint64_t len = ptr->len;
 //       vga.writeln(KString(base) + " base");

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
            memory_allocations = (char *) base;
            vga.writeln(KString("Allocated memory for memory allocation table at:") + KString(uintptr_t(base))); 
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
    memset(memory_allocations, 0xFF, memory_allocation_size); 

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

    /* And finally reserve what we used at the start. */
    reserve_range((uintptr_t) memory_allocations, ma_upper_boundary); 
    /* And reserve our stack. */
    reserve_range(0x3000, 0x4000); 
    /* @todo: And, shit, we'd better reserve the kernel space too! */
    reserve_range(0x7000, 0x10000); 
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
    vga.writeln(KString("Size: ") + KString(ram_data_size));

    MemoryMapEntry * ptr = &ram_data; 
    for (unsigned i=0; i < ram_data_size; ++i)
    {
        KString s = "base_addr="; 
        s += KString(ptr->base_addr);
        s += " len=";
        s += KString(ptr->len);
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

    vga.writeln(KString("Memory available ") + KString(available_memory / 1024) + "k");
}

PhysicalMemoryManager pmem;

