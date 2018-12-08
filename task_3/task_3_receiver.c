#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#define BILLION 1000000000L;
#define MSG_SIZE 1024
typedef struct msgbuf {
    	long    mtype;
    	char    mtext[MSG_SIZE];
} message_buf;
int main(){
	int msqid;
    	key_t key;
	message_buf rbuf;
	struct timespec start, stop;
	double accum;
	
	int size = 0;
	printf("ENTER SIZE OF THE FILE:\n");
	scanf("%d", &size);
	char* rec_data=malloc(sizeof(char)*size);
	if( clock_gettime( CLOCK_MONOTONIC, &start) == -1 ) {
      		perror( "clock gettime" );
	      	return EXIT_FAILURE;
	}	
	int buf_size = 0;
	printf("ENTER SIZE OF BUF:\n");
	scanf("%d", &buf_size);
	key = ftok("input.txt",1);
	int msgflg = IPC_CREAT | 0666;
	
	 if ((msqid = msgget(key, msgflg)) < 0) {
        	perror("msgget");
       	 	exit(1);
	}
	
	int i = 0;
	int k = 0;
	int al = size/buf_size;
	for(k=0;k<(al);k++){
		if(msgrcv(msqid, &rbuf, buf_size, 1, 0)<0){
			perror("NO MESSEGES");
			exit(1);
		}
        	for(i=0;i<buf_size;i++){
			rec_data[buf_size*k+i]=rbuf.mtext[i];	
		}
	}	
	if( clock_gettime( CLOCK_MONOTONIC, &stop) == -1 ) {
      		perror( "clock gettime" );
      		return EXIT_FAILURE;
	}
	
	accum = ( stop.tv_sec - start.tv_sec )
             + (double)( stop.tv_nsec - start.tv_nsec )
/                (double)BILLION;
	accum = accum - accum - accum;
	printf( "time=%lf seconds\n", accum);
	free(rec_data);	
	return 0;
}
