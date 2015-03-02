/** The most boring of file systems to get started. But FAT16 can be 
 * written to by MacOS so it'll be handy 
 * for copy cross-compiled binaries and so on into the system for testing. 
 * 
 * Plan A - get a read-only file system working. 
 * Plan B - make it so we can write. 
 * Plan C - do something more futuristic. 
 *
 * */
class Fat16
{
public:
    /* http://www.maverick-os.dk/FileSystemFormats/FAT16_FileSystem.html
     * FILENAME AND EXTENSION
     * Shorter names must be trailing padded with 20h. The extension is 3
     * byte long and shorter names also have to be trailing padded.
     *
     * The first byte in the filename is special:
     * A value of 00h is interpertated as no more entries.
     * A value of 05h is should be replaced with the ASCII character E5h.
     * Must not contain the value 20h.
     * A value of E5h is interpertated as 'the entry is free'.
     *
     * The following characters are not allowed in the filename or its extension:
     * Any character below 20h, except the 05h character.
     * Any character in the following list: 22h ("), 2Ah (*), 2Bh (+), 2Ch (,),
     * 2Eh (.), 2Fh (/), 3Ah (:), 3Bh (;), 3Ch (<), 3Dh (=), 3Eh (>), 3Fh (?),
     * 5Bh ([), 5Ch (\), 5Dh (]), 7Ch (|).
     *
     * For compatibility reasons it is recommended to limit the characters to:
     * Any uppercase characters from A to Z.
     * Any digit from 0 to 1.
     * Any of the following characters: #, $, %, &, ', (, ), -, @
     */
    struct F_PACKED DirectoryEntry
    {
        char    name[8];
        char    extension[3];
        uint8   attrib;
        uint8   reserved_nt;
        uint8   creation_ms;
        uint16  creation_time;
        uint16  creation_date;
        uint16  access_date;  /* Some sort of FDate class is needed really. */
        uint16  reserved;
        uint16  modification_time;
        uint16  modification_date;
        uint16  first_cluster;
        uint32  size;

        /* Some basic tests. */
        bool is_volume() const { return (attrib & VOLUME) != 0; }
        bool is_dir() const { return (attrib & DIRECTORY) != 0; }
        bool is_free() const { return ((unsigned char) name[0] == 0xE5) || (name[0] == '\0'); }
        bool is_last_entry() const { return name[0] == '\0'; }
        
        KString get_full_name()
        {
            KString r(name, 8);
            r.trim_end();
            KString r_ext(extension, 3);
            r_ext.trim_end();
            if (r_ext.length() > 0)
            {
                r.append_char('.');
                r.append(r_ext);
            }
            return r;
        }
        
        KString get_attrib_string()
        {
            KString r;
            if (attrib & ARCHIVE) r.append_char('A');
            if (attrib & DIRECTORY) r.append_char('D');
            if (attrib & VOLUME) r.append_char('V');
            if (attrib & SYSTEM) r.append_char('S');
            if (attrib & HIDDEN) r.append_char('H');
            if (attrib & READ_ONLY) r.append_char('R');
            return r;
        }
        
    };


    enum Attribute
    {
        RESV_1 = 0x80,
        RESV_2 = 0x40,
        ARCHIVE = 0x20,
        DIRECTORY = 0x10,
        VOLUME = 0x08,
        SYSTEM = 0x04,
        HIDDEN = 0x02,
        READ_ONLY = 0x01,
        LONG_FILE_NAME = 0x0f
    };

    /** File objects read files mapped using the File Allocation Table. */
    class File
    {
    public:
        File(Fat16 & file_system, const DirectoryEntry & d)
            :
            fs(file_system), dir_entry(d)
        {
            go_to_first_cluster();
        }
        /* Creates a null one - this will vanish after we get memory allocation sorted 
         * in the kernel. */
        File(Fat16 & file_system)
            :
                fs(file_system)
        {}
        /** Reads the next block (512 bytes) of the file.  Returns the number 
         * of bytes actually transferred.  If it returns 0 there are no more 
         * blocks or a negative number if an error occurred. */
        int get_next_block(char * buffer);
    private:
        /** Finds the first sector. */
        void go_to_first_cluster();
        /** Looks in the FAT, finds the next cluster and updates the internal 
         * pointers to be at its first sector. Returns false
         * if there is no next cluster and we are at the end. */
        bool go_to_next_cluster();

        uint32 file_size_remaining;
        uint16 current_cluster;
        uint32 next_sector;
        uint32 sector_after_cluster;

        Fat16 & fs;
        DirectoryEntry dir_entry;
    };

    /** The root directory in FAT16 has special handling as it isn't allocated or mapped
     * using the File Allocation Table. */ 
    class RootDirectoryFile
    {
    public:
        RootDirectoryFile(Fat16 & file_system)
            :
            fs(file_system)
        {
            go_to_first_sector(); 
        }
        int get_next_block(char * buffer);
    private:
        void go_to_first_sector()
        {
            start_sector = fs.loc.root_dir_start; 
            current_sector = start_sector; 
            size = fs.loc.root_dir_size;
        }

        Fat16 & fs; 
        uint16 start_sector; 
        uint16 current_sector;
        uint16 size; 
    };

    /** Wraps a FatFile object with methods for handling directory entries. */
    class Directory
    {
    public:
        /** Construct a FatDirectory object pointing at the root directory. */
        Directory(Fat16 & file_system)
            :
            is_root(true),
            root_dir_file(file_system),
            dir_file(file_system),
            pointer(0)
        {
            size_read = root_dir_file.get_next_block(buffer);
        }

        /** Construct a FatDirectory object for subdirectory specified by dir_entry.  */
        Directory(Fat16 & file_system, const DirectoryEntry & dir_entry)
            :
            is_root(false),
            root_dir_file(file_system),
            dir_file(file_system, dir_entry),
            pointer(0) 
        {
            size_read = dir_file.get_next_block(buffer);
        }
        /** Returns the next valid directory entry.  Returns a DiskError
         * if we have reached the end. */
        DiskResultCode next(DirectoryEntry * entry);

    private:
        bool get_next_block();
        bool is_root;

        RootDirectoryFile root_dir_file;
        File dir_file;
        
        int size_read;
        int pointer;
        char buffer[512];
    };
    
    struct F_PACKED BootSector
    {
        char    jump[3];
        char    os_name[8];

        /* This next bit is called the BIOS Parameter Block */

        uint16  bytes_per_sector;       /* 512 for compatibility. */
        uint8   sectors_per_cluster;    /* must be 1, 2, 4 ... cluster size must not be above 32k */
        uint16  reserved_sectors;       /* typically 1, just this sector. */
        uint8   copies_of_fat;          /* 2 is standard. */
        uint16  max_root_entries;       /* 512 for compatibility. */
        uint16  number_of_sectors_small;/* 0 if more than 65535 sectors are in use. */

        uint8   media_descriptor;       /* same value as the low byte in the first FAT entry */
        uint16  sectors_per_fat;        /* size of one copy of the FAT. */
        uint16  sectors_per_track;
        uint16  number_of_heads;
        uint32  hidden_sectors;         /* might be useful on partitioned disks. */
        uint32  number_of_sectors_large;/* if used file system id in MBR needs to be 6. */
        uint8   drive_number;
        uint8   reserved;
        uint8   has_extended_boot_sig;
        uint32  volume_serial;
        char    volume_label[11];
        char    fs_type[8]; /* should be the string FAT16 */
        char    bootstrap_code[448];
        uint16  bootstrap_magic_number;
    };


    /** Cconstruct a Fat16 object. */
    Fat16 (StorageDriver& device, uint32 start_block, uint32 size_in_blocks)
        :
        device (device),
        start_block (start_block),
        size (size_in_blocks)
    {
        kassert(sizeof(DirectoryEntry) == 32); 

        read_boot_sector();

        filesystem_is_valid = validate_boot_sector();

        if (is_valid())
            initalize_locations_table();
        else 
            kerror("fat16: file system not valid.");
    }

    bool is_valid() { return filesystem_is_valid; }

    uint32 get_sector_for_cluster(uint32 cluster)
    {
        /* What's this minus 2 for? */
        return bs.sectors_per_cluster * (cluster - 2) + loc.data_start; 
    }

private:
    int dir_reader_pointer;
    DirectoryEntry dir_reader_sector_cache[512 / sizeof(DirectoryEntry)];

    /** Clear this flag if the file system is corrupt and cannot be used in any way. */
    bool filesystem_is_valid; 

    BootSector bs;
    struct SectorLocations
    {
        uint32 reserved_region_start;
        uint32 fat_region_start;
        uint32 root_dir_start;
        uint32 root_dir_size;

        uint32 data_start;
        uint32 data_size;

        uint32 fat_entry_count;
    } loc;

    void initalize_locations_table()
    {
        loc.reserved_region_start = start_block;
        loc.fat_region_start = start_block + bs.reserved_sectors;
        loc.root_dir_start = loc.fat_region_start + (bs.copies_of_fat * bs.sectors_per_fat);
        loc.root_dir_size = get_root_dir_size();

        loc.data_start = loc.root_dir_start + loc.root_dir_size;
        loc.data_size = get_number_of_sectors() -
                        (bs.reserved_sectors + bs.copies_of_fat * bs.sectors_per_fat + loc.root_dir_size);

        if (bs.sectors_per_cluster != 0)
            loc.fat_entry_count = loc.data_size / bs.sectors_per_cluster;
        else 
            filesystem_is_valid = false;
    }

    uint32 get_number_of_sectors() const
    {
        if (bs.number_of_sectors_small > 0)
            return bs.number_of_sectors_small;
        else
            return bs.number_of_sectors_large;
    }
    uint32 get_root_dir_size() const
    {
        if (bs.bytes_per_sector == 0)
            return 0; 

        uint32 size_in_bytes = bs.max_root_entries * sizeof (DirectoryEntry);
        /* We could do this in a integer only way. But this is probably clearer:
        double size_in_sectors = double(size_in_bytes) / double(bs.bytes_per_sector);
        However we don't have a 'round' function yet. And implementing a general purpose
        one is not totally straightforward!
        */
        uint32 size_in_sectors = ( (bs.bytes_per_sector - 1) + size_in_bytes) / bs.bytes_per_sector;
        return size_in_sectors;
    }


    void fat16_assert (bool test_result, const char* message_on_fail)
    {
        if (test_result)
            return;

        assert_has_failed = true;
        kerror (KString ("fat:") + KString (message_on_fail));
    }

    bool validate_boot_sector()
    {
        assert_has_failed = false;

        fat16_assert (bs.bytes_per_sector == 512,
                      "bytes per sector is not 512");
        fat16_assert (bs.copies_of_fat == 2,
                      "number of fat is not 2");
        fat16_assert (bs.max_root_entries == 512,
                      "wrong number of root entries");
        fat16_assert (strncmp (bs.fs_type, "FAT16", 5) == 0,
                      "not labelled FAT16");

        khex_dump((char *) &bs, 32); 

        return ! assert_has_failed;
    }

    void read_boot_sector()
    {
        fat16_assert (device.read ((char*)&bs, start_block), "failed to read boot record");
    }


    bool assert_has_failed;
    StorageDriver& device;
    uint32 start_block;
    uint32 size; /* in blocks of the partition. */
};


