#include <assert.h>
#include <iostream>
#include "../kvector.h"


int main ()
{
    kstd::kvector<int> kv1; 

    for (int i = 0; i < 20; ++i)
        kv1.push_back(i); 

    std::cout << kv1.size() << std::endl; 
    assert(kv1.size() == 20); 
    assert(kv1.capacity() == 32); 
    assert(kv1[5] == 5); 

}
