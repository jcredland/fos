

int Fat16::RootDirectoryFile::get_next_block(char * buffer)
{
    if (current_sector >= (start_sector + size))
        return 0; 

    bool success = fs.device.read(buffer, current_sector++); 

    static int debug_flag = 0; 

    if (! success)
        return (int) DiskResultCode::DISK_ERROR;

    return 512;
}

DiskResultCode Fat16::Directory::next(DirectoryEntry * entry)
{
    do 
    {
        if (pointer >= 512)
        {
            int bytes_read; 

            if (is_root)
                bytes_read = root_dir_file.get_next_block(buffer);
            else
                bytes_read = dir_file.get_next_block(buffer); 

            if (bytes_read != 512) 
                return DiskResultCode::DISK_ERROR; 

            pointer = 0; 
        }

        *entry =  * ((DirectoryEntry *) &buffer[pointer]);
        pointer += sizeof(DirectoryEntry); 

    } while ((*entry).name[0] == 0); 

    return DiskResultCode::SUCCESS;
}


int Fat16::File::get_next_block(char * buffer)
{
    if (file_size_remaining == 0)
        return 0; 

    fs.device.read(buffer, next_sector);

    bool more_data_available = true;

    if (++next_sector >= sector_after_cluster)
        more_data_available = go_to_next_cluster();

    if (file_size_remaining > 512)
    {
        file_size_remaining -= 512; 

        if (! more_data_available)
            kerror("fat16: fat or directory corruption (1)"); 

        return 512; 
    }
    else
    {
        if (more_data_available)
            kerror("fat16: fat or directory corruption (2)"); 

        uint32 fs = file_size_remaining;
        file_size_remaining = 0; 
        return fs;
    }
}

void Fat16::File::go_to_first_cluster()
{
    next_sector = fs.get_sector_for_cluster(dir_entry.first_cluster);
    current_cluster = dir_entry.first_cluster;
    sector_after_cluster = next_sector + fs.bs.sectors_per_cluster;
    file_size_remaining = dir_entry.size;
}


bool Fat16::File::go_to_next_cluster()
{
    uint16 fat_offset = current_cluster * 2; 
    uint16 fat_sector = fs.loc.fat_region_start + (fat_offset / 512);
    uint16 index_into_sector = fat_offset % 512; 

    char fat_sector_buffer[512];
    fs.device.read(fat_sector_buffer, fat_sector); 

    uint16 new_cluster = (*(uint16 *)(fat_sector_buffer + index_into_sector));

    if (new_cluster >= 0xFFF8)
    {
        return false;
    }
    else
    {
        current_cluster = new_cluster;
        next_sector = fs.get_sector_for_cluster(current_cluster); 
        sector_after_cluster = next_sector + fs.bs.sectors_per_cluster;
        return true;
    }
}
