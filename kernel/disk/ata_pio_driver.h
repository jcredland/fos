


/**
 * ATA Programmed Input Output Driver (ATA PIO).
 *
 * This implements the slowest possible type of ATA disk driver.  But at least it'll work!
 * At some point it needs to be swapped for a DMA one, and it'd be good to get the SATA drivers
 * working as well.
 *
 * It uses Logical Block Addressing and only supports the primary IDE controller right now.
 *
 * Logical Block Addressing
 * The logical address is a 28-bit unsigned binary number, which is placed into the command block as follows:
        – bits 27–24 into the Drive/Head register bits 3–0
        – bits 23–16 into the Cylinder High register
        – bits 15–8 into the Cylinder Low register
        – bits 7–0 into the Sector Number register
 */
class ATAControllerPIO
{
public:
    ATAControllerPIO (bool usePrimaryController = true);

    /** Load a sector of 512 (0x200) bytes. */
    bool read (char* buffer, uint8 drive, uint32 logical_block_address);

    /** Write a sector of 512 (0x200 bytes. */
    bool write (char* buffer, uint8 drive, uint32 logical_block_address);

    /** Return the number of 512 byte sectors. */
    uint32 size(uint8 /* drive */) { return 0; }

private:
    uint16 base_addr = 0x1F0;

    /** Return the controllers status bits as a string for debugging. */
    KString status_to_string (uint8 status) const;

    void ata_out (uint16 port_type, uint8 value) const
    {
        outb (base_addr + port_type, value);
    }
    void ata_out_word (uint16 port_type, uint16 value) const
    {
        outw (base_addr + port_type, value);
    }
    uint8 ata_in (uint16 port_type) const
    {
        return inb (base_addr + port_type);
    }
    uint16 ata_in_word (uint16 port_type) const
    {
        return inw (base_addr + port_type);
    }

    /** Send software reset. */
    void reset();
    /*
     * @param: drive 0 for master, 1 for slave.
     */
    void select (uint8 drive, uint8 head, uint16 cylinder, uint8 sector);
    void select_logical_block (uint8 drive, uint32 address);
    void send_command (uint8 command);

    /** Read the status byte after a 400ns delay. */
    uint8 read_status() const;

    /** Check to see if we are busy. */
    bool is_busy();
    void wait_while_busy(); 
    bool wait_until_ready_or_error(); 

    /** Returns true if this IDE controller is detected. */
    bool has_ide_controller();

    enum RegisterOffsets
    {
        R_DATA = 0, R_ERROR_FEATURE = 1, R_SECTOR_COUNT = 2, R_SECTOR_NUMBER = 3,
        R_CYLINDER_LO = 4, R_CYLINDER_HI = 5, R_LBA_LO = 3, R_LBA_MID = 4,
        R_LBA_HI = 5, R_DRIVE_HEAD = 6, R_STATUS_COMMAND = 7, R_CONTROL_REGISTER = 0x206
    };

    enum ControlRegister
    {
        SRST = 0x4
    };

    /** This enum contains the commands that all ATA compatible IDE systems
     * must implement. */
    enum Command
    {
        IDENTIFY = 0xEC,
        READ_SECTORS = 0x20,            /* Mandatory to implement in IDE: with re-try. */
        READ_SECTORS_NO_RETRY = 0x21,   /* Mandatory to implement in IDE */
        WRITE_SECTORS = 0x30,
        WRITE_SECTORS_NO_RETRY = 0x31,
        READ_SECTORS_VERIFY = 0x40,
        READ_SECTORS_VERIFY_NO_RETRY = 0x41,
        SEEK = 0x70                     /* Seek is defined as 0x70 - 0x7F (but why?) */
    };

    enum StatusBits
    {
        BUSY = 0x80,
        RDY  = 0x40,
        DRIVE_FAULT = 0x20,
        DSC  = 0x10, /* drive seek complete (1 when not seeking). */
        DRQ  = 0x8, /* data request bit, set to 1 when the drive is ready to transfer data. */
        CORR = 0x4, /* correctable error encounter. */
        IDX  = 0x2, /* goes to 1 once per disk revolution. */
        ERR  = 0x1,
    };


    const uint16 R_ALT_STATUS = 0x3f6;



    struct F_PACKED DriveData
    {
        uint16  general_config_info;
        /* Note there are some limitations on the maximum value of the cylinders field for backwards
         * compatibility.  See ATA specification for details. */
        uint16  cylinders;
        uint16  r0;
        uint16  heads;
        uint16  vendor_obsolete0;
        uint16  vendor_obsolete1;
        uint16  sectors_per_track;
        uint16  r1;
        uint16  r2;
        uint16  r3;
        char    serial[20];
        uint16  vendor_specific0;
        uint16  vendor_specific1;
        uint16  ecc_bytes_on_long;
        char    firmware_revision[8]; /* padded with spaces. */
        char    model_number[40];
        uint8   seagate_reserved;
        uint8   max_sectors_per_interrupt; /* LSB TODO: is this in the right plac? */
        uint16  can_perform_double_word_io; /* might be reserved??? */
        uint16  capability_bits;
        uint16  r4;
        uint8   pio_transfer_timing_cycle;
        uint8   r5;
        uint8   dma_transfer_timing_cycle;
        uint8   r6;
        struct F_PACKED
        {
            uint16 reserved : 15;
            uint16 is_valid : 1;
            uint16 current_num_cylinder;
            uint16 current_num_heads;
            uint16 current_num_sectors_per_track;
        } translation_mode;
        uint16 multiple_sectors;
        uint32 lba_total_sectors;
        uint16 singleword_dma_info;
        uint16 multiword_dma_info;
        uint16 reserved_space[64];
        uint16 reserved_space2[128];
    };

    DriveData master_data;
    DriveData slave_data;

    /** Runs the IDENTIFY command on a drive and fills the response
     * data buffer (DriveData). . */
    bool identify (uint8 drive, DriveData* drive_data_struct);

    /** Deals with the byte order problem in strings from the IDE controller.
     * @param size must be even.
     */
    KString decode_word_string (const char* data, int size) const;

    void klog_identify_info (const DriveData& d) const;

    bool has_master;
    bool has_slave;
};



/** The ATA drive manages access to a single drive in linear block addressing mode.  */
class ATADrive
:
    public StorageDriver
{
public:
    ATADrive(ATAControllerPIO & controller, uint8 drive)
        :
            controller(controller),
            drive(drive)
    {}

    bool read(char * buffer, uint32 logical_block_address) override
    {
        return controller.read(buffer, drive, logical_block_address); 
    }

    uint32 size() override
    {
        return controller.size(drive); 
    }

private:
    ATAControllerPIO & controller;
    uint8 drive; 
};

