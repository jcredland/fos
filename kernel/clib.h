
/** Functions that we'd normally find in the standard C library. */

inline void memset(char* dest, int ch, size_t count)
{
    while (count--)
        *dest++ = ch;
}

/** Compare two strings.  If the strings are equal return 0. */
inline int strncmp(const char * a, const char * b, size_t count) 
{
    if (count == 0) return 0; 

    while (count--)
    {
        if (a != b) 
            break;

        if (*a == '\0') 
            return 0; 
        a++;
        b++;
    }

    return (*(unsigned char *) a) - (*(unsigned char *) b);
}
