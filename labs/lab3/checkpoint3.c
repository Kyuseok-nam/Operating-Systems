/* count-shm.c */

/* create a shared memory segment, then create a child process
    such that both parent and child are attached to the shared memory */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>

/* This constant defines the shared memory segment such that
   multiple processes can attach to this segment */
#define SHM_SHARED_KEY 4000

void child(int* data);

int main(int argc, char* argv[])
{
    int num = atoi(argv[1]);
    char c = *(argv[2]);
    printf("C: %c\n",c);
    struct stat sb;
    int size = lstat("data.txt",&sb);
    size = sb.st_size;
    int per_child;
    per_child= size/num;
    printf("size: %d\n",size);
    
    printf("PARENT: reading \"data.txt\" into shared memory segment\n");
    
    
    key_t key = SHM_SHARED_KEY;
    int shmid = shmget( key, size, IPC_CREAT | IPC_EXCL | 0660 );
                          /* ^^^^^^^^^^^^^
                              request a shared memory segment of this size */

    printf( "shmget() returned %d\n", shmid );

    if ( shmid == -1 )
    {
      perror( "shmget() failed" );
      return EXIT_FAILURE;
    }


    /* attach to the shared memory segment */
    char *data = shmat( shmid, NULL, 0 );

    if ( data == (void *)-1 )
    {
      perror( "shmat() failed" );
      return EXIT_FAILURE;
    }


    FILE* f = fopen("data.txt","r");
    int rd = 0;
    for(int i=0;i<size;i++){
        //stores character in c
        rd = fscanf(f,"%c",&c);
        data[i]=c;
    }
    data[size]='\0';
    printf("buffer: %s\n",data);
    
    

    printf("PARENT: creating %d child processes",num);
    /*
    int p;
    for(int i=0;i<num;i++){
        p=fork();
        if ( p == -1 )
        {
            perror( "fork() failed" );
            return EXIT_FAILURE;
        }
        if(p==0){
            int count=0;
            for(int j = i*per_child; j< j+per_child;j++){
                if(buff[j]==c){
                    count+=1;
                }
            }
            printf("CHILD: Counted %d occurrences of '%c' in byte range %d-%d \n",count,c,i*per_child,i*per_child+per_child-1);
            exit(0);
        }
    }
    
    for(int i=0;i<num;i++){
        if ( p > 0 )
        {
            waitpid( p, NULL, 0 );
        }
    }
    
   */
    
    //detach from shared memory
    int rc = shmdt( data );

    if ( rc == -1 )
    {
        perror( "shmdt() failed" );
        exit( EXIT_FAILURE );
    }
    if ( shmctl( shmid, IPC_RMID, 0 ) == -1 )
    {
        perror( "shmctl() failed" );
        exit( EXIT_FAILURE );
    }
    
    return EXIT_SUCCESS;
}
