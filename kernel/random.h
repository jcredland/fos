

/** Code stolen from a snippet on intel's website.  This is not intended to go anywhere near any 
 * production code! */
class Random
{
public:
    Random()
    {
        seed = inb (0x40); /* Timer. */
    }

    uint32 seed;

    uint32 next_int()
    {
        seed = (214013 * seed + 2531011);
        return (seed >> 16) & 0x7FFF;
    }
};



