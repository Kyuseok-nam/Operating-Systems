/* forked.c */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

/* implement these functions in fork-functions.c */
int lab2_child( int n );
int lab2_parent();

int main()
{
  int n = 7;  /* or some other value ... */
  int rc;

  /* create a new (child) process */
  pid_t p = fork();

  if ( p == -1 )
  {
    perror( "fork() failed" );
    return EXIT_FAILURE;
  }

  if ( p == 0 )
  {
    rc = lab2_child( n );
  }
  else /* p > 0 */
  {
    rc = lab2_parent();
  }

  return rc;
}