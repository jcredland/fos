/** The kstd namespace contains a very limited set of the C++ standard classes and
 * functions as required for the kernel.
 */
namespace kstd
{
template <typename T> struct remove_reference
{
    typedef T type;
};
template <typename T> struct remove_reference<T&>
{
    typedef T type;
};
template <typename T> struct remove_reference < T&& >
{
    typedef T type;
};

/** The std::move operation. */
template <typename T>
typename remove_reference<T>::type&& move (T&& arg)
{
    return static_cast < typename remove_reference<T>::type && > (arg);
}

/** 
 * A vector, stored on the kernels main free store 'kheap'
 *
 * Useful reference:
 * http://codereview.stackexchange.com/questions/60484/stl-vector-implementation
 */
template <typename T>
class kvector
{
public:
    typedef T* iterator;

    kvector(size_t capacity = 16)
        :
        begin_ptr   (new T[capacity]),
        end_ptr     (begin_ptr),
        end_cap_ptr (begin_ptr + capacity)
    {}

    ~kvector()
    {
        delete[] begin_ptr;
    }

    void push_back (const T& object)
    {
        if (end_ptr == end_cap_ptr)
            grow();

        *end_ptr++ = object;
    }

    T & operator[] (size_t pos)
    {
        return begin_ptr[pos]; 
    }

    iterator erase(const iterator first, const iterator last)
    {
//        kassert(first <= last); 

        if (first != last)
        {
            auto num_to_move = end_ptr - last;
            auto distance = last - first;
            auto last_it = first + num_to_move;
   
            iterator it; 
            for (it = first; it < last_it; ++it)
                *it = move(*(it + distance));
            
            end_ptr -= distance;
            return it; 
        }
        
        return last; 
    }

    size_t size() const
    {
        return end_ptr - begin_ptr;
    }
    size_t capacity() const
    {
        return end_cap_ptr - begin_ptr;
    }
    iterator begin() 
    {
        return begin_ptr;
    }
    iterator end()
    {
        return end_ptr;
    }

private:
    T* begin_ptr;
    T* end_ptr;
    T* end_cap_ptr;

    /* This will invalidate all the iterators. */
    void grow()
    {
        size_t new_capacity = capacity() * 2;
        T* new_ptr = new T[new_capacity];
        T* p = new_ptr;
        size_t size_now = size();

        for (auto& item : *this)
            * (p++) = move(item);

        delete[] begin_ptr;

        begin_ptr   = new_ptr;
        end_cap_ptr = new_ptr + new_capacity;
        end_ptr     = new_ptr + size_now;
    }
};

};

