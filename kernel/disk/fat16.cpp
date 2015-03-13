
#include <disk/fat16.h>
#include <klibrary/klibrary.h>


/************************************************************/

/************************************************************/

int Fat16::RootDirectoryFile::get_next_block(char * buffer)
{
    if (current_sector >= (start_sector + size))
        return 0; 

    bool success = fs.device.read(buffer, current_sector++); 

    if (! success)
        return (int) DiskResultCode::DISK_ERROR;

    return 512;
}

bool Fat16::Directory::get_next_block()
{
    int bytes_read; 

    if (is_root)
        bytes_read = root_dir_file.get_next_block(buffer);
    else
        bytes_read = dir_file.get_next_block(buffer); 

    if (bytes_read != 512) 
        return false;

    pointer = 0; 

    return true;
}


DiskResultCode Fat16::Directory::next(DirectoryEntry * entry)
{
    DirectoryEntry * e;

    do 
    {
        /* Detect end-of-directory conditions. 
         * - no more directory, or filename
         *   starts with a zero. */
        if (pointer >= 512 && (! get_next_block()))
            return DiskResultCode::END_OF_FILE; 

        e = (DirectoryEntry *) &buffer[pointer];

        if (e->is_last_entry())
            return DiskResultCode::END_OF_FILE;

        pointer += sizeof(DirectoryEntry); 

    } while ( (e->is_volume()) || (e->is_free()));

    *entry = *e;

    return DiskResultCode::SUCCESS;
}


int Fat16::File::get_next_block(char * buffer)
{
    bool is_directory = dir_entry.is_dir(); 

    if (file_size_remaining == 0 && ! is_directory)
        return 0; 

    kdebug(KString("get_next_block - reading next sector ") + KString(next_sector)); 
    fs.device.read(buffer, next_sector);

    bool more_data_available = true;

    if (++next_sector >= sector_after_cluster)
        more_data_available = go_to_next_cluster();

    if (is_directory)
    {
        return 512; 
    } 
    else 
    {
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
    kdebug("go_to_next_cluster()"); 
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

const int num_commands = 3;
const char * supported_commands[num_commands] = { "dir", "cd", "type" };

bool Fat16::is_command_supported(const KString & cmd)
{
    for (int i = 0; i < num_commands; ++i)
        if (cmd == supported_commands[i])
            return true; 

    return false;
}

void Fat16::execute_dir(const kstd::kvector<KString> &)
{
    Fat16::Directory dir(*this); 
    Fat16::DirectoryEntry entry; 

    while (dir.next(&entry) == DiskResultCode::SUCCESS)
    {
        KString attrib = entry.get_attrib_string();
        kdebug(entry.get_full_name() + " [" + attrib + "]");
        khex_dump(entry.name, 8); 
        kdebug(entry.first_cluster); 
    }
}

int Fat16::execute_cli_command(const kstd::kvector<KString> & param_list)
{
    if (param_list[0] == "dir")
        execute_dir(param_list); 
    return 0;
}

