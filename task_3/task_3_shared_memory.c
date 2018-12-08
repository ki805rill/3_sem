#include<sys/types.h>
#include<stdio.h>
#include<stdlib.h>
#include<sys/stat.h>
#include<fcntl.h>
#include <time.h>
#include<sys/ipc.h>
#include<sys/shm.h>
#include<unistd.h>
#include<string.h>
#include<errno.h>
#include <semaphore.h>
#define BILLION 1000000000L;
int main(){
	sem_t* sem;
	sem_t* sem1;
	struct timespec start, stop;
        double accum;
	if( clock_gettime( CLOCK_MONOTONIC, &start) == -1 ) {
                perror( "clock gettime" );
                return EXIT_FAILURE;
        }

	struct stat statbuf;
	int fdin;
	fdin = open("input.txt", O_RDONLY);	
  	if(fdin<0){
		  printf("can't open input\n");
		  exit(-1);
  	}
	fstat(fdin, &statbuf);
	const int size = statbuf.st_size;
	printf("size=[%d]\n",size);
	
	int shmid, sh_size,i,j,l,k;
	printf("ENTER SIZE OF SHARED MEMORY:\n");
	scanf("%d", &sh_size);
	char* shared;
	char* data;
	data = malloc(sizeof(char)*size);
	char* drc = malloc(sizeof(char)*size);
	read(fdin,data, size);
	
	int al = size/sh_size;	
	shmid = shmget(IPC_PRIVATE, sh_size, IPC_CREAT|0666);
	pid_t pid = fork();
	if(pid<0){
		printf("FORK ERROR\n");
		exit(-1);
	}
	if(pid){
		shared = shmat(shmid, (void*)0,0);
		sem = sem_open("NAME", O_CREAT, 0776, 0);
		sem1= sem_open("NAME1", O_CREAT, 0776, 0);
		sem_post(sem1);
		for(l=0;l<al;l++){
			
			sem_wait(sem1);
			for(j=0;j<sh_size;j++){
				shared[j]=data[l*sh_size+j];
			}
			sem_post(sem);
		}
	}
	else{
		char* rec = shmat(shmid, (void*)0,0);
		sem = sem_open("NAME", 0);
                sem1= sem_open("NAME1", 0);
		for(k=0;k<al;k++){
			sem_wait(sem);
			
			for(i=0;i<sh_size;i++){
				drc[k*sh_size+i]=rec[i];
			}
			
			sem_post(sem1);
		}
				if( clock_gettime( CLOCK_MONOTONIC, &stop) == -1 ) {
               		 perror( "clock gettime" );
        	        return EXIT_FAILURE;
	        }
		accum = ( stop.tv_sec - start.tv_sec )
             + (double)( stop.tv_nsec - start.tv_nsec )
/                (double)BILLION;
       		 accum = accum - accum - accum;
	        printf( "time=%lf seconds\n", accum );

		shmctl(shmid,IPC_RMID,0);
	}
	free(drc);
	free(data);
	return 0;
}
