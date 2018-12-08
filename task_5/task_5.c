#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <pthread.h>
#include <semaphore.h>
#include <sys/shm.h>
#include <string.h>
#include <signal.h>
#include <time.h>
int size;
char* rec;
int j = 0;
void sig_handler(int signo)
{
        switch(signo){
                case SIGUSR2:
                        rec[j]='0';
                        j++;
                        break;
                case SIGUSR1:
                        rec[j]='1';
                        j++;
                        break;
        }
}
int main(){
        int i = 0;
        int fdin;
        struct stat statbuf;
        fdin = open("in.txt", O_RDONLY);
	if(fdin<0){
                  printf("can't open input\n");
                  exit(-1);
        }
        fstat(fdin, &statbuf);
        size = statbuf.st_size;
        printf("size=[%d]\n",size);

        char* data;
        data = malloc(sizeof(char)*size);
        read(fdin, data, size);
        close(fdin);
        rec = malloc(sizeof(char)*size);
        sem_t *sem;
        sem = sem_open("NAME", O_CREAT, 0776, 0);

        struct timespec tim_sender, tim2, tim_rec;
        int sleep_t = 5000000; 
        tim_sender.tv_sec = 0;
        tim_sender.tv_nsec = sleep_t;
        tim_rec.tv_sec = 0;
        tim_rec.tv_nsec = sleep_t/2*3;
        srand(time(0));
        pid_t pid = fork();
        if(pid<0){
                printf("FORK ERR\n");
                exit(-1);
        }
        if(pid)
        {
                sem_wait(sem);
                for(i = 0; i < size; i++)
                {
                        kill(pid, data[i] == '1' ? SIGUSR1 : SIGUSR2);
                        nanosleep(&tim_sender, &tim2);
               }
        }
        else
        {
                if(signal(SIGUSR1, sig_handler) == SIG_ERR)
                        printf("\ncan't catch a SIGUSR1\n");
                if(signal(SIGUSR2, sig_handler) == SIG_ERR)
                        printf("\ncan't catch a SIGUSR2\n");
                sem_post(sem);

                while(j < size)
                         nanosleep(&tim_sender, &tim2);

                for(i=0;i<(size-1);i++)
                        printf("%c", rec[i]);
                free(rec);
        }
        free(data);
        return 0;
}

