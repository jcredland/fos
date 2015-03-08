#pragma once
/** Base class for block storage drivers. */
class StorageDriver
{
public:
    /** Load a sector which is always 512 bytes long. */
    virtual bool read(char * buffer, uint32 logical_block_address) = 0;
    /** Return the number of sectors. */
    virtual uint32 size() = 0;
private:
};
