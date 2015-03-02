/*
 * Functions we normally take for granted with C++. 
 */

void * operator new(size_t size)
{
    return kheap.malloc(size); 
}

void * operator new[](size_t size)
{
    return kheap.malloc(size); 
}

void operator free(void * ptr)
{
    kheap.free(ptr); 
}

void operator free[](void * ptr)
{
    kheap.free(ptr); 
}
