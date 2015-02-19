

class VgaDriver
{
public:
    VgaDriver();
    
    void write(const char * string)
    {
        while (*string != 0)
            putc(*string++);
    }
    
    void putc(char c)
    {
        *(vmem + cursor * 2 + 1) = 0xF;
        *(vmem + cursor * 2) = c;
        cursor++;
        
        if (cursor >= 80 * 25)
            scrollOneLine();
    }
    
    void scrollOneLine()
    {
        cursor -= 80;
        
        uint16_t * src = vmemi + 80;
        
        for (int i = 0; i < 80 * 24; ++i)
            *(vmemi + i) = *(src + i);
        
        uint16_t * lastLine = vmemi + 80 * 24;
        
        for (int i = 0; i < 80; ++i)
            *(lastLine+i) = 0;
    }
    
    void updateCursorPositionFromHardware()
    {
        
    }
    
    void write(uint8_t x, uint8_t y, const char * string)
    {
        uint32_t offset = x + y * 80;
        char * v = vmem + offset * 2;
        while (*string != (char)0)
        {
            *v++ = *string++;
            *v++ = 0xF;
        }
    }
    
private:
    uint32_t cursor;
    
    char * const vmem =          (char *) 0xb8000;
    uint16_t * const vmemi = (uint16_t *) 0xb8000;
};
