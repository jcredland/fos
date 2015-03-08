#include <std_headers.h>
#include <display/vga.h>
VgaDriver::VgaDriver()
:
cursor(0)
{
}

void VgaDriver::writeln(const KString & ks)
{
    write(ks); 

    int y = get_y(); 
    int x = get_x(); 

    if (x == 0) 
        return;

    y++;

    if (y >= 25)
    {
        scroll_one_line(); 
        set_pos(0, 24); 
    }
    else
    {
        set_pos(0, y); 
    }
}

int VgaDriver::get_x() const
{
    return cursor % columns;
}

int VgaDriver::get_y() const
{
    return cursor / columns; 
}


void VgaDriver::write(const char * string)
{
    while (*string != 0)
        putc(*string++);
}

void VgaDriver::write(const KString & ks)
{
    write(ks.get()); 
}

void VgaDriver::putc(char c)
{
    *(vmem + cursor * 2 + 1) = current_colour;
    *(vmem + cursor * 2) = c;
    cursor++;
    
    if (cursor >= 80 * 25)
    {
        scroll_one_line();
        set_pos(0, 24);
    }

}

void VgaDriver::set_pos(int x, int y)
{
    cursor = x + y * columns; 
}

void VgaDriver::scroll_one_line()
{
    uint16_t * src = vmemi + columns;

    uint16_t mem_to_move = columns * (rows - 1); 
    
    for (int i = 0; i < mem_to_move; ++i)
        *(vmemi + i) = *(src + i);
    
    uint16_t * lastLine = vmemi + mem_to_move; 
    
    for (uint16 i = 0; i < columns; ++i)
        *(lastLine+i) = 0;
}

void VgaDriver::update_cursor_position_from_hardware()
{
    
}

void VgaDriver::unlock_crtc_registers()
{
    outb(crtc_index_port, 0x03);
    outb(crtc_data_port, inb(crtc_data_port) | 0x80);
    outb(crtc_index_port, 0x11);
    outb(crtc_data_port, inb(crtc_data_port) & ~0x80);
}

unsigned char g_80x50_text[] =
{
/* MISC */
    0x67,
/* SEQ */
    0x03, 0x00, 0x03, 0x00, 0x02,
/* CRTC */
    0x5F, 0x4F, 0x50, 0x82, 0x55, 0x81, 0xBF, 0x1F,
    0x00, 0x47, 0x06, 0x07, 0x00, 0x00, 0x01, 0x40,
    0x9C, 0x8E, 0x8F, 0x28, 0x1F, 0x96, 0xB9, 0xA3,
    0xFF, 
/* GC */
    0x00, 0x00, 0x00, 0x00, 0x00, 0x10, 0x0E, 0x00,
    0xFF, 
/* AC */
    0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x14, 0x07,
    0x38, 0x39, 0x3A, 0x3B, 0x3C, 0x3D, 0x3E, 0x3F,
    0x0C, 0x00, 0x0F, 0x08, 0x00,
};

void VgaDriver::set_mode(uint8 new_mode)
{
    outb(seq_misc_output_port_write, 0x67); 

    unlock_crtc_registers();

    outb(crtc_index_port, VGA_MODE); 
    outb(crtc_data_port, new_mode); 
}

void VgaDriver::write(int x, int y, const char * string)
{
    uint32_t offset = x + y * 80;
    char * v = vmem + offset * 2;
    while (*string != (char)0)
    {
        *v++ = *string++;
        *v++ = 0xF;
    }
}

void VgaDriver::set_colour(VgaColours colour)
{
    current_colour = (uint8) colour; 
}

