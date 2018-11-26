#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#include <sys/wait.h>
int main()
{
	printf("to stop the procces print:11\n");
	int i = 0;
	int num=0, l = 0;
	int maxlen = sysconf(_SC_ARG_MAX)+1;
	char* com;
	char** str_arg;
	char delim[]=" ,-\n";
	while(1)
	{
		com=malloc(sizeof(char)*maxlen);
	        printf("enter a command:");
		i=0;
	        scanf("%[^\n]%*c", com);
		l = 0;
		l = strlen(com);
		if(*(com)=='1' && *(com+1)=='1')
			break;
		num = 0;
		for(i=0;i<l;i++)
		{
			if(*(com+i)==' ' || *(com+i)==',' || *(com+i)=='-')
			{
				num++;
			}
		}

		i = 0;
		str_arg = malloc(sizeof(char*)*(num+1));
		for(char *p=strtok(com, delim); p!=NULL; p = strtok(NULL, delim))
	
		{
			*(str_arg+i)=p;
			i++;	
		}
		pid_t pid = fork();
		if(pid<0)
		{
			printf("fork error\n");
			return -1;
		}
		if(pid)
		{
			int status;
			waitpid(pid, &status, 0);
			printf("Ret code: %d\n", WEXITSTATUS(status));
		}
		else
		{
			execvp(str_arg[0],str_arg);
		}
		
		free(str_arg);
		free(com);
	}
	return 0;
}
