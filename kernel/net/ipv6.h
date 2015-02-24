
struct ipv6_address
{
    uint64_t a;
    uint64_t b; 
};

struct ipv6_header1
{
    unsigned version : 4; 
    unsigned traffic_class : 8; 
    unsigned flow_label : 20; 
    uint8_t header_type; // RFC1700
    uint8_t hop_limit; 
    ipv6_address source_addr;
    ipv6_address dest_addr;
};


/* Recommended order of headers: 
           IPv6 header
           Hop-by-Hop Options header
           Destination Options header (note 1)
           Routing header
           Fragment header
*/

class Header
{
enum 
{
};
public:
    Header * getNextHeader();
};

class HopByHopHeader
:
public Header
{
/*
      Option Type          8-bit identifier of the type of option.

      Opt Data Len         8-bit unsigned integer.  Length of the Option
                           Data field of this option, in octets.

      Option Data          Variable-length field.  Option-Type-specific
                           data.
*/
};
