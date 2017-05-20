#include "fetchvideo.h"
#include "errno.h"

FetchVideo::FetchVideo(int & socket, bool retransmission): Connection(socket), m_retransmission(retransmission)
{
    if (requestVideo())
        cout << "Video file is loaded!" << endl;
}
bool sortorder2(const pair<unsigned short, vector<char> > &left, const pair<unsigned short, vector<char> > &right)
{
    return left.first < right.first;
}
bool FetchVideo::requestVideo()
{
    char buf[100] = "VIDEO 03627456";
    char buf2[100];
    char recv_buf[1500];

    u_int16_t seq_no=0;
    u_int16_t seq_no_old=0;
    vector<u_int16_t> lost_packets;

    vector<char> payload;
    map<int,vector<char> > map_file;
    vector<pair<u_int16_t ,vector<char> > > vec_file;
    int retcode = send(m_socket,buf,100,0);

    if (retcode > 0)
    {
        retcode = recv(m_socket,recv_buf,sizeof(recv_buf),0);
        if (retcode < 0)
            cout << errno << endl;
        while (strcmp(recv_buf,"END"))
        {
            retcode = recv(m_socket,recv_buf,sizeof(recv_buf),0);
            if (retcode > 0)
            {
                if (!strcmp(recv_buf,"END")) break;
                seq_no_old = seq_no;
                seq_no = (unsigned char)recv_buf[2];
                seq_no = (seq_no<<8) + (unsigned char)recv_buf[3];
                cout << "Sequence number:" << seq_no << endl;
                if (m_retransmission ) // w/ retransmission
                {
                    if ((seq_no - seq_no_old) > 1)
                    {
                        int num_lost = abs(seq_no - seq_no_old);
                        for (int i=1;i<num_lost;i++)
                        {
                            cout << "Lost: " << seq_no_old+i << endl;
                            lost_packets.push_back(seq_no_old+i);
                        }
                    }
                }

                for (int i=12;i<retcode;i++)
                {
                    payload.push_back(recv_buf[i]);
                }
                map_file.insert(pair<int, vector<char> >(seq_no,payload));
                vec_file.push_back(pair<u_int16_t , vector<char> >(seq_no,payload));
                // flush payload
                //cout << "vec seq: "<< (vec_file.end()-1)->first << "B Seq_no: " << seq_no << " ssrc:" << ssrc << endl;
                payload.erase(payload.begin(),payload.end());



            }
            sort(vec_file.begin(),vec_file.end(),sortorder2);
        }

        //request lost packets
        if (m_retransmission)
        {
            for (int i=0;i<lost_packets.size();i++)
            {
                int n = sprintf(buf2,"R %d",lost_packets[i]);
                retcode = send(m_socket,buf2,100,0);
                if (n > 0 && retcode > 0)
                {
                    retcode = recv(m_socket,recv_buf,sizeof(recv_buf),0);
                    if (retcode > 0)
                    {
                        //copy data into vector
                        for (int i=12;i<retcode;i++)
                        {
                            payload.push_back(recv_buf[i]);
                        }
                        seq_no = (unsigned char) recv_buf[2];
                        seq_no = (seq_no<<8) + (unsigned char)recv_buf[3];
                        vec_file.push_back(pair<u_int16_t , vector<char> >(seq_no,payload));
                        // flush payload
                        payload.erase(payload.begin(),payload.end());
                    }
                }
                cout << " [" << lost_packets[i] << "] ";
            }
            cout << "Lost a total of "<< lost_packets.size() << " packets!" << endl;
        }
        // sort again
        sort(vec_file.begin(),vec_file.end(),sortorder2);

        if (writeVideo(vec_file))
        {
            return true;
        }
    }
}

bool FetchVideo::writeVideo(vector<pair<u_int16_t,vector<char> > > &sorted_file)
{
    string filename;
    if(m_retransmission)
        filename = "retransmission.yuv";
    else
        filename = "no_retransmission.yuv";

    ifstream ifile(filename.c_str());
    if (ifile)
        remove(filename.c_str());

    fstream fs;
    fs.open(filename.c_str(),fstream::in | fstream::out | fstream::app);

    vector<pair<u_int16_t,vector<char> > >::iterator it;
    vector<char>::iterator it2;
    int ctr = 0;
    for (it = sorted_file.begin(); it != sorted_file.end(); it++)
    {
        if(ctr)
        {
            int dif = it->first - (it-1)->first;
            if (dif > 1)
                cout << "Detected sequence no. jump! Distance: " << dif  << it->first << ", " << (it-1)->first << endl;
        }

        for (it2 = it->second.begin(); it2 != it->second.end(); it2++)
            fs << *it2;
        ctr++;
    }
    fs.close();
    return true;
}
