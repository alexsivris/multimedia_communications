#include <iostream>
#include <string>
#include "connectsock.h"
#include "dashdaemon.h"
using namespace std;

int main(int argc, char **argv)
{
    // connect to server
    char * host = argv[1];
    char * port = argv[2];
    bool b;
    if(!strcmp(argv[3],"1" ))
    {
        cout << "Rate adaption activated (task 2)." << endl;
        b = true;
    }
    else
    {
        cout << "Fetching segments with highest representation (task 1)." << endl;
        b = false;
    }

    const char * protocol = "tcp";

    int sock = connectsock(host,port,protocol);

    DashDaemon dd(sock, b);
    return 0;
}
