/* hw3-main.c */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

extern int next_thread_id;
extern int max_squares;
extern int total_tours;

int m_r[8] = {-2,-1, 1, 2,2,1,-1,-2};
int m_c[8] = {-1,-2,-2,-1,1,2, 2, 1};

int n_rows;
int n_col;
int full_move;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

typedef struct {
    int ** board;
    int r;
    int c;
    int n_moves;
    int id;
} board_t;

void print_board(int**board){
    for(int i=0;i<n_rows;i++){
        for(int j=0;j<n_col;j++){
            printf("%d ",board[i][j]);
        }
        printf("\n");
    }
}

int possible_moves(int r, int c,int**board,int * possible){
    int moves = 0;
    for(int i=0;i<8;i++){
        int next_r = r+m_r[i];
        int next_c = c+m_c[i];
        //if next move is inside board parameter
        if(0<=next_r && next_r<n_rows && 0<=next_c && next_c<n_col){
            //if that spot hasn't been visited
            if(board[next_r][next_c]==0){
                possible[moves]=i;
                moves+=1;
            }
        }
    }
    return moves;
}

void* knight_tour(void*args){
    
    board_t* args2 = args;
    int r = args2->r;
    int c = args2->c;
    int** board = args2->board;
    int n_moves = args2->n_moves;
    int id = args2->id;
    int* returnid = malloc(sizeof(int));
    //get number of possible moves
    int *possible =calloc(8,sizeof(int));
    int *p_m = malloc(sizeof(int));
    *p_m =possible_moves(r,c,board,possible);
    
    //if there is only one possible move
    if(*p_m==1){
        free(args2);
        free(returnid);
        //get next move
        int next_r = r + m_r[possible[0]];
        int next_c = c + m_c[possible[0]];
        free(possible);
        //mark visited
        board[next_r][next_c] = 1;
        
        //printf("T%d: 1 possible moves after move #%d\n",id,n_moves);
        
        board_t* args3 = malloc(sizeof(board_t));
        args3->board = board;
        args3->r = next_r;
        args3->c = next_c;
        args3->n_moves = n_moves+1;
        args3->id = id;
        knight_tour(args3);
        
    }
    //if there are more than one possible moves
    else if(*p_m>1){
        free(args2);
        if(id==0){
            printf("MAIN: %d possible moves after move #%d; creating %d child threads...\n",*p_m,n_moves,*p_m);
        }
        else{
            printf("T%d: %d possible moves after move #%d; creating %d child threads...\n",id,*p_m,n_moves,*p_m);
        }
        
        //create child threads
        pthread_t tid[8];
        for(int i=0;i<*p_m;i++){
            int next_r = r + m_r[possible[i]];
            int next_c = c + m_c[possible[i]];
            //printf("next_r: %d, next_c: %d\n",next_r,next_c);
            //create a new copy board
            int ** n_board = calloc(n_rows,sizeof(int*));
            for(int j=0;j<n_rows;j++){
                n_board[j] = calloc(n_col,sizeof(int));
                for(int k=0;k<n_col;k++){
                    n_board[j][k] = board[j][k];
                }
            }
            n_board[next_r][next_c] = 1;
            board_t* args3 = malloc(sizeof(board_t));
            args3->board = n_board;
            args3->r = next_r;
            args3->c = next_c;
            args3->n_moves = n_moves+1;
            args3->id = next_thread_id;
            pthread_mutex_lock(&mutex);
            next_thread_id+=1;
            pthread_mutex_unlock(&mutex);
            int rc = pthread_create(&tid[i],NULL,knight_tour,args3);
            if ( rc != 0 ){
                fprintf( stderr, "pthread_create() failed (%d)\n", rc );
            }
            //free(possible);
#ifdef NO_PARALLEL
            int* return_value = malloc(sizeof(int));
            pthread_join(tid[i], (void**)&return_value);
            if(id==0){
                printf("MAIN: T%d joined\n",*return_value);
            }
            else{
                printf("T%d: T%d joined\n", id,*return_value);
            }
            free(return_value);
#endif
        }
#ifndef NO_PARALLEL
        //join thread
        for(int i=0;i<*p_m;i++){
            int* return_value = malloc(sizeof(int));
            pthread_join(tid[i], (void**)&return_value);
            if(id==0){
                printf("MAIN: T%d joined\n",*return_value);
            }
            else{
                printf("T%d: T%d joined\n", id,*return_value);
            }
            free(return_value);
        }
#endif
    }
    //tour completed
    else{
        free(args2);
        if(n_moves == full_move){
            pthread_mutex_lock(&mutex);
            max_squares=n_moves;
            pthread_mutex_unlock(&mutex);
            if(id==0){
                printf("MAIN: Sonny found a full knight's tour; incremented total_tours\n");
            }
            else{
                printf("T%d: Sonny found a full knight's tour; incremented total_tours\n",id);
            }
            pthread_mutex_lock(&mutex);
            total_tours+=1;
            pthread_mutex_unlock(&mutex);
        }
        //dead end
        else{
            //update max_square
            if(n_moves>max_squares){
                pthread_mutex_lock(&mutex);
                max_squares=n_moves;
                pthread_mutex_unlock(&mutex);
                if(id==0){
                    printf("MAIN: Dead end at move #%d; updated max_squares\n",n_moves);
                }
                else{
                    printf("T%d: Dead end at move #%d; updated max_squares\n",id,n_moves);
                }
            }
            else{
                if(id==0){
                    printf("MAIN: Dead end at move #%d\n",n_moves);
                }
                else{
                    printf("T%d: Dead end at move #%d\n",id,n_moves);
                }
            }
            for(int i=0;i<n_rows;i++){
                free(board[i]);
            }
            free(board);
        }
        if(id!=0){
            *returnid = id;
            pthread_exit(returnid);
        }
    }
    if(id!=0){
        *returnid = id;
        for (int i = 0; i < n_rows; i++){
            free(board[i]);
        }
        free(board);
        pthread_exit(returnid);
    }
    //free(possible);
    free(returnid);
    return returnid;
}

/* write the simulate() function and place all of your code in hw3.c */
int simulate( int argc, char * argv[] ){
    //if command line argument does not have 4 arguments ERROR
    if(argc!=5){
        fprintf(stderr, "ERROR: Invalid argument(s)\n");
        fprintf(stderr, "USAGE: a.out <m> <n> [<x>]\n");
        return EXIT_FAILURE;
    }
    
    //initialize global variables
    next_thread_id = 1;
    max_squares = 0;
    total_tours = 0;
    
    //initialize variables
    n_rows = atoi(argv[1]);
    n_col = atoi(argv[2]);
    full_move = n_rows * n_col;
    int r = atoi(argv[3]);
    int c = atoi(argv[4]);
    
    
    //if the board's row or columns is less than 2 ERROR
    if(n_rows<2 || n_col<2){
        fprintf(stderr, "ERROR: Invalid argument(s)\n");
        fprintf(stderr, "USAGE: a.out <m> <n> [<x>]\n");
        return EXIT_FAILURE;
    }
    
    
    //initialize board
    int ** board = calloc(n_rows,sizeof(int*));
    for(int i=0;i<n_rows;i++){
        board[i] = calloc(n_col,sizeof(int));
    }
    board[r][c] = 1;
    
    board_t * args = malloc(sizeof(board_t));
    args->board = board;
    args->r = r;
    args->c = c;
    args->n_moves = 1;
    args->id = 0;
    
    printf("MAIN: Solving Sonny's knight's tour problem for a %dx%d board\n",n_rows,n_col);
    printf("MAIN: Sonny starts at row %d and column %d (move #1)\n",r,c);
    knight_tour(args);
    
    if(total_tours==0){
        if(max_squares==1){
            printf("MAIN: Search complete; best solution(s) visited %d square out of %d\n",max_squares,full_move);
        }
        else{
            printf("MAIN: Search complete; best solution(s) visited %d squares out of %d\n",max_squares,full_move);
        }
    }
    else{
        printf("MAIN: Search complete; found %d possible paths to achieving a full knight's tour\n",total_tours);
    }
    //printf("next thread id: %d\n",next_thread_id);
    return EXIT_SUCCESS;
}


