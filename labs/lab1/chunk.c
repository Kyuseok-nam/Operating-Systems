/* chunck.c */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>


int main(int argc, char** argv){
	int n = atoi(*(argv+1));
	char *name = *(argv+2);
	int fd = open(name,O_RDONLY);
	
	if(fd == -1){
		perror("open() failed");
		return EXIT_FAILURE;
	}

	char* buffer = calloc(n+1,sizeof(char));
	
	int rc = read(fd,buffer,n);
	*(buffer+rc)='\0';
	printf("%s",buffer);
	rc = lseek(fd,n,SEEK_CUR);
	
	while(1){
		rc = read(fd,buffer,n);
		*(buffer+rc)='\0';
		if(rc==0){break;}
		printf("-%s",buffer);
		rc = lseek(fd,n,SEEK_CUR);
	}
	printf("\n");
	close(fd);
	return EXIT_SUCCESS;
}

