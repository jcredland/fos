
/** 
 * Implements a standard interface to the FAT16 file system.  This standard
 * interface will expand become the standard adapter for all file systems 
 * attached to the root VirtualFS. It hides all the file system specific
 * behaviour from the VirtualFS. 
 *
 * The VirtualFS will probably become something a bit POSIX like. 
 * */
class Fat16VFSAdapter 
{
    public:
        Fat16VFSAdapter(Fat16 & underlying_filesystem)
            :
            fs(underlying_filesystem)
        {}
        /** Open a file for reading. */
        int open(const char * filename)
        {
            File f = fs.get_file(filename); 

            if (! f.exists_as_file())
                return (int) DiskErrorCode::kFileNotFound;

            open_files.push_back(f);  /* TODO - should really search for an empty slot. */

            return open_files.size() - 1; 
        }
        /** Perform sequential reading of a file. */
        int64 read(int file_handle, const char * buffer, int64 bytes)
        {
            if ((bytes & 0x1ff) > 0)
                return (int64) DiskResultCode::ALIGNMENT_ERROR;

            if (! is_valid_handle(file_handle))
                return (int64) DiskResultCode::BAD_HANDLE;

            int32 blocks = bytes / 512; 
            int64 bytes_transferred; 

            File & f = open_files[file_handle];

            while (blocks--)
            {
                int b = f.get_next_block(buffer); 

                if (b == 0)
                    break;

                bytes_transferred += b;
                buffer += b;
            } 

            return bytes_transferred;
        }
        int64 size(int file_handle)
        {
            if (is_valid_handle(file_handle))
                return open_files[file_handle].size(); 
            else 
                return (int64) DiskResultCode::BAD_HANDLE;
        }
        int close(int file_handle)
        {
            /* This can do nothing right now, as we don't reuse handles. */
        }
    private:
        kvector<Fat16::File> open_files;
        Fat16 & fs; 
};

