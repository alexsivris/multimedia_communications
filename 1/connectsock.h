#ifndef CONNECTSOCK_H
#define CONNECTSOCK_H
#include <sys/types.h>
#include <sys/socket.h>

#include <netinet/in.h>
#include <arpa/inet.h>

#include <netdb.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <stdio.h>

#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#ifndef	INADDR_NONE
#define	INADDR_NONE	0xffffffff
#endif	/* INADDR_NONE */
typedef enum {small, medium, large, jumbo} _t_packet_size;
/*------------------------------------------------------------------------
 * connectsock - allocate & connect a socket using TCP or UDP
 *------------------------------------------------------------------------
 */
int connectsock(const char *host, const char *service, const char *transport );
/*------------------------------------------------------------------------
 * testConnection - test connection by sending a "TEST" message to the server
 *------------------------------------------------------------------------
 */
int testConnection(int s);
/*------------------------------------------------------------------------
 * back2backPackets - used for bottleneck link rate estimation for various packet sizes
 *------------------------------------------------------------------------
 */
int back2backPackets(int s, _t_packet_size size);
/*------------------------------------------------------------------------
 * findMaxPacket - find out maximum packet size
 *------------------------------------------------------------------------
 */
int findMaxPacket(int s);
#endif // CONNECTSOCK_H
