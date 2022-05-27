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

/* This constant defines the shared memory segment such that
   multiple processes can attach to this segment */
#define SHM_SHARED_KEY 8999

void child(int *data){
    int i, stop = 10000;
    printf( "CHILD: adding 1..%d to shared memory segment\n",stop);
    for ( i = 1 ; i <= stop ; i++ )
    {
        *data += i;
    }
}
