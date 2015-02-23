

PhysicalMemoryManager::PhysicalMemoryManager()
{
    /* mark our memory as in use. */
    memset(memory_allocations, 0xFF, sizeof(memory_allocations)); 
    
    MemoryMapEntry * ptr = &ram_data; 
    for (int i=0; i < ram_data_size; ++i)
    {
        if (ptr->type == NORMAL_MEMORY)
        {
            uint64_t end_addr = type->base_addr + type->len; 

            for (int location = type->base_addr; location < end_addr; location += PAGE_SIZE)
                mark_available(location);

            total_memory += type->len; /* @todo: doesn't work if the memory table has overlaps! */
        }
    }

    available_memory = total_memory;
}

void PhysicalMemoryManager::mark_available(int page_number)
{
    int index = page_number / 8; 
    int bit = page_number % 8;
    memory_allocations[index] = memory_allocations[index] & (^(1 << bit)); 
}

void PhysicalMemoryManager::mark_used(int page_number)
{
    int index = page_number / 8; 
    int bit = page_number % 8;
    memory_allocations[index] = memory_allocations[index] | (1 << bit); 
    
void PhysicalMemoryManager::print_debug(VgaDriver & vga)
{
    vga.writeln("Memory Map Entries"); 
    vga.writeln(KString("Size: ") + KString(ram_data_size));

    MemoryMapEntry * ptr = &ram_data; 
    for (int i=0; i < ram_data_size; ++i)
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
}

PhysicalMemoryManager pmem;

