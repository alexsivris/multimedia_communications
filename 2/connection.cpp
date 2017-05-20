#include "connection.h"

Connection::Connection(int &socket) : m_socket(socket)
{
    struct timeval tv2;
    tv2.tv_sec = 1;
    tv2.tv_usec = 0;
    setsockopt(m_socket, SOL_SOCKET, SO_RCVTIMEO, (char *)&tv2,  sizeof tv2);
}

