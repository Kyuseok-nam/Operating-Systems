/* hw2.c */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>

int m_row[8] = {-2,-1,1,2,2,1,-1,-2};
int m_col[8] = {-1,-2,-2,-1,1,2,2,1};


void block_on_waitpid( pid_t pid )
{
    int status;
    pid_t child_pid;

    child_pid = waitpid( pid, &status, 0 );   /* BLOCKING */
    if (child_pid==-1){perror("waitpid() failed");}
}

void print_board(int**board,int columns, int rows){
    for(int i=0; i<rows;i++){
        for(int j=0; j< columns; j++){
            printf("%d ",*(*(board+i)+j));
        }
        printf("\n");
    }
}

int possible_moves(int columns,int rows,int* x, int* y,int**board,int possible[8]){
    int moves = 0;
    for(int i=0;i<8;i++){
        int new_x =(*x)+m_col[i];
        int new_y =(*y)+m_row[i];
        if(0<=new_y && new_y<rows && 0<=new_x && new_x<columns){
            if(board[new_y][new_x]==0){
                possible[moves]=i;
                moves+=1;
            }
        }
    }
    return moves;
}
/*
void create_fork(int moves,int**board,int columns, int rows,int y, int x,int possible[8],int count_move){
    pid_t p;
    if(moves>0){
        printf("create child process\n");
        p = fork();
        if(p==-1){ perror( "fork() failed" ); }
        if(p>0){
            create_fork(moves-1,board,columns,rows,y,x,possible,count_move);
            block_on_waitpid(p);
        }
        else if(p==0){
            printf("PID %d\n",getpid());
            y = y+m_row[possible[moves-1]];
            x = x+m_col[possible[moves-1]];
            board[y][x]=count_move;
            print_board(board,columns,rows);
        }
    }
}
*/
void create_fork(int moves,int**board,int columns, int rows,int* y, int* x,int possible[8],int count_move){
    pid_t p;
    if(moves>0){
        //printf("create child process\n");
        p = fork();
        if(p==-1){ perror( "fork() failed" ); }
        if(p>0){
            create_fork(moves-1,board,columns,rows,y,x,possible,count_move);
            block_on_waitpid(p);
        }
        else if(p==0){
            //printf("PID %d\n",getpid());
            *y = (*y)+m_row[possible[moves-1]];
            *x = (*x)+m_col[possible[moves-1]];
            board[(*y)][(*x)]=count_move;
            int pos_moves = possible_moves(columns,rows,x, y,board,possible)
            if(pos_moves>1){
                create_fork(pos_moves,board,columns,rows,y,x,possible,count_move);
            }
            else if(pos_moves==1){
                
            }
        }
    }
}


int main(int argc, char**argv){
	if(argc!=5){
		fprintf(stderr,"ERROR: Invalid argument(s)\n");
		return EXIT_FAILURE;
	}
    //starting position
	int r = atoi(*(argv+3));
	int c = atoi(*(argv+4));
    int rows = atoi(*(argv+1));
    int columns = atoi(*(argv+2));
    
    if(0>r || r>=rows || 0>c || c>=columns){
        fprintf(stderr,"ERROR: Invalid argument(s)\n");
        fprintf(stderr,"USAGE: hw2.out <m> <n> <r> <c>\n");
        return EXIT_FAILURE;
    }
    
    int pipefd[2];
    int rc = pipe(pipefd);
    if(rc==-1){
        perror("pipe() failed");
        return EXIT_FAILURE;
    }
    
	
    //current position
    int* y = &r;
    int* x = &c;
    
	//create a board and mark the starting point
    //1 implies visited and 0 implies not visited
	int ** board = calloc(rows,sizeof(int*));
	for(int i=0; i<rows;i++){
		*(board+i)=calloc(columns,sizeof(int));
	}
    *(*(board+(*y))+(*x)) = 1;
    
    printf("PID %d: Solving Sonny's knight's tour problem for a %dx%d board\n",getpid(),rows,columns);
    printf("PID %d: Sonny starts at row %d and column %d (move #1)\n",getpid(),r,c);
    fflush(stdout);
    
    int possible[8]={0,0,0,0,0,0,0,0};
    int count_move=2;
    //bool parent = false;
    int num_tour=0;
    int max_move = 1;
    
    int moves = possible_moves(columns,rows,x,y,board,possible);
    if(moves == 1){
        while(moves==1){
            r = r+m_row[possible[0]];
            c = c+m_col[possible[0]];
            board[r][c]=count_move;
            if(count_move>max_move){max_move = count_move;}
            count_move+=1;
        }
    }
    else if(moves>1){
        create_fork
    }
    else if(moves==0){
        printf("PID %d: Dead end at move #%d\n",getpid(),count_move-1);
    }
    /*
    for(int i=0;i<moves;i++){
        possible[i]=0;
    }*/
    
    if(num_tour==0){
        printf("PID %d: Search complete; best solution(s) visited %d squares out of %d\n",getpid(),max_move,rows*columns);
    }
    else{
        printf("PID %d: Search complete; found %d possible paths to achieving a full knight's tour\n",getpid(),num_tour);
    }
    
	//free dynamcially allocated memories
	for(int i=0; i<rows; i++){
		free(*(board+i));
	}
	free(board);
}
