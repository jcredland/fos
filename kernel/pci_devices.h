

class PciDevice
{
public:
    enum DeviceClasses
    {
        UNCLASSIFIED_DEVICE = 0, MASS_STORAGE_CONTROLLER = 1, NETWORK_CONTROLLER = 2, DISPLAY_CONTROLLER = 3, 
        MULTIMEDIA_CONTROLLER = 4, MEMORY_CONTROLLER, BRIDGE, COMMUNICATION_CONTROLLER, 
        GENERIC_SYSTEM_PERIPHERAL = 8, INPUT_DEVICE_CONTROLLER, DOCKING_STATION, PROCESSOR, 
        SERIAL_BUS_CONTROLLER = 12, WIRELESS_CONTROLLER, INTELLIGENT_CONTROLLER,
        SATELLITE_COMMUNICATIONS_CONTROLLER, ENCRYPTION_CONTROLLER, 
        SIGNAL_PROCESSING_CONTROLLER, PROCESSING_ACCELERATORS, NONESSENTIAL_INSTRUMENTATION = 0X13,
        UNASSIGNED_CLASS = 0xff 
    };

    /** Create a new device. */
    PciDevice(uint8_t class_code, uint8_t subclass_code, uint8_t prog_if)
        :
            class_code(class_code), 
            subclass_code(subclass_code),
            prog_if(prog_if)
    {}

    /** Return a text description of the device. */
    KString get_description();

private:
    uint8 class_code, subclass_code, prog_if;

};

