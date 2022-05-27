/* tcp-client.c */

#include <sys/types.h>
#include <string.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>
#include <strings.h>
#include <unistd.h>

#define MAXBUFFER 1024

int main(int argc, char*argv[])
{
    /*
    if(argc<5){
        fprintf( stderr, "Invalid number of arguments" );
        return EXIT_FAILURE;
    }
    */
    char * hostname =argv[1] ;
    int port_number = atoi(argv[2]);
    int num_int = atoi(argv[3]);
    int numbers[num_int+1];
    numbers[0]=htonl(num_int);
    for(int i=0;i<num_int;i++){
        numbers[i+1] = htonl(atoi(argv[i+4]));
    }
    
    /* create TCP client socket (endpoint) */
    int sd = socket( AF_INET, SOCK_STREAM, 0 );
    if ( sd == -1 ) { perror( "socket() failed" ); exit( EXIT_FAILURE ); }

    /* see the man page for gethostbyname() and use it to set up
     *  the TCP server to which your client will connect
     *
     * for local testing, you should be able to use "localhost"
     */
#if 0
    struct hostent * hp = gethostbyname( "127.0.0.1" );
    struct hostent * hp = gethostbyname( "128.113.126.39" );
    struct hostent * hp = gethostbyname( "linux02.cs.rpi.edu" );
#endif


  struct hostent * hp = gethostbyname(hostname);  /* 127.0.0.1 */

    if ( hp == NULL )
    {
        fprintf( stderr, "ERROR: gethostbyname() failed\n" );
        return EXIT_FAILURE;
    }

    struct sockaddr_in tcp_server;
    tcp_server.sin_family = AF_INET;
    memcpy( (void *)&tcp_server.sin_addr, (void *)hp->h_addr, hp->h_length );
    unsigned short server_port = port_number;
    tcp_server.sin_port = htons( server_port );

   
    if ( connect( sd, (struct sockaddr *)&tcp_server, sizeof( tcp_server ) ) == -1 )
    {
        perror( "connect() failed" );
        return EXIT_FAILURE;
    }
    
    //client connected to server
    printf("CLIENT: Successfully connected to server\n");
    if(num_int==1){
        printf("CLIENT: Sending 1 integer value\n");
    }
    else{
        printf("CLIENT: Sending %d integer values\n",num_int);
    }
    


    /* The implementation of the application protocol is below... */
    
    //sending array of integers to server
    //index 0 has number of ints, the rest has int values to be sent
    int n;
    n = write( sd,numbers,sizeof(numbers));
    
    
    
    //receiving message from the server
    int rnum=0;
    n = read( sd, &rnum, MAXBUFFER );
    rnum = ntohs(rnum);
    printf("CLIENT: Rcvd result: %d\n",rnum);
    
    int msg_num = 1;
    do{
        char buffer[MAXBUFFER+1];
        n = read( sd, buffer, rnum); /* BLOCKING */
        
        if ( n == -1 )
        {
            perror( "read() failed" );
            return EXIT_FAILURE;
        }
        else if ( n == 0 )
        {
        }
        else  /* n > 0 */
        {
            buffer[n] = '\0';    /* assume we rcvd text data */
            printf( "CLIENT: Rcvd secret message #%d: \"%s\"\n",msg_num, buffer );
        }
        msg_num++;
    }
    while(n>0);
    printf( "CLIENT: Disconnected from server\n" );
    
    close( sd );

    return EXIT_SUCCESS;
}
