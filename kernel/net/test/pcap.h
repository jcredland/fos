#include <stdint.h>
#include <iostream> 
#include <fstream>
#include <string>

class Packet 
{
public:
    int length; 
    std::shared_ptr<char> data; 

    bool isValid() const { return length != 0; }
};

class PCapFormat
{
public:
    PCapFormat(std::string filename)
    {
        file.open(filename, std::ios::binary | std::ios::in); 
        file.read((char *) &file_header, sizeof(file_header)); 

        if (file.fail())
            std::cout << "Read failed." << std::endl; 
    }

    Packet get_next_packet()
    {
        PCapPacketHeader __attribute__((packed)) packet_header;

        file.read((char *) &packet_header, sizeof(packet_header)); 
        if (file.fail())
        {
            Packet empty; 
            empty.length = 0; 
            return empty; 
        }
        else
        {
            Packet p;
            p.data = std::shared_ptr<char>(new char[packet_header.incl_len], [](char * c){ delete[] c; });
            p.length = packet_header.incl_len;

            file.read(p.data.get(), p.length); 
            
            if (file.fail())
                std::cout << "Error reading data." << std::endl;

            int missing_bytes = packet_header.orig_len - p.length; 
            if (missing_bytes != 0)
                std::cout << "Error, shortened packet. " << missing_bytes << " bytes missing." << std::endl;

            return p;
        }
    }

    struct PCapFileHeader
    {
        uint32_t magic_number;   /* magic number */
        uint16_t version_major;  /* major version number */
        uint16_t version_minor;  /* minor version number */
        int32_t  thiszone;       /* GMT to local correction */
        uint32_t sigfigs;        /* accuracy of timestamps */
        uint32_t snaplen;        /* max length of captured packets, in octets */
        uint32_t network;        /* data link type */
    };

    struct PCapPacketHeader
    {
        uint32_t ts_sec;         /* timestamp seconds */
        uint32_t ts_usec;        /* timestamp microseconds */
        uint32_t incl_len;       /* number of octets of packet saved in file */
        uint32_t orig_len;       /* actual length of packet */
    };
        
private:
    std::ifstream file; 
    PCapFileHeader __attribute__((packed)) file_header; 
};

