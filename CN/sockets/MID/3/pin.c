#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include"../../../helper.h"

int main(){
	// printf("THis is p2\n");
	fflush(stdout);
	int token = 123413;
	int shmid;
	shmid = allocateSharedMemory(sizeof(int), token);
	// printf("shmid is %d\n", shmid);
	int *pid = (int *) mapSharedMemory(shmid);
	// printf("Starting upper\n");
	// fflush(stdout);
	char *str  = (char *)malloc(sizeof(char)*250);
	size_t write_size;

	while(1){
		
		// ssize_t in  = getline(&str, &write_size, stdin);
		// printf("p\n");
		strcpy(str,"hey");
		printf("[p2->%s]\n", str);
		// printf("p2-> %d\n", getpid());
		int x = getpid();
		*(pid) = x;

		kill(getppid(), SIGUSR1);
		fflush(stdout);
		memset(str, 0, sizeof(str));
		sleep(3);
	}
}