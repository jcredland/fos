#include <std_headers.h>
#include <mem/kmalloc.h>
#include <klibrary/new.h>

extern MemoryPool<64> kheap; 

/*
 * Functions we normally take for granted with C++. 
 */

void * operator new(size_t /*size*/, void* ptr) 
{
   return ptr; 
}

void * operator new(size_t size)
{
    return kheap.malloc(size); 
}

void * operator new[](size_t size)
{
    return kheap.malloc(size); 
}

void operator delete(void * ptr)
{
    kheap.free(ptr); 
}

void operator delete[](void * ptr)
{
    kheap.free(ptr); 
}

/** And some stuff we aren't actually going to have called as the kernle
 * isn't going to exit like a normal application. */
extern "C" {
int __cxa_atexit(void (*)(void *), void *, void *)
{
    return 0;
}
void __cxa_finalize(void *)
{
}
void *__dso_handle;
};

extern "C" void __cxa_pure_virtual()
{
    kerror("Pure virtual function call"); 
}
