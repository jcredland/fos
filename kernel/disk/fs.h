
/* Public interface to the file system. 
 *
 * File sizes are 64 bits, which will be nice and fast when this finally gets moved over to
 * long mode.   This avoids a 4Gb limit on files.
 *
 * The file system interface will need to be able to route the request to the
 * correct underlying file system. 
 *
 * FUTURE
 * Object storage looks like the future - at last for this week. 
 *
 * NOTES
 * These are the first system calls to be defined.  It's a total mystery as to how they
 * are actually going to be called. 
 *
 * Also, whether it should be a bit more POSIX.  And whether they should all be defined
 * in one place, or ... well ... yeah.
 */

class VirtualFS 
{
    VirtualFS(FileSystem & fs_to_mount);

    int open(const char * file_name);
    int read(int file_handle, const char * buffer, uint64 bytes);
    int close(int file_handle); 

    struct F_PACKED FsDirEntry
    {
        char name[256];
    };

    /**
     * Reads at most maximum_size of directory entries into a buffer. 
     */
    uint32 fs_dir_read(const char * file_name, const char * buffer, uint32 maximum_size);
};
