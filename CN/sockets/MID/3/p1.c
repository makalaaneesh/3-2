#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include"../../../helper.h"

struct temp{
	int pid;
	int fd;
};

int *PID;

struct temp p[2];
int pindex = 0;

void _signal(int signo){
	// printf("THis is signal!!!!\n");
	// sleep(10);
	fflush(stdout);
	int pp = *(PID);
	// printf("pp is %d\n", pp);
	int i;
	int index = -1;
	for(i =0 ; i<2;i++){
		if(p[i].pid == pp){
			index = i;
			break;
		}


	}
	// printf("index%d\n", index);
	fflush(stdout);
	if(index >= 0){
		char * buf;
		buf = (char *)malloc(sizeof(char)*100);
		int r = read(p[index].fd, buf, 100);
		printf("%s\n", buf);
		fflush(stdout);
	}

	signal(SIGUSR1, _signal);
}

void _p2(){
	int pipefd[2];
	pipe(pipefd);
	int pid = fork();
	if (pid == 0){
		printf("%d\n", getpid());
		close(pipefd[0]);
		dup2(pipefd[1], 1);
		execl("p2","p2",(char*)0);
	}
	else{
		close(pipefd[1]);
		p[pindex].pid = pid;
		p[pindex].fd = pipefd[0];
		pindex++;
		printf("Child just created 's pid is %d\n", pid);
	}


}


void _p6(){
	FILE *f = popen("./p6", "w");
	int fd = fileno(f);
	dup2(fd,1);
}



int main(){
	signal(SIGUSR1, _signal);
	int token = 123413;
	int shmid;
	shmid = allocateSharedMemory(sizeof(int), token);
	printf("shmid is %d\n", shmid);
	PID = (int *) mapSharedMemory(shmid);
	*(PID) = 0;

	_p2();
	_p6();
	while(1){
		sleep(7);
		printf("[p1->hey]\n");
		fflush(stdout);
		// sleep(10);
	}

}