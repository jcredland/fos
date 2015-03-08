#pragma once
#include <klibrary/klibrary.h>
#include <interrupt/interrupt.h>
/**
 * The keyboard hardware driver takes scan codes and provides a stream of 
 * KeyboardKeyCodes. 
 *
 * Right now it assumes a one to one mapping between scan codes and key codes.
 * However in future a look-up table will be provided which allows keyboards 
 * with different scan codes to work. 
 */
class KeyboardHardwareDriver
    :
    public Device,
    public DeviceInterruptHandler
{
public:
    KeyboardHardwareDriver();
    const char* get_device_name() const;
    int read_char() override;
    void handle_interrupt (uint8 /* interrupt_number */) override;

private:
    SmallCircularBuffer<int, 8> input_buffer;
    int last_scan_code; /* should be a buffer. */
    const char* kDeviceName = "keyboard";
};

/** Function called by the interrupt stub. */
void interrupt_keyboard (int);


