#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main(){
	printf("PARENT: start here!\n");
	pid_t p = fork();
	if(p==-1){
		perror("for() failed");
		return EXIT_FAILURE;
	}
	if(p==0){
		printf("CHILD: happy birthday to me!\n");
		printf("CHILD: idk......good-bye!\n");
	}
	else{
		int status;
		pid_t child_pid;
		child_pid = waitpid(p,&status,0);
		printf("PARENT: child process terminated.\n");
		printf("PARENT: sigh, i'm gonna miss that little child process.\n");
	}
	return EXIT_SUCCESS;

}
