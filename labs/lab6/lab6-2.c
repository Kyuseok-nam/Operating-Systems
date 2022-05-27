/* udp-server.c */

/* UDP SERVER example
   -- socket()             create a socket (endpoint) for communication
   -- bind()               bind to (assign) a specific port number
                           (or let the OS assign us a port number)
   -- getsockname()        get socket "name" -- IP address, port number, etc.
   -- recvfrom()/sendto()  receive/send datagrams
*/

/* To test this server, you can use the following
   command-line netcat tool:


   bash$ netcat -u linux02.cs.rpi.edu 41234
                   ^^^^^^
                   replace with your hostname

   Note that netcat will act as a client to this UDP server...

   The hostname (e.g., linux02.cs.rpi.edu) can also be
   localhost (127.0.0.1); and the port number must match what
   the server reports.
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>

#define MAXBUFFER 1024





int main(int argc, char*argv[])
{
    int memory = atoi(argv[1]);
    int page = atoi(argv[2]);
    
    if(argc!=3){
        fprintf(stderr,"ERROR: Invalid argument(s)\n");
        fprintf(stderr,"USAGE: a.out <n> <p>\n");
        return EXIT_FAILURE;
    }
    
    printf("Simulating %d-bit memory addressing with a %d-bit page number...\n",memory,page);
    while(1){
        char input[256];
        char word[256];
        int length = 0;
        char hex[256];
        strcpy(word,"");
        printf("Enter logical address ==> ");
        scanf("%s",input);
        
        length = strlen(input);
        memset(hex, '\0', sizeof(hex));
        strncpy(hex,input+2,length);
        
        
        if(strcmp(input,"exit")==0 || strcmp(input,"quit")==0){break;}
        int count = 0;
        while(hex[count]){
                switch(hex[count]){
                    case '0' : strcat(word,"0000");
                        break;
                    case '1' : strcat(word,"0001");
                        break;
                    case '2' : strcat(word,"0010");
                        break;
                    case '3' : strcat(word,"0011");
                        break;
                    case '4' : strcat(word,"0100");
                        break;
                    case '5' : strcat(word,"0101");
                        break;
                    case '6' : strcat(word,"0110");
                        break;
                    case '7' : strcat(word,"0111");
                        break;
                    case '8' : strcat(word,"1000");
                        break;
                    case '9' : strcat(word,"1001");
                        break;
                    case 'A' : strcat(word,"1010");
                        break;
                    case 'B' : strcat(word,"1011");
                        break;
                    case 'C' : strcat(word,"1100");
                        break;
                    case 'D' : strcat(word,"1101");
                        break;
                    case 'E' : strcat(word,"1110");
                        break;
                    case 'F' : strcat(word,"1111");
                        break;
                    case 'a' : strcat(word,"1010");
                        break;
                    case 'b' : strcat(word,"1011");
                        break;
                    case 'c' : strcat(word,"1100");
                        break;
                    case 'd' : strcat(word,"1101");
                        break;
                    case 'e' : strcat(word,"1110");
                        break;
                    case 'f' : strcat(word,"1111");
                        break;
                    default : printf("\nInvalid argument, Please Try Again  %c\n",hex[count]); continue;
                }
                count++;
        }
        
        char first[256];
        char second[256];
        strcpy(first,"");
        strcpy(second,"");
        strncpy(first,word,page);
        strncpy(second,word+page,strlen(word));
        long first_dec = strtol(first,NULL,2);
        long second_dec = strtol(second,NULL,2);
        
        long remainder;
        int i, j = 0;
        char hexfirst[100];
        while (first_dec != 0)
            {
                remainder = first_dec % 16;
                if (remainder < 10)
                    hexfirst[j++] = 48 + remainder;
                else
                    hexfirst[j++] = 55 + remainder;
                first_dec = first_dec / 16;
            }
        
        char first_answer[256];
        strcpy(first_answer,"");
        for (i = j; i >= 0; i--)
            strncat(first_answer,&hexfirst[i],1);
        
        
        i= 0;
        j=0;
        char hexsecond[100];
        while (second_dec != 0)
            {
                remainder = second_dec % 16;
                if (remainder < 10)
                    hexsecond[j++] = 48 + remainder;
                else
                    hexsecond[j++] = 55 + remainder;
                second_dec = second_dec / 16;
            }
        
        char second_answer[256];
        strcpy(second_answer,"");
        for (i = j; i >= 0; i--)
                    strncat(second_answer,&hexsecond[i],1);
        printf("Logical address %s maps to page 0x%s and offset 0x%s\n",input,first_answer,second_answer);
        
        
        
        
        
    }
    
    
    return EXIT_SUCCESS;
}
