
/** A simple string function that isn't dependent on any memory allocation being 
 * available. 
 */
class KernelString
{
public:
    KernelString()
    {
        buf[0] = 0; 
        buf[MAX_STRING_LENGTH] = 0;
    }

    KernelString(const char * string)
    {
        copyFrom(buf, string, MAX_STRING_LENGTH);
    }

    void appendHex(uint32_t hex)
    {
        char tb[9];
        int i = 8; 
        while (i--)
        {
           tb[i] = getDigit(0xF & hex); 
           hex = hex >> 8; 
        }
        tb[9] = 0; 

        int len = length();
        copyFrom(buf + len, tb, MAX_STRING_LENGTH - len); 
    }

    KernelString(const KernelString & rhs)
    {
       copyFrom(buf, rhs.get(), MAX_STRING_LENGTH); 
    }

    KernelString & operator+ (const KernelString & rhs)
    {
        append(rhs); 
        return *this;
    }

    /** Get a C string array. */
    const char * get() const { return buf; }

    void append(const KernelString & rhs)
    {
        int len = length();
        copyFrom(buf + len, rhs.get(), MAX_STRING_LENGTH - len); 
    }

    enum { MAX_STRING_LENGTH = 200 };

    /** Returns the length of the string. */
    int length() const
    {
        int len = 0; 
        const char * b = buf; 
        while (*b++ != 0)
            ++len;

        return len; 
    }
private:
    static char getDigit(uint8_t number)
    {
       if (number < 10)
          return number + '0';
       else 
          return number + ('A' - 10);
    }

    void copyFrom(char * dest, const char * source, int count)
    {
        char * b = dest; 
        while (count--)
        {
            *b = *source++;
            if (*b++ == '\0') 
                break;
        }
    }

    char buf[MAX_STRING_LENGTH + 1];
};

