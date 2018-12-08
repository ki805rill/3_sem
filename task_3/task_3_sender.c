#include<sys/types.h>
#include<stdio.h>
#include<stdlib.h>
#include<sys/stat.h>
#include<fcntl.h>
#include <time.h>
#include<sys/ipc.h>
#include<unistd.h>
#include<string.h>
#include<errno.h>
#include <sys/msg.h>
#define BILLION 1000000000L;
#define MSG_SIZE 1024
typedef struct msgbuf {
         long    mtype;
         char    mtext[MSG_SIZE];
} message_buf;
int main(){
	struct timespec start, stop;
	double accum;

	if( clock_gettime( CLOCK_MONOTONIC, &start) == -1 ) {
      		perror( "clock gettime" );
      		return EXIT_FAILURE;
	}

	struct stat statbuf;
	int fdin;
	fdin = open("XL.txt", O_RDONLY);
  	if(fdin<0){
		  printf("can't open input\n");
		  exit(-1);
  	}
	fstat(fdin, &statbuf);
	const int size = statbuf.st_size;
	printf("size=[%d]\n",size);
	
	char* data= malloc(sizeof(char)*size);
	read(fdin,data, size);
	
	int msqid;
    	int msgflg = IPC_CREAT | 0666;
    	key_t key;
    	message_buf sbuf;
	int buf_length;
	
	key = ftok("input.txt",1);
	
	if ((msqid = msgget(key, msgflg )) < 0) {
        	perror("msgget");
        	exit(1);
	}
	else{ 
		printf("msgget: msgget succeeded: msqid = %d\n", msqid);
	}
	
	sbuf.mtype = 1;
	
	printf("ENTER SIZE OF THE BUF:\n");
	scanf("%d", &buf_length);
	
	int al = size/buf_length;
	int i,j,k;
	
	for(i=0;i<al;i++){
		for(j=0;j<buf_length;j++){
			sbuf.mtext[j]=data[buf_length*i+j];
		}
		msgsnd(msqid, &sbuf, buf_length, IPC_NOWAIT);
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
	free(data);	
	return 0;
}
