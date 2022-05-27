/* hw1.c */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

int main(int argc, char** argv){
	if(argc<3){
		fprintf(stderr,"ERROR: <need more arguments>\n");
	}
	for(int i=0;i<strlen(*(argv+1));i++){
		if(*(*(argv+1)+i)<48||*(*(argv+1)+i)>57){
			fprintf(stderr,"ERROR: FLOATING POINT ERROR\n");
			return EXIT_FAILURE;
		}
	}
	int size = atoi(*(argv+1));
	//hashtable 
	int ** ht = calloc(size,sizeof(int*));
	//dynamic array for tracking how many elements are in the index
	int * track = calloc(size,sizeof(int));
	//iterate to read all the files in command
	for(int i=2; i<argc;i++){
		//opens file
		FILE* f = fopen(*(argv+i),"r");
		if(f==NULL){
			perror("ERROR: INVALID MEMORY REFERENCE\n");
			return EXIT_FAILURE;
		}
		int rd=0;
		char c;
		int num=0;
		int p = 10;
		int check = 0;
		//reads all words til EOF
		while(rd!=EOF){
			//stores character in c 
			rd = fscanf(f,"%c",&c);
			//if(rd==0){break;}
			if(c>=48&&c<=57){
				num = num*p;
				num+=(c-'0');
				check = 1;
			}
			else{
				if(check==1){
					int index = num%size;
					int size_index = *(track+index); 
					printf("Read %d ==> cache index %d ",num,index);
					//if that index is empty
					if(*(ht+index)==NULL){
						*(ht+index)=calloc(1,sizeof(int));
						*(*(ht+index)) = num;
						*(track+index) = 1;
						printf("(calloc)\n");
					}
					else{
						int exist = 0;
						for(int i=0;i<size_index;i++){
							//if that number exists
							if(*(*(ht+index)+i)==num){
								printf("(skipped)\n");
								exist = 1;
								break;
							}
						}
						if(exist==0){
							*(ht+index)=realloc(*(ht+index),(size_index+1)*sizeof(int));
							*(*(ht+index)+size_index)=num;
							*(track+index)=size_index+1;
							printf("(realloc)\n");
						}
					}
				}
				num = 0;
				check = 0;
			}
		}
		fclose(f);
	}
    printf("========================================\n");
    for(int i=0;i<size;i++){
        if(*(ht+i)!=NULL){
            printf("Cache index %d ==> %d",i,*(*(ht+i)));
            int size_index = *(track+i);
            for(int j=1;j<size_index;j++){
                printf(", %d",*(*(ht+i)+j));
            }
            printf("\n");
        }
        free(*(ht+i));
    }
	free(ht);
	free(track);
	return EXIT_SUCCESS;
}
