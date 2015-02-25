
/** Base class for storage drivers. */
class StorageDriver
{
public:
    /** Load a sector. */
    virtual bool read(char * buffer, uint32_t logical_block_address) = 0;
    /** Return the number of sectors. */
    virtual uint32_t size() = 0;
private:
};
