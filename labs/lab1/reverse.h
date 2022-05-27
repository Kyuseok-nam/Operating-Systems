/* reverse.h */
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>


char * reverse(char * s){
	int i,len = strlen(s);	
	char *buffer = calloc(len,sizeof(char));
	for(i=0;i<len;i++){
		*(buffer+i) = *(s+len-i-1);
	}
	*(buffer+i)='\0';
	for(i=0;i<=len;i++){
		*(s+i)=*(buffer+i);
	}
	free(buffer);
	return s;
}
