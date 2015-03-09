#include <klibrary/klibrary.h>
#include <disk/disk.h>

ATAControllerPIO::ATAControllerPIO (bool usePrimary)
{
    if (usePrimary)
        base_addr = 0x1F0;
    else
        base_addr = 0x170;

    if (has_ide_controller())
    {
        if (identify (0, &master_data))
        {
            kdebug ("ide: master drive found");
//            klog_identify_info (master_data);
        }

        if (identify (1, &slave_data))
        {
            kdebug ("ide: slave drive found");
//            klog_identify_info (slave_data);
        }
    }
}

bool ATAControllerPIO::read (char* buffer, uint8 drive, uint32 logical_block_address)
{
    int retry_count = 2;

retry_access:
    select_logical_block (drive, logical_block_address);
    ata_out (R_SECTOR_COUNT, 1);
    send_command (READ_SECTORS);

    wait_while_busy(); /* not necessary? */

    if (! wait_until_ready_or_error())
    {
        kdebug("ide: read error"); 
        if (retry_count-- == 0) 
        {
            return false;
        }
        else
        {
            reset(); 
            timer.delay_ms(2); 
            goto retry_access;
        }
    }

    uint16* word_buffer = (uint16*) buffer;

    for (int i = 0; i < 256; ++i)
        word_buffer[i] = ata_in_word (R_DATA);

    return true;
}

bool ATAControllerPIO::write (char* buffer, uint8 drive, uint32 logical_block_address)
{
    select_logical_block (drive, logical_block_address);
    ata_out (R_SECTOR_COUNT, 1);
    send_command (WRITE_SECTORS);
    
    wait_while_busy(); /* not necessary? */

    if (! wait_until_ready_or_error())
    {
        kdebug("ide: read error"); 
        return false;
    }

    uint16* word_buffer = (uint16*) buffer;

    for (int i = 0; i < 256; ++i)
        ata_out_word(R_DATA, word_buffer[i]); 

    return true;
}


/*****/

/*
 * To use the IDENTIFY command, select a target drive by sending 0xA0 for the 
 * master drive, or 0xB0 for the slave, to the "drive select" IO port. On the 
 * Primary bus, this would be port 0x1F6. Then set the Sectorcount, LBAlo,
 * LBAmid, and LBAhi IO ports to 0 (port 0x1F2 to 0x1F5). Then send the IDENTIFY
 * command (0xEC) to the Command IO port (0x1F7). Then read the Status port
 * (0x1F7) again. If the value read is 0, the drive does not exist. For any
 * other value: poll the Status port (0x1F7) until bit 7 (BSY, value = 0x80)
 * clears. Because of some ATAPI drives that do not follow spec, at this point you
 * need to check the LBAmid and LBAhi ports (0x1F4 and 0x1F5) to see if they are
 * non-zero. If so, the drive is not ATA, and you should stop polling. Otherwise,
 * continue polling one of the Status ports until bit 3 (DRQ, value = 8) sets,
 * or until bit 0 (ERR, value = 1) sets.
 *
 * At that point, if ERR is clear, the data is ready to read from the Data port
 * (0x1F0). Read 256 16-bit values, and store them.
 * */


bool ATAControllerPIO::identify (uint8 drive, DriveData* drive_data_struct)
{
    uint16* response_data_buffer = (uint16*) drive_data_struct;

    kassert (drive < 2);

    reset();

    if (ata_in (R_SECTOR_NUMBER) != 0x01)
        kdebug ("Sector number wrong after reset");

    select (drive, 0, 0, 0);

    send_command (Command::IDENTIFY);

    wait_while_busy();

    if (ata_in (R_LBA_MID) != 0 || ata_in (R_LBA_HI) != 0)
        return false;

    if (read_status() == 0) 
        return false; /* Drive not present. */

    if (! wait_until_ready_or_error())
        return false; /* Error. */

    for (int i = 0; i < 256; ++i)
        response_data_buffer[i] = ata_in_word (R_DATA);

    /* TODO
     * If ecc_bytes_on_long is not four set features must be
     * used to specify the appropraite number of ECC bytes to be
     * used on long commands. */

    return true;
}


KString ATAControllerPIO::status_to_string (uint8 status) const
{
    KString s;

    if (bit_set (status, BUSY)) s.append ("BUSY ");

    if (bit_set (status, RDY)) s.append ("RDY ");

    if (bit_set (status, DRIVE_FAULT)) s.append ("DFLT ");

    if (bit_set (status, DSC)) s.append ("DSC ");

    if (bit_set (status, DRQ)) s.append ("DRQ ");

    if (bit_set (status, ERR)) s.append ("ERR ");

    return s;
}


/** Send software reset. */
void ATAControllerPIO::reset()
{
    ata_out (R_CONTROL_REGISTER, SRST);

    if (! is_busy())
        kdebug ("Drive failed to assert busy flag during reset.");

    ata_out (R_CONTROL_REGISTER, 0);
}

/**
 * @param: drive 0 for master, 1 for slave.
 */
void ATAControllerPIO::select (uint8 drive, uint8 head, uint16 cylinder, uint8 sector)
{
    kassert (head < 16 && drive < 2);

    ata_out (R_DRIVE_HEAD, 0xA0 | (drive << 4) | head);
    ata_out (R_SECTOR_NUMBER, sector);
    ata_out (R_CYLINDER_LO, cylinder & 0xFF);
    ata_out (R_CYLINDER_HI, cylinder >> 8);
}

void ATAControllerPIO::select_logical_block (uint8 drive, uint32 address)
{
    select (drive, (address >> 24) & 0xFF, (address >> 8) & 0xFFFF, address & 0xFF);
}

void ATAControllerPIO::send_command (uint8 command)
{
    /*
     * Upon receipt of a command, the device sets the BSY bit or the DRQ bit within 400 ns. Following the setting
     * of BSY bit equal to one or equal to zero and DRQ bit equal to one, the status presented by the device
     * depends on the type of command: PIO data in, PIO data out, non-data transfer or DMA. See the individual
     * command descriptions and clause 9 for the protocol followed by each command and command type.
     */
    ata_out (R_STATUS_COMMAND, command);
}

uint8 ATAControllerPIO::read_status() const
{
    int count = 4;

    while (count--)
        (void) ata_in (R_STATUS_COMMAND); /* while away some time. */

    return ata_in (R_STATUS_COMMAND);
}

bool ATAControllerPIO::is_busy()
{
    return bit_set (read_status(), BUSY);
}

bool ATAControllerPIO::has_ide_controller()
{
    ata_out (R_SECTOR_NUMBER, 0x12);
    return 0x12 == ata_in (R_SECTOR_NUMBER);
}

KString ATAControllerPIO::decode_word_string (const char* data, int size) const
{
    KString result;
    int half_size = size / 2;

    for (int i = 0; i < half_size; i++)
    {
        result.append_char (data[i * 2 + 1]);
        result.append_char (data[i * 2]);
    }

    return result;
}

void ATAControllerPIO::klog_identify_info (const DriveData& d) const
{
    kdebug (KString ("ide: ") + status_to_string (read_status()));
    kdebug (KString ("ide serial: ") + decode_word_string (d.serial, sizeof (d.serial)));
    kdebug (KString ("ide model: ") + decode_word_string (d.model_number, sizeof (d.model_number)));
    kdebug (KString ("ide sectors per track: ") + KString(d.sectors_per_track));
}

void ATAControllerPIO::wait_while_busy()
{
    uint64 time_target = timer.timer + 100; 

    while (is_busy()) 
    {
        if (timer.timer > time_target)
            return;
    }
}

bool ATAControllerPIO::wait_until_ready_or_error()
{
    uint64 time_target = timer.timer + 100; 
    uint8 s; 
    do
    {
        s = read_status();

        if (timer.timer > time_target)
            return false;

    } while (! (bit_set(s, DRQ) || bit_set(s, ERR))); 

    return (! bit_set(s, ERR)); 
}


