#ifndef DASHDAEMON_H
#define DASHDAEMON_H
#include "connection.h"
using namespace std;
class DashDaemon : protected Connection
{
public:
    DashDaemon(int &sock, bool b);
private:
    void startTask(const char * msg, bool _rate_adaption);
    bool fetchContent(bool _rate_adaption);
    void adaptRate(int & _rep);

    const float mcf_target_buffer;
    float mf_throughput;
    float mf_buffer;
    float mi_segment;

    vector<unsigned int> mv_rep;
};

#endif // DASHDAEMON_H
