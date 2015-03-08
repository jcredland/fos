#pragma once 
/** A simple string function that isn't dependent on kmalloc being available.  Limited to a fixed number of characters. */
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
        copy_from (buf, string, MAX_STRING_LENGTH);
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
            copy_from (buf, string, maxLength);
        }
        else
        {
            copy_from (buf, string, MAX_STRING_LENGTH);
        }
    }

    KString (uint64_t hex_number) : KString()
    {
        append_hex (hex_number);
    }
    KString (uint32_t hex_number) : KString()
    {
        append_hex (hex_number);
    }
    KString (uint16_t hex_number) : KString()
    {
        append_hex (hex_number);
    }
    KString (uint8_t  hex_number) : KString()
    {
        append_hex (hex_number);
    }

    void append_hex (uint64_t hex)
    {
        append_hex (hex, 16);
    }
    void append_hex (uint32_t hex)
    {
        append_hex (hex, 8);
    }
    void append_hex (uint16_t hex)
    {
        append_hex (hex, 4);
    }
    void append_hex (uint8_t  hex)
    {
        append_hex (hex, 2);
    }

    void append_char (char c)
    {
        int len = length();

        if (len >= MAX_STRING_LENGTH)
            return;

        buf[len] = c;
        buf[len + 1] = 0;
    }

    void append_hex (uint64_t hex, int num_digits)
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

            tb[string_idx] = get_digit (0xF & hex);
            hex = hex >> 4;

            if (i != 0 && (i % 4) == 0)
                tb[--string_idx] = '_';
        }

        tb[num_digits + blanks] = 0;

        int len = length();
        copy_from (buf + len, tb, MAX_STRING_LENGTH - len);
    }

    KString (const KString& rhs)
    {
        copy_from (buf, rhs.get(), MAX_STRING_LENGTH);
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
        copy_from (buf + len, rhs.get(), MAX_STRING_LENGTH - len);
    }

    /** Remove a single character from the end of the string. */
    void remove_last_character()
    {
        char *p = buf + length() - 1;

        if (p >= buf)
            *p = 0; 
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
    static char get_digit (uint8_t number)
    {
        if (number < 10)
            return number + '0';
        else
            return number + ('A' - 10);
    }

    void copy_from (char* dest, const char* source, int count)
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


inline KString operator+(const char * lhs, const KString & rhs)
{
    return KString(lhs) + rhs; 
}

