

class VgaDriver
{
public:
    VgaDriver();
    
    /** Display a string and move to the next line. */
    void writeln(const KString &);
    void write(const char * string);
    void write(const KString & ks);
    
    void putc(char c);
    void write(int x, int y, const char * string);

    /** Set the location of the cursor use by the write functions. */
    void set_pos(int x, int y); 
    
private:
    void scroll_one_line();
    void update_cursor_position_from_hardware();
    int get_x() const; 
    int get_y() const; 

    uint32_t cursor;

    const uint32_t rows = 25;
    const uint32_t columns = 80;
    
    char * const vmem =          (char *) 0xb8000;
    uint16_t * const vmemi = (uint16_t *) 0xb8000;
};


extern VgaDriver vga; 
