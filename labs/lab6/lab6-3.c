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
    int offset = memory-page;
    
    if(argc!=3){
        fprintf(stderr,"ERROR: Invalid argument(s)\n");
        fprintf(stderr,"USAGE: a.out <n> <p>\n");
        return EXIT_FAILURE;
    }
    
    printf("Simulating %d-bit memory addressing with a %d-bit page number...\n",memory,page);
    
    while(1){
        char input[256];
    
        
        //recieves input
        printf("Enter logical address ==> ");
        scanf("%[^\n]%*c", input);
        
        //exit
        if(strcmp(input,"exit")==0 || strcmp(input,"quit")==0){break;}
        
        //seperates space for input to get page and bit
        int space = 0;
        for(;space<strlen(input);space++){
            if(input[space]==' '){
                break;
            }
        }
        char first[256];
        char second[256];
        memset(first, '\0', sizeof(first));
        memset(second, '\0', sizeof(second));
        strncpy(first,input,space);
        strncpy(second,input+space+1,strlen(input));
        
        //deliminate 0x
        char first1[256];
        char second1[256];
        memset(first1, '\0', sizeof(first1));
        memset(second1, '\0', sizeof(second1));
        strncpy(first1,first+2,strlen(first));
        strncpy(second1,second+2,strlen(second));
        printf("first: %s, second: %s \n",first1,second1);

        char word[256];
        strcpy(word,"");
        int count = 0;
        while(first1[count]){
                switch(first1[count]){
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
                    default : printf("\nInvalid argument, Please Try Again  %c\n",first1[count]); continue;
                }
                count++;
        }
        
        
        
        
        char word2[256];
        strcpy(word2,"");
        count = 0;
        while(second1[count]){
                switch(second1[count]){
                    case '0' : strcat(word2,"0000");
                        break;
                    case '1' : strcat(word2,"0001");
                        break;
                    case '2' : strcat(word2,"0010");
                        break;
                    case '3' : strcat(word2,"0011");
                        break;
                    case '4' : strcat(word2,"0100");
                        break;
                    case '5' : strcat(word2,"0101");
                        break;
                    case '6' : strcat(word2,"0110");
                        break;
                    case '7' : strcat(word2,"0111");
                        break;
                    case '8' : strcat(word2,"1000");
                        break;
                    case '9' : strcat(word2,"1001");
                        break;
                    case 'A' : strcat(word2,"1010");
                        break;
                    case 'B' : strcat(word2,"1011");
                        break;
                    case 'C' : strcat(word2,"1100");
                        break;
                    case 'D' : strcat(word2,"1101");
                        break;
                    case 'E' : strcat(word2,"1110");
                        break;
                    case 'F' : strcat(word2,"1111");
                        break;
                    case 'a' : strcat(word2,"1010");
                        break;
                    case 'b' : strcat(word2,"1011");
                        break;
                    case 'c' : strcat(word2,"1100");
                        break;
                    case 'd' : strcat(word2,"1101");
                        break;
                    case 'e' : strcat(word2,"1110");
                        break;
                    case 'f' : strcat(word2,"1111");
                        break;
                    default : printf("\nInvalid argument, Please Try Again  %c\n",second1[count]); continue;
                }
                count++;
        }
        
        char w1[256];
        strncpy(w1,word+(strlen(word)-page),strlen(word));
        char w2[256];
        strncpy(w2,word2+(strlen(word2)-offset),strlen(word2));
       
        
        //add the page and byte offset
        char binary[256];
        strcpy(binary,w1);
        strcat(binary,w2);
        
        //convert binary to decimal
        long dec = strtol(binary,NULL,2);
        
        
        long remainder;
        int i, j = 0;
        char hex[100];
        while (dec != 0)
            {
                remainder = dec % 16;
                if (remainder < 10)
                    hex[j++] = 48 + remainder;
                else
                    hex[j++] = 55 + remainder;
                dec = dec / 16;
            }
        
        char answer[256];
        strcpy(answer,"");
        for (i = j; i >= 0; i--)
            strncat(answer,&hex[i],1);
        

        printf("Logical page 0x%s and offset 0x%s maps to address 0x%s\n",first1,second1,answer);
        
        
        
    }
    
    
    return EXIT_SUCCESS;
}
