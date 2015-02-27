

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
            klog_identify_info (master_data);
        }

        if (identify (1, &slave_data))
        {
            kdebug ("ide: slave drive found");
            klog_identify_info (slave_data);
        }
    }
}

bool ATAControllerPIO::read (char* buffer, uint32 logical_block_address)
{
    select_logical_block (0, logical_block_address);
    ata_out (R_SECTOR_COUNT, 1);
    send_command (READ_SECTORS);

    wait_while_busy(); /* not necessary? */

    if (! wait_until_ready_or_error())
        return false;

    uint16* word_buffer = (uint16*) buffer;

    for (int i = 0; i < 128; ++i)
        word_buffer[i] = ata_in_word (R_DATA);

    return true;
}

bool ATAControllerPIO::write (char* buffer, uint32 logical_block_address)
{
    select_logical_block (0, logical_block_address);
    ata_out (R_SECTOR_COUNT, 1);
    send_command (WRITE_SECTORS);
    
    wait_while_busy(); /* not necessary? */

    if (! wait_until_ready_or_error())
        return false;

    uint16* word_buffer = (uint16*) buffer;

    for (int i = 0; i < 128; ++i)
        ata_out_word(R_DATA, word_buffer[i]); 

    return true;
}


/*****/


bool ATAControllerPIO::identify (uint8 drive, DriveData* drive_data_struct)
{
    uint16* response_data_buffer = (uint16*) drive_data_struct;

    kassert (drive < 2);

    reset();

    if (ata_in (R_SECTOR_NUMBER) != 0x01)
        klog (KL_DEBUG, "Sector number wrong after reset");

    select (drive, 0, 0, 0);

    send_command (Command::IDENTIFY);

    wait_while_busy();

    if (ata_in (R_LBA_MID) != 0 || ata_in (R_LBA_HI) != 0)
        return false;

    uint8 status;

    if (! wait_until_ready_or_error())
        return false; /* No drive. */

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
        klog (KL_DEBUG, "Drive failed to assert busy flag during reset.");

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
    /* TODO - introduce some timeout. */
    while (is_busy()) {}
}

bool ATAControllerPIO::wait_until_ready_or_error()
{
    uint8 s; 
    do
    {
        s = read_status();
    } while (! (bit_set(s, DRQ) || bit_set(s, ERR))); 

    return (! bit_set(s, ERR)); 
}
