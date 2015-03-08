#pragma once
#include <klibrary/klibrary.h>
#include <mem/physical.h>

/* 
 * From the intel documentation:
 *
 All three paging modes translate linear addresses use hierarchical
 paging structures. This section provides an overview of their
 operation. Section 4.3, Section 4.4, and Section 4.5 provide details
 for the three paging modes.

 Every paging structure is 4096 Bytes in size and comprises a
 number of individual entries. With 32-bit paging, each entry is
 32 bits (4 bytes); there are thus 1024 entries in each structure.
 With PAE paging and IA-32e paging, each entry is 64 bits (8 bytes);
 there are thus 512 entries in each structure.  (PAE paging includes
 one exception, a paging structure that is 32 bytes in size,
 containing 4 64-bit entries.)

 The processor uses the upper portion of a linear address to
 identify a series of paging-structure entries. The last of these
 entries identifies the physical address of the region to which the
 linear address translates (called the page frame). The lower portion
 of the linear address (called the page offset) identifies the
 specific address within that region to which the linear address
 translates.

 Each paging-structure entry contains a physical address, which is
 either the address of another paging structure or the address of
 a page frame. In the first case, the entry is said to reference
 the other paging structure; in the latter, the entry is said to
 map a page.

 The first paging structure used for any translation is located at
 the physical address in CR3. A linear address is translated using
 the following iterative procedure. A portion of the linear address
 (initially the uppermost bits) select an entry in a paging structure
 (initially the one located using CR3). If that entry references
 another paging structure, the process continues with that paging
 structure and with the portion of the linear address immediately
 below that just used. If instead the entry maps a page, the process
 completes: the physical address in the entry is that of the page
 frame and the remaining lower portion of the linear address is the
 page offset.
 */

/* The default mode is non-PAE paging. */
extern "C" {
void paging_enable();


};

template <typename T>
void check_page_alignment(T * ptr)
{
    uintptr_t p = (uintptr_t) ptr; 
    if ((p & 0xFFFFFF000) != p)
    {
        kerror("page alignment problem. halting."); 
        while (1) {}
    }
}

void check_page_alignment(uintptr_t p)
{
    if ((p & 0xFFFFFF000) != p)
    {
        kerror("page alignment problem. halting."); 
        while (1) {}
    }
}

/** 
Page tables should be allocated as raw physical memory pages. 
*/
class PageTable32
{
    public:
        PageTable32()
        {
            check_page_alignment(this); 
        }

        /* Handy definition from 
           http://www.jamesmolloy.co.uk/tutorial_html/6.-Paging.html 
           
           Actually - this doesn't look right!  See tables 4-5 and 4-6 of the
           intel processor manual. Seems to be mssing PWT and PCD.

           */
        struct Entry 
        {
            Entry()
            {}
            void set_physical_address(uintptr_t physical_address)
            {
                check_page_alignment(physical_address); 
                value = (value & 0xFFF) | physical_address;
            }
            void set_present(bool is_present)
            {
                if (is_present)
                    value = (value & 0xFFFFFFFE) | 1; 
                else
                    value = (value & 0xFFFFFFFE); 
            }
            uint32 value;
        };

    Entry & operator[] (int index)
    {
        return data[index];
    }

    private:
        /* Page aligned structure. */
        Entry data[1024];
};

/** Must be aligned to a 4k page boundary. */
class PageDirectory32
{
    public:
        PageDirectory32()
        {
            check_page_alignment(this); 
        }
    struct Entry
    {
        uint32 value; 
        PageTable32 * get_linked_table()
        {
            return reinterpret_cast<PageTable32 *>(value & 0xFFFFF000); 
        }
        void set_linked_table(PageTable32 * physical_addr)
        {
            check_page_alignment(physical_addr); 
            value = (value & 0xFFF) | reinterpret_cast<uintptr_t>(physical_addr);
        }
        bool is_present() const
        {
            return (value & 0x1) == 1; 
        }
        void set_present(bool is_present)
        {
            if (is_present)
                value = (value & 0xFFFFFFFE) | 1; 
            else
                value = (value & 0xFFFFFFFE); 
        }

    };

    Entry & operator[] (int index)
    {
        return data[index];
    }
    private:
        Entry data[1024];
};

class PagingManager
{
    public:
        PagingManager()
        {
            setup_inital_paging(); 
            paging_enable(); 
        }

        void setup_inital_paging()
        { 
            page_dir = new ((PageDirectory32 *) pmem.get_4k_page()) PageDirectory32();
        }

        /** 
         * Maps a linear address to a page.  
         *
         * The physical_address must be page aligned. 
         */
        void map_page(uintptr_t linear_address, uintptr_t physical_address)
        {
            PageTable32 * table = nullptr; 
            auto & dir_entry = (*page_dir)[get_page_dir_index(linear_address)];

            if (! dir_entry.is_present())
            {
                table = reinterpret_cast<PageTable32 *>(pmem.get_4k_page());
                dir_entry.set_linked_table(table); 
                dir_entry.set_present(true); 
            }
            else
            {
                table = dir_entry.get_linked_table(); 
            }

            auto & table_entry = (*table)[get_page_table_index(linear_address)];
            table_entry.set_physical_address(physical_address); 
            table_entry.set_present(true); 
        }

    private:
        /* INTEL: If a paging-structure entry maps a page when more than
        12 bits remain in the linear address, the entry identifies
        a page frame larger than 4 KBytes. For example, 32-bit
        paging uses the upper 10 bits of a linear address to locate
        the first paging-structure entry; 22 bits remain. */
        uintptr_t get_page_dir_index(uintptr_t linear_address) const
        {
            return linear_address >> 22; 
        }

        /* After this operation to get the next ten bits, only 12 bits 
         * remain. These are the offset into the physical page that
         * this page table points to. */
        uintptr_t get_page_table_index(uintptr_t linear_address) const
        {
            return (linear_address >> 12) & 0x3FF;
        }

        PageDirectory32 * page_dir;


};
