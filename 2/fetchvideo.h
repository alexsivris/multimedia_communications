#ifndef FETCHVIDEO_H
#define FETCHVIDEO_H
#include "connection.h"
using namespace std;
class FetchVideo : protected Connection
{
public:
    FetchVideo(int & socket, bool retransmission);
private:
    bool requestVideo();
    bool writeVideo(vector<pair<u_int16_t,vector<char> > > &sorted_file);

    fstream m_fs;
    bool m_retransmission;
};

#endif // FETCHVIDEO_H
