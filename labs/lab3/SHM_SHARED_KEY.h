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

int main()
{
  /* create the shared memory segment with a size of 4 bytes */
  key_t key = SHM_SHARED_KEY;
  int shmid = shmget( key, sizeof( int ), IPC_CREAT | IPC_EXCL | 0660 );
                        /* ^^^^^^^^^^^^^
                            request a shared memory segment of this size */

  printf( "shmget() returned %d\n", shmid );

  if ( shmid == -1 )
  {
    perror( "shmget() failed" );
    return EXIT_FAILURE;
  }


  /* attach to the shared memory segment */
  int * data = shmat( shmid, NULL, 0 );

  if ( data == (void *)-1 )
  {
    perror( "shmat() failed" );
    return EXIT_FAILURE;
  }


  /* the memory created by shmget() is zeroed out */
  *data = 0;

  /* create a child process */
  int p = fork();

  if ( p > 0 ) usleep( 100 );

  if ( p == -1 )
  {
    perror( "fork() failed" );
    return EXIT_FAILURE;
  }


  /* both parent and child processes will run the code below, */
  /*  both accessing (and changing the shared memory segment) */
  int i, stop = 10000;

  for ( i = 1 ; i <= stop ; i++ )
  {
#if 0
    printf( "%s: adding %d\n", ( p > 0 ? "PARENT" : "CHILD" ), i );
#endif

    *data += i;

#if 0
    int tmp = *data;
    tmp += i;
    *data = tmp;
#endif

    usleep( 10 );
  }

  printf( "%s: Sum 1..%d is %d\n", ( p > 0 ? "PARENT" : "CHILD" ), stop, *data );


  /* detach from the shared memory segment */
  int rc = shmdt( data );

  if ( rc == -1 )
  {
    perror( "shmdt() failed" );
    exit( EXIT_FAILURE );
  }

  if ( p > 0 )  /* parent process only */
  {
    waitpid( p, NULL, 0 );

    /* remove the shared memory segment */
    if ( shmctl( shmid, IPC_RMID, 0 ) == -1 )
    {
      perror( "shmctl() failed" );
      exit( EXIT_FAILURE );
    }
  }

  return EXIT_SUCCESS;
}