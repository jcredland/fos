
/** Routines for setting and modifying the GDT.  This could be pretty fixed. Note
 *
 * If you are using the Intel SYSENTER/SYSEXIT routines, the GDT must be structured like this:
 *
 * Any descriptors preceding (null descriptor, special kernel stuff, etc.)
 * A DPL 0 code segment descriptor (the one that SYSENTER will use) 0x8
 * A DPL 0 data segment descriptor (for the SYSENTER stack)         0x10
 * A DPL 3 code segment (for the code after SYSEXIT)
 * A DPL 3 data segment (for the user-mode stack after SYSEXIT)
 * Any other descriptors
 */ 

class GlobalDescriptorTable
{
public:
    static void load_fos_table()
    {
    }
    struct F_PACKED Entry 
    {
        uint16 limit = 0xffff; 
        uint16 base  = 0x0;
        uint8  base2 = 0x0;
        uint8  flags1;
        uint8  flags2 : 4;
        uint8  limit2 : 4; 
        uint8  base; 
    };
private:
    static uint32 gdt[5] = 
    { 0, 0, 0, 0 }
        

};
