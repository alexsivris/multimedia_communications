#ifndef FETCHMUSIC_H
#define FETCHMUSIC_H
#include "connection.h"
using namespace std;
class FetchMusic : protected Connection
{
public:
    FetchMusic(int & socket);

private:
    bool requestMusic();
    bool writeMP3(vector<pair<u_int16_t,vector<char> > > &sorted_file);


    fstream m_fs;

};

#endif // FETCHMUSIC_H
