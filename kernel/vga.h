
#include <stdint.h>

class VgaDriver
{
public:
    VgaDriver()
    {
    }
    
    void write(uint8_t x, uint8_t y, char * string)
    {
        uint32_t offset = x + y * 80;
        char * v = vmem + offset;
        while (*string != (char)0)
        {
            *v++ = 0xF;
            *v++ = *string++;
        }
    }
    
private:
    uint32_t cursor;
    
    char * const vmem = (char *) 0xb8000;
};
