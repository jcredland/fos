
class DeviceInterruptHandler
{
public:
    virtual ~DeviceInterruptHandler() {}
    virtual void handle_interrupt(uint8 interrupt_number) = 0; 
};
/** 
 * A device is something that can be accessed from user or kernel land. 
 * The kernel maintains a list of available devices. 
 *
 * Each device implements some basic functions to allow user and kernel 
 * processes to interact with it. 
 *
 * Devices may allow only one, or multiple processes to get access to 
 * data.
 */
class Device
{
    public:
        virtual ~Device() {}
        /** Returns the friendly name of the device. */
        virtual const char * get_device_name() const = 0;
        /** Get the next item from the device's buffer.  Returns -1 if there is no character waiting. */
        virtual int read_char() = 0;
    protected:
        /** Install a handler for an interrupt. */
        void install_interrupt_handler(uint8 interrupt_number, DeviceInterruptHandler * handler);
    private:
};

/**
 * The DeviceManager provides a record of all devices loaded on the system. 
 */

class DeviceManager
{
    public:
        void register_device(Device * new_device)
        {
            registered_devices.push_back(new_device); 
        }

        Device * get_device_by_name(const char * name)
        {
            for (auto dptr: registered_devices)
                if (strcmp(name, dptr->get_device_name()) == 0)
                    return dptr; 

            return nullptr; 
        }

    private:
        kstd::kvector<Device *> registered_devices; 
};

extern DeviceManager device_manager;

