#pragma once
/** Functions that we'd normally find in the standard C library. */

template <typename T> 
const T & min(const T & a, const T & b)
{
    return a < b ? a : b; 
}

template <typename T> 
const T & max(const T & a, const T & b)
{
    return a > b ? a : b; 
}

inline void memset(char* dest, int ch, size_t count)
{
    while (count--)
        *dest++ = ch;
}

/** Neither fast, not checked for safety. */
inline void memcpy(char * dest, const char * source, size_t count)
{
    while (count--)
        *dest++ = *source++;
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

inline int strcmp(const char * a, const char * b)
{
    while (*a == *b)
    {
        if (*a == 0)
            return 0; 

        ++a; 
        ++b; 
    }

    return (*(unsigned char *) a) - (*(unsigned char *) b);
}
