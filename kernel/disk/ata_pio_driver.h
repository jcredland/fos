

/** 
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
class AtaPioDriver
:
    public StorageDriver
{
    public:
        AtaPioDriver() 
        {
            identify(0, master_data); 
//            identify(1, slave_data); 
        }

        /** Load a sector. */
        bool read(char * buffer, uint32_t logical_block_address) { return false; }

        /** Return the number of sectors. */
        uint32_t size() { return 0; }

    private:
        const uint16 BASE_PRIMARY = 0x1F0;
        enum 
        {
            R_DATA = 0,
            R_ERROR_FEATURE = 1,
            R_SECTOR_COUNT = 2,
            R_SECTOR_NUMBER = 3,
            R_CYLINDER_LO = 4,
            R_CYLINDER_HI = 5,
            R_LBA_LO = 3,
            R_LBA_MID = 4, 
            R_LBA_HI = 5, 
            R_DRIVE_HEAD = 6,
            R_STATUS_COMMAND = 7
        };

        enum Command 
        {
            IDENTIFY = 0xEC
        };

        enum StatusBits
        {
            BUSY = 128,
            RDY = 64,
            DRIVE_FAULT = 32,
            SRV = 16,
            DRQ = 8,
            ERR = 1,

        };

        const uint16 R_ALT_STATUS = 0x3f6;

        void select_master() { outb(BASE_PRIMARY + R_DRIVE_HEAD, 0xA0); /* master. */ }
        void select_slave() { outb(BASE_PRIMARY + R_DRIVE_HEAD, 0xB0); /* master. */ }

        /** 
         * @param: drive 0 for master, 1 for slave.
         */
        void select(uint8 drive, uint8 head, uint16 cylinder, uint8 sector)
        {
            kassert(head < 16 && drive < 2); 
            outb(BASE_PRIMARY + R_DRIVE_HEAD, 0xA0 | (drive << 4) | head); 
            outb(BASE_PRIMARY + R_SECTOR_NUMBER, sector); 
            outb(BASE_PRIMARY + R_CYLINDER_LO, cylinder & 0xFF); 
            outb(BASE_PRIMARY + R_CYLINDER_HI, cylinder >> 8); 
        }

        void select_logical_block(uint8 drive, uint32 address)
        {
            select(drive, (address >> 24) & 0xFF, (address >> 8) & 0xFFFF, address & 0xFF);
        }

        void send_command(uint8 command)
        {
            outb(BASE_PRIMARY + R_STATUS_COMMAND, command); 
        }

        uint8 read_status()
        {
            int count = 4; 
            while (count--)
                (void) inb(BASE_PRIMARY + R_STATUS_COMMAND); /* while away some time. */

            return inb(BASE_PRIMARY + R_STATUS_COMMAND); 
        }

        bool is_busy()
        {
            return (read_status() & BUSY) == BUSY;
        }

        /** Tries to identify a drive. */
        bool identify(uint8 drive, char * response_data_buffer)
        {
            kassert(drive < 2); 

            select(drive, 0, 0, 0); 
            send_command(Command::IDENTIFY); 

            while (is_busy()) {}

            if (inb(BASE_PRIMARY + R_LBA_MID) != 0 || inb(BASE_PRIMARY + R_LBA_HI) != 0)
                return false; 

            uint8 status;

            do { status = read_status(); } 
            while ((status & DRQ) == 0 && (status & ERR) == 0);

            if ((status & DRQ) != 0)
                klog(KL_DEBUG, "Found IDE"); 
            else 
                return false; /* No drive. */

//            for (int i = 0; i < 256; ++i)
//                response_data_buffer[i] = inb(BASE_PRIMARY + R_DATA); 
                master_data[255] = inb(BASE_PRIMARY + R_DATA); 
                master_data[255] = inb(BASE_PRIMARY + R_DATA); 

            return true; 
        }

        char master_data[256];
        char slave_data[256];

        bool has_master;
        bool has_slave;
};


