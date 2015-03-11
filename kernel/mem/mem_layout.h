#pragma once
/**
 * Basic functions and constants that specify our memory layout. 
 */ 

const uintptr_t kPageSize = 0x1000; 

struct MemoryRange 
{
    uintptr_t base; 
    uintptr_t end; 
    uintptr_t base_addr() const { return base; }
    uintptr_t size() const { return base - end; }
}; 

struct PhysicalMemoryRange 
{
    uintptr_t base; 
    uintptr_t end; 
    uintptr_t base_addr() const { return base; }
    uintptr_t size() const { return base - end; }
}; 



/***********************/
/*** PHYSICAL MEMORY ***/
const PhysicalMemoryRange  kPRangeBaseMemory       { 0x0,          0x100000 }; 
const PhysicalMemoryRange  kPRangePageDirectory    { 0x100000,     0x400000 }; 
const PhysicalMemoryRange  kPRangeGeneral          { 0x400000,   0xFFFFFFFF }; 

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
