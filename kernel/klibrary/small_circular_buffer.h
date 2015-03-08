
#include <std_types.h>

/**
 * A circular bufffer. The size must be a power of two.
 */
template <typename T, int size>
class SmallCircularBuffer
{
public:
    SmallCircularBuffer()
        :
        next_addition (0),
        next_removal (0)
    {}

    bool is_empty() const
    {
        return next_addition == next_removal;
    }

    bool push (T new_item)
    {
        if (next_addition - next_removal >= size)
            return false;

        data[ (mask & next_addition++)] = new_item;
        return true;
    }

    T next()
    {
        if (is_empty())
            return T();

        return data[ (mask & next_removal++)];
    }

private:
    static constexpr uint64 mask = size - 1;
    T data[size];
    uint64 next_addition;
    uint64 next_removal;
};

