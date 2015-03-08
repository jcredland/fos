#pragma once
void * operator new(size_t /*size*/, void* ptr);
void * operator new(size_t size);
void * operator new[](size_t size);
void operator delete(void * ptr);
void operator delete[](void * ptr);

