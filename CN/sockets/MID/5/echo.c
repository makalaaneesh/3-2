#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include"../../../helper.h"
#define TOKEN 123321

int main(){
	int *pid;
	int shmid;
	shmid = allocateSharedMemory(sizeof(int), TOKEN);
	pid = (int *) mapSharedMemory(shmid);
	// printf("Starting upper\n");
	// fflush(stdout);
	char *str  = (char *)malloc(sizeof(char)*250);
	size_t write_size;

	while(1){
		
		ssize_t in  = getline(&str, &write_size, stdin);
		

		printf("%s", str);
		fflush(stdout);
		
		if(strcmp(str,"exit\n") == 0){
			*pid = getpid();
			kill(getppid(), SIGUSR2);
		}
		memset(str, 0, sizeof(str));
	}
}