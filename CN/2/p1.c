#include"../helper.h"

#include <signal.h>

struct items
{
	int x;
	int y;
	pid_t p1_pid;
	pid_t p2_pid;
};

struct items *s;
key_t token;
int shmid; 

void sig_action(int signo){
	int y = s->y;
	s->x = y + 1;
	printf("P1 just wrote X as %d\n", s->x);
	kill(s->p2_pid, SIGUSR1);
	if(signal(SIGUSR1, sig_action) == SIG_ERR){
		printf("%s\n", "Error in catching SIGUSR1");
	}

}


void clean(int signo){
	cleanUpShm(shmid);
	printf("Cleaned up. You hit ctrl-c\n");
	exit(1);
}


int main(){
	token = ftok(".",'r');
	printf("token is %d\n", token);
	shmid = allocateSharedMemory(sizeof(struct items), token);
	printf("shm id is %d\n", shmid);
	s = (struct items *) mapSharedMemory(shmid);
	s->x = 0;
	s->y = 0;
	s->p1_pid = getpid();
	if(signal(SIGUSR1, sig_action) == SIG_ERR){
		printf("%s\n", "Error in catching SIGUSR1");
	}
	if(signal(SIGINT, clean) == SIG_ERR){
		printf("%s\n", "Error in catching SIGINT");
	}
	printf("P1.this is p1 pid %d \n",s->p1_pid);
	printf("P1.this is p2 pid %d \n",s->p2_pid);
	while(1)
		pause();

}