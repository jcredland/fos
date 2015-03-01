
/** A simple string function that isn't dependent on any memory allocation being
 * available.
 */
class KString
{
public:
    KString()
    {
        buf[0] = 0;
        buf[MAX_STRING_LENGTH] = 0;
    }

    KString (const char* string)
    {
        copyFrom (buf, string, MAX_STRING_LENGTH);
    }

    /** Copy at most maxLength characters from a buffer string to the KString.
     * If the limit is reached before a null terminating character then a null
     * terminating character will be added. 
     * */
    KString (const char* string, int maxLength)
    {
        if (maxLength < MAX_STRING_LENGTH)
        {
            buf[maxLength] = 0;
            copyFrom (buf, string, maxLength);
        }
        else
        {
            copyFrom (buf, string, MAX_STRING_LENGTH);
        }
    }

    KString (uint64_t hexNumber) : KString()
    {
        appendHex (hexNumber);
    }
    KString (uint32_t hexNumber) : KString()
    {
        appendHex (hexNumber);
    }
    KString (uint16_t hexNumber) : KString()
    {
        appendHex (hexNumber);
    }
    KString (uint8_t  hexNumber) : KString()
    {
        appendHex (hexNumber);
    }

    void appendHex (uint64_t hex)
    {
        appendHex (hex, 16);
    }
    void appendHex (uint32_t hex)
    {
        appendHex (hex, 8);
    }
    void appendHex (uint16_t hex)
    {
        appendHex (hex, 4);
    }
    void appendHex (uint8_t  hex)
    {
        appendHex (hex, 2);
    }

    void append_char (char c)
    {
        int len = length();

        if (len >= MAX_STRING_LENGTH)
            return;

        buf[len] = c;
        buf[len + 1] = 0;
    }

    void appendHex (uint64_t hex, int num_digits)
    {
        int blanks = num_digits / 4 - 1;

        if (blanks < 0)
            blanks = 0;

        char tb[num_digits + 1 + blanks];

        int i = num_digits;
        int string_idx = num_digits + blanks;

        while (i--)
        {
            string_idx--;

            tb[string_idx] = getDigit (0xF & hex);
            hex = hex >> 4;

            if (i != 0 && (i % 4) == 0)
                tb[--string_idx] = '_';
        }

        tb[num_digits + blanks] = 0;

        int len = length();
        copyFrom (buf + len, tb, MAX_STRING_LENGTH - len);
    }

    KString (const KString& rhs)
    {
        copyFrom (buf, rhs.get(), MAX_STRING_LENGTH);
    }

    KString& operator+ (const KString& rhs)
    {
        append (rhs);
        return *this;
    }

    KString& operator+= (const KString& rhs)
    {
        append (rhs);
        return *this;
    }


    /** Get a C string array. */
    const char* get() const
    {
        return buf;
    }

    void append (const KString& rhs)
    {
        int len = length();
        copyFrom (buf + len, rhs.get(), MAX_STRING_LENGTH - len);
    }

    /** Removes white-space characters from the end of the string. */
    void trim_end()
    {
        int len = length();
        
        if (len == 0)
            return;
        
        char * p = buf + len - 1;
        
        while (is_whitespace(*p))
        {
            *p = 0;
            
            if (p == buf)
                break;
            
            p--;
        }
    }
    static bool is_whitespace(char c)
    {
        return c == ' ' || c == '\t';
    }

    enum { MAX_STRING_LENGTH = 200 };

    /** Returns the length of the string. */
    int length() const
    {
        int len = 0;
        const char* b = buf;

        while (*b++ != 0)
            ++len;

        return len;
    }
private:
    static char getDigit (uint8_t number)
    {
        if (number < 10)
            return number + '0';
        else
            return number + ('A' - 10);
    }

    void copyFrom (char* dest, const char* source, int count)
    {
        char* b = dest;

        while (count--)
        {
            *b = *source++;

            if (*b++ == '\0')
                break;
        }
    }

    char buf[MAX_STRING_LENGTH + 1];
};


