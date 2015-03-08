#define FOS_MOCK

#include <stdio.h>
#include <stdlib.h>
#include "../../std_headers.h"
#include "../storage_driver.h"
#include "../fat16.h"

#include "../fat16.cpp"

class MockDriver
: 
    public StorageDriver
{
public:
    MockDriver(const char * disk_image_file_name)
    {
        f = fopen(disk_image_file_name, "rb"); 

        if (f == NULL)
        {
            printf("error: could not open disk image"); 
            exit(1); 
        }
    }

    /** Load a sector which is always 512 bytes long. */
    bool read(char * buffer, uint32 logical_block_address) 
    {
        fseek(f, logical_block_address * 512, SEEK_SET); 
        kdebug(KString("reading lba: ") + KString(logical_block_address * 512)); 
        return fread(buffer, 512, 1, f) == 1; 
    }

    /** Return the number of sectors. */
    uint32 size()
    {
        fseek(f, 0L, SEEK_END);
        uint32 size = ftell(f); 
        
        if (size % 512 != 0)
            printf("warning: file size is not a round number of 512 byte sectors"); 

        return size / 512;
    }

private:
    FILE * f; 
};

class FileSystemCommand
{
};

/** File system testing.  */

void ls_dir(Fat16 & fs, Fat16::DirectoryEntry dir_to_view)
{
    Fat16::Directory dir(fs, dir_to_view); 
    Fat16::DirectoryEntry entry; 
    kdebug("Subdir..."); 
    while (dir.next(&entry) == DiskResultCode::SUCCESS)
    {
        kdebug(entry.get_full_name());
    }
    kdebug("...end Subdir.");
}

void ls_root(Fat16 & fs) 
{
    Fat16::Directory dir(fs); 
    Fat16::DirectoryEntry entry; 

    while (dir.next(&entry) == DiskResultCode::SUCCESS)
    {
        KString attrib = entry.get_attrib_string();
        kdebug(entry.get_full_name() + " [" + attrib + "]");
        khex_dump(entry.name, 8); 
        kdebug(entry.first_cluster); 

        if (entry.is_dir() && entry.name[0] == 'S')
            ls_dir(fs, entry);
    }
}

int main(int argc, char ** argv)
{
    if (argc != 2)
    {
        printf("usage: fs_test <image_file> <command> [<parameters>]\n"); 
        exit(1); 
    }
    
    MockDriver driver(argv[1]); 
    Fat16 fat_fs(driver, 1, driver.size()); 

    ls_root(fat_fs);
}

