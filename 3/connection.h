#ifndef CONNECTION_H
#define CONNECTION_H
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <sys/time.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <netdb.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <stdio.h>
#include <cstdio>

#include <vector>
#include <algorithm>
#include <utility>
#include <map>

class Connection
{
public:
    Connection(int &socket);

protected:

    int &m_socket;
};

#endif // CONNECTION_H
