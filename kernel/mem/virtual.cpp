

int VirtualMemoryManager::execute_cli_command(const kstd::kvector<KString> & parameter_list)
{
    vga.write("vmm test and info"); 

    return 0; 
}


ProcessVirtualMemoryMap::ProcessVirtualMemoryMap(int number_of_shared_pages)
{
    shared_page_count = number_of_shared_pages; 

    while (number_of_shared_pages--)
    {
        if (! page_map.create_empty_table(shared_page_count))
            kdebug("error creating page table"); 
    }
}

ProcessVirtualMemoryMap::ProcessVirtualMemoryMap(ProcessVirtualMemoryMap & map_to_clone)
{
    shared_page_count = map_to_clone.shared_page_count;

    for (int page = 0; page < shared_page_count; ++page)
        page_map.set_page_table_addr(page, map_to_clone.page_map.get_page_table_addr(page)); 
}
