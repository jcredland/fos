#include "pcap.h"

int main()
{
    PCapFormat pcap("example.pcap"); 
    int count = 0; 

    Packet p; 
    do 
    {
        p = pcap.get_next_packet();  
        count++; 
    } 
    while (p.isValid()); 

    std::cout << "Packets Processed: " << count << std::endl;
    return 0;
}
