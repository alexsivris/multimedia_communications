#include "fetchmusic.h"

FetchMusic::FetchMusic(int & socket) : Connection(socket)
{
    if (requestMusic())
    {
        cout << "Music file is loaded!" << endl;
    }
}
bool sortorder(const pair<unsigned short, vector<char> > &left, const pair<unsigned short, vector<char> > &right)
{
    return left.first < right.first;
}

bool FetchMusic::requestMusic()
{
    char buf[100] = "MUSIC XXXX";
    char recv_buf[1500];
    string test;
    u_int16_t seq_no=0;

    unsigned int timestamp=0;
    unsigned int ssrc=0;

    vector<char> payload;
    vector<pair<u_int16_t,vector<char> > > vec_file;
    int retcode = send(m_socket,buf,100,0);

    if (retcode > 0)
    {
        retcode = recv(m_socket,recv_buf,sizeof(recv_buf),0);
        while (strcmp(recv_buf,"END"))
        {
            retcode = recv(m_socket,recv_buf,sizeof(recv_buf),0);
            if (retcode >0)
            {
                seq_no = (unsigned char)recv_buf[2];
                seq_no = (seq_no << 8) + (unsigned char)recv_buf[3];
                timestamp = (recv_buf[4] << 32) + (recv_buf[5] << 16) + (recv_buf[6] << 8) + recv_buf[7] ;
                ssrc = (recv_buf[8] << 32) + (recv_buf[9] << 16) + (recv_buf[10] << 8) + recv_buf[11] ;

                if (strcmp(recv_buf,"END"))
                {
                    test = recv_buf;
                    for (int i=12;i<retcode;i++)
                    {
                        payload.push_back(recv_buf[i]);
                    }
                    vec_file.push_back(pair<u_int16_t, vector<char> >(seq_no,payload));
                    // flush payload
                    cout << "vec seq: "<< (vec_file.end()-1)->first << "B Seq_no: " << seq_no << " ssrc:" << ssrc << endl;
                    payload.erase(payload.begin(),payload.end());
                }
            }
            sort(vec_file.begin(),vec_file.end(),sortorder);
        }
        cout << "Last element" << test << endl;
        if (writeMP3(vec_file))
        {
            return true;
        }
    }
}
bool FetchMusic::writeMP3(vector<pair<u_int16_t,vector<char> > > &sorted_file)
{
    ifstream ifile("MusicFile.mp3");
    if (ifile)
        remove("MusicFile.mp3");
    fstream fs;
    fs.open("MusicFile.mp3",fstream::in | fstream::out | fstream::app);
    vector<pair<u_int16_t,vector<char> > >::iterator it;
    vector<char>::iterator it2;
    int ctr = 0;
    for (it = sorted_file.begin(); it != sorted_file.end(); it++)
    {
        if(ctr)
        {
            int dif = it->first - (it-1)->first;
            if (dif > 1)
                cout << "Detected sequence no. jump! Distance: " << dif << endl;
        }

        for (it2 = it->second.begin(); it2 != it->second.end(); it2++)
            fs << *it2;
        ctr++;
    }
    fs.close();
    return true;
}
