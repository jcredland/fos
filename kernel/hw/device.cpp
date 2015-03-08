
void Device::install_interrupt_handler(uint8 interrupt_number, DeviceInterruptHandler * handler)
{
    interrupt_driver.set_handler(interrupt_number, handler); 
}

