#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int lab2_child(int n){
	FILE* f = fopen("data.txt","r");
	if(f==NULL){
		perror("ERROR: INVALID MEMORY REFERENCE\n");
		abort();
	}
	int rd = 0;
	for(int i=0;i<n;i++){
		rd = fgetc(f);
	}
	fclose(f);
	return rd;

}

int lab2_parent(){
	int status;
	pid_t child_pid;
	child_pid = waitpid(-1,&status,0);
	if(WIFSIGNALED(status)){
		printf("PARENT: child process terminated abnormally!\n");
	}
	else if(WIFEXITED(status)){
		int exit_status = WEXITSTATUS(status);
		printf("PARENT: child process returned %c\n",exit_status);
	}
	return 0;	
}
