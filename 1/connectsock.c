/* connectsock.c - connectsock */

#include "connectsock.h"

int
connectsock(const char *host, const char *service, const char *transport )
/*
 * Arguments:
 *      host      - name of host to which connection is desired
 *      service   - service associated with the desired port
 *      transport - name of transport protocol to use ("tcp" or "udp")
 */
{
    struct hostent	*phe;	/* pointer to host information entry	*/
    struct servent	*pse;	/* pointer to service information entry	*/
    struct protoent *ppe;	/* pointer to protocol information entry*/
    struct sockaddr_in sin;	/* an Internet endpoint address		*/
    int	s, type;	/* socket descriptor and socket type	*/

    int retcode = -1;


    memset(&sin, 0, sizeof(sin));
    sin.sin_family = AF_INET;

    /* Map service name to port number */
    if ( pse = getservbyname(service, transport) )
        sin.sin_port = pse->s_port;
    else if ((sin.sin_port=htons((unsigned short)atoi(service))) == 0)
    {
        printf("can't get \"%s\" service entry\n", service);
        exit(-1);
    }


    /* Map host name to IP address, allowing for dotted decimal */
    if ( phe = gethostbyname(host) )
        memcpy(&sin.sin_addr, phe->h_addr, phe->h_length);
    else if ( (sin.sin_addr.s_addr = inet_addr(host)) == INADDR_NONE )
    {
        printf("can't get \"%s\" host entry\n", host);
        exit(-1);
    }

    /* Map transport protocol name to protocol number */
    if ( (ppe = getprotobyname(transport)) == 0)
    {
        printf("can't get \"%s\" protocol entry\n", transport);
        exit(-1);
    }

    /* Use protocol to choose a socket type */
    if (strcmp(transport, "udp") == 0)
        type = SOCK_DGRAM;
    else
        type = SOCK_STREAM;

    /* Allocate a socket */
    s = socket(PF_INET, type, ppe->p_proto);
    if (s < 0)
    {
        printf("can't create socket \n");
        exit(-1);
    }

    /* Connect the socket */
    if (connect(s, (struct sockaddr *)&sin, sizeof(sin)) < 0)
    {
        printf("can't connect to %s.%s\n", host, service);
        exit(-1);
    }

    return s;
}

int testConnection(int s)
{
    const char * buf = "TEST";
    char buf2[100];

    int retcode = send(s,buf,sizeof(buf),0);
    if (retcode>0)
        printf("%s sent!\n",buf);
    else
        printf("An error occured.\n");

    retcode = recv(s, buf2, 8,0);
    printf("%s \n", buf2);
}
int findMaxPacket(int s)
{
    char packet[15000];
    char response[15000];
    int ret = 0;

    // send 15kB packet
    ret = send(s, packet, 15000, 0);
    if (ret > 0)
    {
        ret = recv(s, response, 15000,0);
        if(ret >0)
            return ret;
        else
            return 0;
    }
}

int back2backPackets(int s,_t_packet_size size)
{
    int time = 0;
    int stopwatch=0;
    int counter = 0;
    int retcode = 0, retcode2 = 0;
    int lines = 0;
    int length;
    char packet[9300] = {0};    // From testing the maximum packet size that can be returned is 9300B
    char response[9300] = {0};
    char filename[200];
    char * buffer;
    char ch;
    char *input = malloc(5*sizeof(char));
    FILE *fp;
    struct timeval tv;
    struct timeval tv2;
    struct stat st = {0};

    buffer = packet;

    // create directory for measurements
    if (stat("./measurements", &st) == -1) {
        mkdir("./measurements", 0700);
    }

    switch (size)
    {
    case jumbo:
        length = 9300;
        strcpy(filename, "./measurements/jumbo_packet.txt");
        if( access( filename , F_OK ) != -1 ) {
            remove(filename);
        }
        fp = fopen(filename ,"a+"); break;
    case large:
        length = 1500;
        strcpy(filename , "./measurements/large_packet.txt");
        if( access( filename, F_OK ) != -1 ) {
            remove(filename);
        }
        fp = fopen(filename,"a+"); break;
    case medium:
        length = 770;
        strcpy(filename, "./measurements/medium_packet.txt");
        if( access( filename, F_OK ) != -1 ) {
            remove(filename);
        }
        fp = fopen(filename,"a+"); break;
    case small:
        length = 40;
        strcpy(filename , "./measurements/small_packet.txt");
        if( access( filename, F_OK ) != -1 ) {
            remove(filename);
        }
        fp = fopen(filename,"a+"); break;
    default:
        length = 1500;
        strcpy(filename, "./measurements/large_packet.txt");
        if( access( filename, F_OK ) != -1 ) {
            remove(filename);
        }
        fp = fopen(filename,"a+"); break;
    }


    // count lines in file
    while(!feof(fp))
    {
        ch = fgetc(fp);
        if(ch == '\n')
        {
            lines++;
        }
    }

    /* timeout socket after 1000ms if not responding */
    tv2.tv_sec = 1;
    tv2.tv_usec = 0;
    if (setsockopt(s, SOL_SOCKET, SO_RCVTIMEO, (char *)&tv2,  sizeof tv2))
        return -1;
    while (counter < 200-lines)
    {
        /*send packet 1*/
        retcode = send(s,buffer,length,0);
        /*send packet 2*/
        if (retcode >0)
            retcode = send(s,buffer,length,0);
        if (retcode >0)
        {

            /*receive packet 1*/
            retcode = recv(s, response, length,0);
            // start timer
            time = gettimeofday(&tv, 0);
            stopwatch = tv.tv_usec;

            /*receive packet 2*/
            retcode2 = recv(s, response, length,0);
            if (retcode > 0 && retcode2 > 0)
            {
                time = gettimeofday(&tv, 0);
                stopwatch = (tv.tv_usec-stopwatch);
                if (stopwatch > 0)
                {
                    printf("%d.) %d microseconds / %d B received.\n",counter,stopwatch,retcode);
                    fprintf(fp,"%d\n",stopwatch);
                    stopwatch = 0;
                    fflush(fp);
                    counter++;
                    usleep(50000);
                }
            }
        }
    }
    fclose(fp);

    free(input);
    return 1;
}
