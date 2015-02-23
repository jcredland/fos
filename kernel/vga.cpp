
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
    *(vmem + cursor * 2 + 1) = 0xF;
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
    
    for (int i = 0; i < columns; ++i)
        *(lastLine+i) = 0;
}

void VgaDriver::update_cursor_position_from_hardware()
{
    
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


VgaDriver vga;


