#include <hw/pci_devices.h>

struct PciDeviceClass
{
    uint8 class_code; 
    const char * description; 
};

PciDeviceClass pci_device_class_table[] = 
{
        {0x00, "Unclassified device"},
        {0x01, "Mass storage controller"},
        {0x02, "Network controller"},
        {0x03, "Display controller"},
        {0x04, "Multimedia controller"},
        {0x05, "Memory controller"},
        {0x06, "Bridge"},
        {0x07, "Communication controller"},
        {0x08, "Generic system peripheral"},
        {0x09, "Input device controller"},
        {0x0a, "Docking station"},
        {0x0b, "Processor"},
        {0x0c, "Serial bus controller"},
        {0x0d, "Wireless controller"},
        {0x0e, "Intelligent controller"},
        {0x0f, "Satellite communications controller"},
        {0x10, "Encryption controller"},
        {0x11, "Signal processing controller"},
        {0x12, "Processing accelerators"},
        {0x13, "Non-Essential Instrumentation"},
        {0xff, "Unassigned class"}
};

struct PciDeviceTypeRecord 
{
    uint8 class_code;
    uint8 subclass_code;
    uint16 prog_if;
    const char * description;

    bool matches(uint8 class_c, uint8 subclass_c, uint8 p_if) const
    {
        if (class_c == class_code && subclass_c == subclass_code)
            if (prog_if == p_if || prog_if == 0xFFFF)
                return true; 
        return false;
    }
};

PciDeviceTypeRecord pci_device_type_table[] = 
{
    {0x00, 0x00, 0x00, "Any device except for VGA-Compatible devices"},
    {0x00, 0x01, 0x00, "VGA-Compatible Device"},
    {0x01, 0x00, 0x00, "SCSI Bus Controller"},
    {0x01, 0x01, 0xFFFF, "IDE Controller"},
    {0x01, 0x02, 0x00, "Floppy Disk Controller"},
    {0x01, 0x03, 0x00, "IPI Bus Controller"},
    {0x01, 0x04, 0x00, "RAID Controller"},
    {0x01, 0x05, 0x20, "ATA Controller (Single DMA)"},
    {0x01, 0x05, 0x30, "ATA Controller (Chained DMA)"},
    {0x01, 0x06, 0x00, "Serial ATA (Vendor Specific Interface)"},
    {0x01, 0x06, 0x01, "Serial ATA (AHCI 1.0)"},
    {0x01, 0x07, 0x00, "Serial Attached SCSI (SAS)"},
    {0x01, 0x80, 0x00, "Other Mass Storage Controller"},
    {0x02, 0x00, 0x00, "Ethernet Controller"},
    {0x02, 0x01, 0x00, "Token Ring Controller"},
    {0x02, 0x02, 0x00, "FDDI Controller"},
    {0x02, 0x03, 0x00, "ATM Controller"},
    {0x02, 0x04, 0x00, "ISDN Controller"},
    {0x02, 0x05, 0x00, "WorldFip Controller"},
    {0x02, 0x06, 0xFFFF, "PICMG 2.14 Multi Computing"},
    {0x02, 0x80, 0x00, "Other Network Controller"},
    {0x03, 0x00, 0x00, "VGA-Compatible Controller"},
    {0x03, 0x00, 0x01, "8512-Compatible Controller"},
    {0x03, 0x01, 0x00, "XGA Controller"},
    {0x03, 0x02, 0x00, "3D Controller (Not VGA-Compatible)"},
    {0x03, 0x80, 0x00, "Other Display Controller"},
    {0x04, 0x00, 0x00, "Video Device"},
    {0x04, 0x01, 0x00, "Audio Device"},
    {0x04, 0x02, 0x00, "Computer Telephony Device"},
    {0x04, 0x80, 0x00, "Other Multimedia Device"},
    {0x05, 0x00, 0x00, "RAM Controller"},
    {0x05, 0x01, 0x00, "Flash Controller"},
    {0x05, 0x80, 0x00, "Other Memory Controller"},
    {0x06, 0x00, 0x00, "Host Bridge"},
    {0x06, 0x01, 0x00, "ISA Bridge"},
    {0x06, 0x02, 0x00, "EISA Bridge"},
    {0x06, 0x03, 0x00, "MCA Bridge"},
    {0x06, 0x04, 0x00, "PCI-to-PCI Bridge"},
    {0x06, 0x04, 0x01, "PCI-to-PCI Bridge (Subtractive Decode)"},
    {0x06, 0x05, 0x00, "PCMCIA Bridge"},
    {0x06, 0x06, 0x00, "NuBus Bridge"},
    {0x06, 0x07, 0x00, "CardBus Bridge"},
    {0x06, 0x08, 0xFFFF, "RACEway Bridge"},
    {0x06, 0x09, 0x40, "PCI-to-PCI Bridge (Semi-Transparent, Primary)"},
    {0x06, 0x09, 0x80, "PCI-to-PCI Bridge (Semi-Transparent, Secondary)"},
    {0x06, 0x0A, 0x00, "InfiniBrand-to-PCI Host Bridge"},
    {0x06, 0x80, 0x00, "Other Bridge Device"},
    {0x07, 0x00, 0x00, "Generic XT-Compatible Serial Controller"},
    {0x07, 0x00, 0x01, "16450-Compatible Serial Controller"},
    {0x07, 0x00, 0x02, "16550-Compatible Serial Controller"},
    {0x07, 0x00, 0x03, "16650-Compatible Serial Controller"},
    {0x07, 0x00, 0x04, "16750-Compatible Serial Controller"},
    {0x07, 0x00, 0x05, "16850-Compatible Serial Controller"},
    {0x07, 0x00, 0x06, "16950-Compatible Serial Controller"},
    {0x07, 0x01, 0x00, "Parallel Port"},
    {0x07, 0x01, 0x01, "Bi-Directional Parallel Port"},
    {0x07, 0x01, 0x02, "ECP 1.X Compliant Parallel Port"},
    {0x07, 0x01, 0x03, "IEEE 1284 Controller"},
    {0x07, 0x01, 0xFE, "IEEE 1284 Target Device"},
    {0x07, 0x02, 0x00, "Multiport Serial Controller"},
    {0x07, 0x03, 0x00, "Generic Modem"},
    {0x07, 0x03, 0x01, "Hayes Compatible Modem (16450-Compatible Interface)"},
    {0x07, 0x03, 0x02, "Hayes Compatible Modem (16550-Compatible Interface)"},
    {0x07, 0x03, 0x03, "Hayes Compatible Modem (16650-Compatible Interface)"},
    {0x07, 0x03, 0x04, "Hayes Compatible Modem (16750-Compatible Interface)"},
    {0x07, 0x04, 0x00, "IEEE 488.1/2 (GPIB) Controller"},
    {0x07, 0x05, 0x00, "Smart Card"},
    {0x07, 0x80, 0x00, "Other Communications Device"},
    {0x08, 0x00, 0x00, "Generic 8259 PIC"},
    {0x08, 0x00, 0x01, "ISA PIC"},
    {0x08, 0x00, 0x02, "EISA PIC"},
    {0x08, 0x00, 0x10, "I/O APIC Interrupt Controller"},
    {0x08, 0x00, 0x20, "I/O(x) APIC Interrupt Controller"},
    {0x08, 0x01, 0x00, "Generic 8237 DMA Controller"},
    {0x08, 0x01, 0x01, "ISA DMA Controller"},
    {0x08, 0x01, 0x02, "EISA DMA Controller"},
    {0x08, 0x02, 0x00, "Generic 8254 System Timer"},
    {0x08, 0x02, 0x01, "ISA System Timer"},
    {0x08, 0x02, 0x02, "EISA System Timer"},
    {0x08, 0x03, 0x00, "Generic RTC Controller"},
    {0x08, 0x03, 0x01, "ISA RTC Controller"},
    {0x08, 0x04, 0x00, "Generic PCI Hot-Plug Controller"},
    {0x08, 0x80, 0x00, "Other System Peripheral"},
    {0x09, 0x00, 0x00, "Keyboard Controller"},
    {0x09, 0x01, 0x00, "Digitizer"},
    {0x09, 0x02, 0x00, "Mouse Controller"},
    {0x09, 0x03, 0x00, "Scanner Controller"},
    {0x09, 0x04, 0x00, "Gameport Controller (Generic)"},
    {0x09, 0x04, 0x10, "Gameport Contrlller (Legacy)"},
    {0x09, 0x80, 0x00, "Other Input Controller"},
    {0x0A, 0x00, 0x00, "Generic Docking Station"},
    {0x0A, 0x80, 0x00, "Other Docking Station"},
    {0x0B, 0x00, 0x00, "386 Processor"},
    {0x0B, 0x01, 0x00, "486 Processor"},
    {0x0B, 0x02, 0x00, "Pentium Processor"},
    {0x0B, 0x10, 0x00, "Alpha Processor"},
    {0x0B, 0x20, 0x00, "PowerPC Processor"},
    {0x0B, 0x30, 0x00, "MIPS Processor"},
    {0x0B, 0x40, 0x00, "Co-Processor"},
    {0x0C, 0x00, 0x00, "IEEE 1394 Controller (FireWire)"},
    {0x0C, 0x00, 0x10, "IEEE 1394 Controller (1394 OpenHCI Spec)"},
    {0x0C, 0x01, 0x00, "ACCESS.bus"},
    {0x0C, 0x02, 0x00, "SSA"},
    {0x0C, 0x03, 0x00, "USB (Universal Host Controller Spec)"},
    {0x0C, 0x03, 0x10, "USB (Open Host Controller Spec"},
    {0x0C, 0x03, 0x20, "USB2 Host Controller (Intel Enhanced Host Controller Interface)"},
    {0x0C, 0x03, 0x80, "USB"},
    {0x0C, 0x03, 0xFE, "USB (Not Host Controller)"},
    {0x0C, 0x04, 0x00, "Fibre Channel"},
    {0x0C, 0x05, 0x00, "SMBus"},
    {0x0C, 0x06, 0x00, "InfiniBand"},
    {0x0C, 0x07, 0x00, "IPMI SMIC Interface"},
    {0x0C, 0x07, 0x01, "IPMI Kybd Controller Style Interface"},
    {0x0C, 0x07, 0x02, "IPMI Block Transfer Interface"},
    {0x0C, 0x08, 0x00, "SERCOS Interface Standard (IEC 61491)"},
    {0x0C, 0x09, 0x00, "CANbus"},
    {0x0D, 0x00, 0x00, "iRDA Compatible Controller"},
    {0x0D, 0x01, 0x00, "Consumer IR Controller"},
    {0x0D, 0x10, 0x00, "RF Controller"},
    {0x0D, 0x11, 0x00, "Bluetooth Controller"},
    {0x0D, 0x12, 0x00, "Broadband Controller"},
    {0x0D, 0x20, 0x00, "Ethernet Controller (802.11a)"},
    {0x0D, 0x21, 0x00, "Ethernet Controller (802.11b)"},
    {0x0D, 0x80, 0x00, "Other Wireless Controller"},
    {0x0E, 0x00, 0xFFFF, "I20 Architecture"},
    {0x0E, 0x00, 0x00, "Message FIFO"},
    {0x0F, 0x01, 0x00, "TV Controller"},
    {0x0F, 0x02, 0x00, "Audio Controller"},
    {0x0F, 0x03, 0x00, "Voice Controller"},
    {0x0F, 0x04, 0x00, "Data Controller"},
    {0x10, 0x00, 0x00, "Network and Computing Encrpytion/Decryption"},
    {0x10, 0x10, 0x00, "Entertainment Encryption/Decryption"},
    {0x10, 0x80, 0x00, "Other Encryption/Decryption"},
    {0x11, 0x00, 0x00, "DPIO Modules"},
    {0x11, 0x01, 0x00, "Performance Counters"},
    {0x11, 0x10, 0x00, "Communications Syncrhonization Plus Time and Frequency Test/Measurment"},
    {0x11, 0x20, 0x00, "Management Card"},
    {0x11, 0x80, 0x00, "Other Data Acquisition/Signal Processing Controller"}
};
    

KString PciDevice::get_description()
{
    for (auto d: pci_device_type_table)
    {
        if (d.matches(class_code, subclass_code, prog_if))
            return KString(d.description);
    }

    for (auto d: pci_device_class_table)
    {
        if (d.class_code == class_code)
            return KString("Unknown device of class: ") + KString(d.description); 
    }
    return KString("Unknown device"); 
}
