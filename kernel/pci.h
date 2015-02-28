
/** Driver for the PCI bus. */
class PciBus
{
public:
    /** Find all the PCI devices. */
    void search_for_devices()
    {
        for (uint32_t bus = 0; bus < 256; ++bus)
            for (uint32_t slot = 0; slot < 32; ++slot)
                check_device(bus, slot); 
    }

private:
    static const uint16_t config_addr_port = 0xCF8;
    static const uint16_t config_data_port = 0xCFC; 



    class BaseAddressRegister
    {
        enum Type 
        {
            BITS32, 
            RESERVED,
            BITS64
        };
        BaseAddressRegister(uint32_t value) : value(value)
        {}
        /** If it's not a memory space BAR then it's an IO space one. */
        bool is_memory_space() { return (value & 0x1) == 0; }
        Type get_type() { return (Type) ((value & 0b110) >> 1); }
        uint32_t get_addr()
        {
            return 0;
        }
        uint32_t value;
    };

    /** Represents an address in the PCI system including an offset into the configuration map. */
    struct PciAddress  /* could be full of bytes really? */
    {
        uint32_t bus;
        uint32_t slot; 
        uint32_t function; 
        uint32_t offset;
        /** Creates an address for the PCI config_addr_port.  Note that the top bit is always set. */
        uint32_t get_aligned_address() const
        {
            return (bus << 16) | (slot << 11) | (function << 8) | (offset & 0xfc) | 0x80000000; 
        }
        /** Actually send this address to the PCI port with all the correct alignment. */
        void write_to_port() const
        {
            outl(config_addr_port, get_aligned_address()); 
        }
        PciAddress with_offset(uint32_t new_offset)
        {
            PciAddress a = *this; 
            a.offset = new_offset;
            return a; 
        }
    };

    uint32_t get_configuration_long(const PciAddress & addr)
    {
        /* @todo: clear interrupts between this write and read to avoid any unexpected reconfig. */
        addr.write_to_port();
        return inl(config_data_port); /* Recover the correct byte(s). */
    }

    uint16_t get_configuration_word(const PciAddress & addr)
    {
        /* @todo: clear interrupts between this write and read to avoid any unexpected reconfig. */
        addr.write_to_port();
        return inl(config_data_port + (addr.offset & 2)); /* Recover the correct byte(s). */
    }

    uint8_t get_configuration_byte(const PciAddress & addr)
    {
        /* @todo: clear interrupts between this write and read to avoid any unexpected reconfig. */
        addr.write_to_port();
        return inl(config_data_port + (addr.offset & 3)); /* Recover the correct byte(s). */
    }

    void check_device(uint32_t bus, uint32_t slot)
    {
        /* Every device should have a function 0. */
        for (uint32_t function = 0; function < 8; ++function)
        {
            PciAddress addr {bus, slot, function, 0};  /*< todo: should this be zero. */
            uint32_t vendor_info = get_configuration_long(addr); 
            if ((vendor_info & 0xFFFF) != 0xFFFF)
            {
                KString str = KString("PCI device found: ") + KString((uint8_t) slot) + " (devid_vendorid):"  + KString(vendor_info); 
                vga.write(str + " "); 
                
                PciAddress class_addr = addr.with_offset(8); 
                uint32_t class_info = get_configuration_long(class_addr); 

                uint8 pci_class = class_info >> 24; 
                uint8 pci_subclass = (class_info >> 16) & 0xFF; 
                uint8 pci_prog_if = class_info & 0xFF;

                PciDevice pci_device(pci_class, pci_subclass, pci_prog_if); 
                vga.writeln(pci_device.get_description());     
            }
        }
    }
};


