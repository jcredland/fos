#pragma once
#include <stdint.h>
#include <stdio.h>
#include "../std_headers.h"

enum class VgaColours
{
    black,
    dark_blue,
    dark_green,
    dark_cyan,
    dark_red,
    dark_magenta,
    dark_yellow,
    light_gray,
    dark_gray,
    blue,
    green,
    cyan,
    red,
    magenta,
    yellow,
    white
};

class VgaDriver
{
public:
    VgaDriver() {}

    
    /** Display a string and move to the next line. */
    void writeln(const KString & s)
    {
        printf("%s\n", s.get());
    }
    void write(const char * string)
    {
        printf("%s", string);
    }
    void write(const KString & ks)
    {
        printf("%s", ks.get());
    }
    
    void putc(char c);
    void write(int x, int y, const char * string);

    /** Set the location of the cursor use by the write functions. */
    void set_pos(int x, int y); 
    /** Set the video mode.  Probably all devices don't support all modes. */
    void set_mode(uint8 new_mode); 

    void set_colour(VgaColours colour) {}
    
private:
    uint8 current_colour; 

    void unlock_crtc_registers();
        
    void scroll_one_line();
    void update_cursor_position_from_hardware();
    int get_x() const; 
    int get_y() const; 

    uint32_t cursor;

    const uint32_t rows = 25;
    const uint32_t columns = 80;
    
    char * const vmem =          (char *) 0xb8000;
    uint16_t * const vmemi = (uint16_t *) 0xb8000;

    static const uint16 seq_misc_output_port_write = 0x3C2;
    static const uint16 seq_misc_output_port_read = 0x3CC;
    static const uint16 crtc_index_port = 0x3D4; 
    enum CRTC
    {
        VGA_MODE = 23
    };

    static const uint16 crtc_data_port = 0x3D5; 

};


extern VgaDriver vga; 
