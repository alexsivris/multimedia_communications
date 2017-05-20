#include <stdio.h>
#include "connectsock.h"

int main(int argc, char **argv)
{
    char * host = argv[1];
    char * port = argv[2];
    const char * protocol = "udp";

    printf("Connecting to server: %s:%s \n",host, port);
    int sock = connectsock(host,port,protocol);
    if(testConnection(sock))
    {
        // find out max packet size
        printf("The maximum packet size is: %d B\n", findMaxPacket(sock));

        // let's run some measurements with different packet sizes
        if (back2backPackets(sock,small))
            printf("small packets (size 40): done! \n");
        if (back2backPackets(sock,medium))
            printf("medium packets (size 770): done! \n");
        if (back2backPackets(sock,large))
            printf("large packets (size 1500): done! \n");
        if (back2backPackets(sock,jumbo))
            printf("large packets (size 9300): done! \n");

        return 1;
    }
    else
    {
        return 0;
    }
}

