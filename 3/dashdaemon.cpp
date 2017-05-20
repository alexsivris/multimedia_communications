#include "dashdaemon.h"

DashDaemon::DashDaemon(int &sock, bool b) : Connection(sock),mcf_target_buffer(5.0)
{
    if (!b)
    {
        // task 1 - init buffer 6 seconds
        startTask("CONNECT 6 http://kufstein2.lmt.ei.tum.de/video/",0);
        cout << "task 1 done..."<< endl;
    }
    else
    {
        // task 2
        startTask("CONNECT 6 http://kufstein2.lmt.ei.tum.de/video/",1);
        cout << "task 2 done..."<< endl;
    }

}

void DashDaemon::startTask(const char * msg, bool _rate_adaption)
{

    char buf[1500];
    int ret = send(m_socket,msg,strlen(msg),0);
    cout << msg << endl;
    if (ret > 0)
    {
        ret = recv(m_socket,buf,1500,0);
        // MES
        cout << buf << endl;
        ret = recv(m_socket,buf,1500,0);
        while (ret > 0)
        {
            ret = recv(m_socket,buf,1500,0);
            cout << buf << endl;
            if (strstr(buf,"MPD"))
            {
                // save rates into vector
                char *tok = strtok(buf," ");
                vector<float> vec_mes(0);
                vector<float>::iterator vm_it;
                int cnt = 0;
                while (tok != NULL)
                {
                    if (cnt)
                    {
                        int i_rep;
                        stringstream(tok) >> i_rep;
                        mv_rep.push_back(i_rep);
                        tok = strtok (NULL, " ");
                    }
                    cnt++;
                }
                cout << "size of vector: " << mv_rep.size() << endl;
                cout << "representations: " << mv_rep.at(2) << " " << mv_rep.at(3) << " "  << mv_rep.at(4) << " "  << mv_rep.at(5) << " "  << mv_rep.at(6) << " "  << mv_rep.at(7) << endl;
                // fetch content
                if (fetchContent(_rate_adaption))
                {
                    cout << "Video downloaded!" << endl;
                }
                break;
            }
        }
    }


}

bool DashDaemon::fetchContent(bool _rate_adaption)
{
    char *msg = "GET 6 1";
    char buf[1500];
    int next_segment = 0;
    int representation = 6;
    struct timeval tv1,tv2;
    double elapsedTime =0;
    double usec=0;
    double mu, epsilon=0;
    int dwn_num_last =0;

    cout << "sending get" << endl;
    int ret = send(m_socket,msg,strlen(msg),0);
    if (ret > 0)
    {
        ret = recv(m_socket,buf,1500,0);
        // start timer
        gettimeofday(&tv1, NULL);


        while (ret > 0)
        {
            ret = recv(m_socket,buf,1500,0);

            // get MES
            if (strstr(buf,"MES") != NULL)
            {
                char *tok = strtok(buf," ");
                vector<float> vec_mes(0);
                vector<float>::iterator vm_it;
                int cnt = 0;

                while (tok != NULL)
                {
                    if (cnt)
                    {
                        float f_ms;
                        stringstream(tok) >> f_ms;
                        vec_mes.push_back(f_ms);
                        tok = strtok (NULL, " ");
                    }
                    cnt++;
                }
                mf_throughput = vec_mes.at(1);
                mf_buffer = vec_mes.at(2);
                mi_segment = (int)vec_mes.at(3);

            }
            if (strstr(buf,"DWN") != NULL)
            {
                // stop timer
                gettimeofday(&tv2, NULL);

                //received DWN message
                char *tok = strtok(buf," ");
                string expl[2];
                int cnt = 0;
                while (tok != NULL)
                {
                    expl[cnt++] = tok;
                    tok = strtok (NULL, " ");
                }
                // convert string to int
                int dwn_num;
                stringstream(expl[1]) >> dwn_num;
                // check if I'm at the last segment
                if (dwn_num == 150)
                    return true;


                long t = (tv2.tv_sec*1e6 + tv2.tv_usec) - (tv1.tv_sec*1e6 + tv1.tv_usec);
                t = t/1000;
                cout << "Downloaded segment "<< dwn_num << " in " << t << "ms " << "representation: " << representation << endl;

                cout << "End of rate adaption" << endl;
                next_segment = dwn_num + 1;

                // Rate-adaption based on paper "Rate adaptation for adaptive HTTP streaming" by C. Liu et al.
                if (_rate_adaption)
                {
                    // switch variable
                    if (t)
                        mu = 2000/(double(t));

                    cout << "mu: " << mu << endl;

                    // switch-up factor

                    for (int i=3;i<mv_rep.size();i++)
                    {
                        int rate_i_plus_1 = mv_rep.at(i);
                        int rate_i = mv_rep.at(i-1);
                        double quotient = (double(rate_i_plus_1) - double(rate_i)) / double(rate_i);
                        if (quotient > epsilon)
                            epsilon = quotient;
                    }
                    cout << "epsilon: " << epsilon << endl;
                    // switch-up
                    if (mu > (1+epsilon))
                    {
                        representation++;
                    }
                    if (mu < 0.8)
                    {
                        int current_br = mv_rep.at(representation+1);
                        double seek_br = mu * current_br;
                        int counter = 1;
                        for (int i=2;i<mv_rep.size();i++)
                        {
                            if (mv_rep.at(i) <= seek_br)
                            {
                                representation = counter;
                                break;
                            }
                            counter++;
                        }
                    }
                }
                // check overflow
                if (representation>6) representation = 6;
                // end of rate adaption

                char msg2[1500];
                int n = sprintf(msg2, "GET %d %d", representation,next_segment);
                ret = send(m_socket,msg2,strlen(msg2),0);

                // start timer
                gettimeofday(&tv1, NULL);

                dwn_num_last = dwn_num;

            }
        }
    }
    return true;

}

void DashDaemon::adaptRate(int &_rep)
{
    float f_U = mf_buffer - mcf_target_buffer;
    float T_est = mf_throughput;
    float v_tilde = f_U * T_est;
    float v = v_tilde;
    vector<unsigned int>::iterator it;
    int counter=0;

    for (it=mv_rep.begin(); it!= mv_rep.end(); it++)
    {
        counter++;
        if (v_tilde <= *it)
        {
            v = *it;
            _rep = counter;
            break;
        }


    }


}
