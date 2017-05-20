#include <iostream>
#include <string>
#include "connectsock.h"
#include "textfile.h"
#include "fetchmusic.h"
#include "fetchvideo.h"
using namespace std;

int main(int argc, char **argv)
{
    // connect to server
    char * host = argv[1];
    char * port = argv[2];
    const char * protocol = "udp";

    int sock = connectsock(host,port,protocol);
    cout << "1.) Fetching text file..." << endl;
    TextFile tf(sock);
    cout << "2.) Fetching music file..." << endl;
    FetchMusic fm(sock);

    cout << "3.) Fetching video file (without retransmission)..." << endl;
    FetchVideo fv_no_rt(sock,0);
    cout << "4.) Fetching video file (with retransmission)..." << endl;
    FetchVideo fv_rt(sock,1);

    return 0;
}
