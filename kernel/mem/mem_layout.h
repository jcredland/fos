#pragma once
#include <klibrary/kstring.h>
/**
 * Basic functions and constants that specify our memory layout. 
 */ 

const uintptr_t kPageSize = 0x1000; 
const uintptr_t kThreadStackSize = 0x4000; /* 16kb. */

struct MemoryRange 
{
    uintptr_t base; 
    uintptr_t end; 
    uintptr_t base_addr() const { return base; }
    uintptr_t size() const { return end - base; }
    KString to_string() const
    {
        return KString((uint32)base) + " to " + KString((uint32)end);
    }
}; 

struct PhysicalMemoryRange 
{
    uintptr_t base; 
    uintptr_t end; 
    uintptr_t base_addr() const { return base; }
    uintptr_t size() const { return end - base; }
}; 



/***********************/
/*** PHYSICAL MEMORY ***/
const PhysicalMemoryRange  kPRangeBaseMemory       { 0x0,          0x100000 }; 
const PhysicalMemoryRange  kPRangePageDirectory    { 0x100000,     0x400000 }; 
const PhysicalMemoryRange  kPRangeGeneral          { 0x400000,   0xFFFFF000 };  /* should be FFF but we deal in pages. */

/****************************************/
/*** KERNEL MEMORY SPACE DEFINITIONS. ***/

/** KERNEL MEMORY - IDENTITY MAPPED SPACE */
const MemoryRange  kIdentityMappedRange    { 0x0,          0x400000 }; 


/** KERNEL MEMORY - VIRTUAL ALLOCATION */
const MemoryRange  kVRangeKernel           { 0x400000,     0x20000000 }; 
const MemoryRange  kVRangeKernelHeap       { 0x400000,     0x800000 }; 


/***************************/
/*** USER PROCESS MEMORY ***/
const MemoryRange  kVRangeUser             { 0x20000000,   0xFFFFFFFF }; 

template <typename T>
inline unsigned bytes_to_pages(T num_bytes)
{
    kassert((num_bytes % 0x1000) == 0);
    return num_bytes / 0x1000; 
}
