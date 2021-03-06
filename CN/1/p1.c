#include"../helper.h"
#include <signal.h>

struct sems
{
	int s1;
	int s2;
	int x;
	int y;
};

key_t token;
struct sems *s;
int shmid;

void clean(int signo){
	cleanUpShm(shmid);
	cleanUpSem(s->s1);
	cleanUpSem(s->s2);
	printf("Cleaned up. You hit ctrl-c\n");
	exit(1);
}

int main(){

	token = ftok(".",'r');
	shmid = allocateSharedMemory(sizeof(struct sems), token);
	s = (struct sems *) mapSharedMemory(shmid);
	s->x = 0;
	s->y = 1;

	if(signal(SIGINT, clean) == SIG_ERR){
		printf("%s\n", "Error in catching SIGINT");
	}

	printf("P1.this is x %d \n",s->x);
	printf("P1.this is y %d \n",s->y);

	sem_init(&s->s1, 0, token+1);
	sem_init(&s->s2, 0, token+2);

	// printf("P1.this is s1 %d \n",s->s1);
	// printf("P1.this is s2 %d \n",s->s2);
	while(1){
		printf("P1 just wrote X as %d\n", s->x);
		sem_up(&s->s1);
		sem_down(&s->s2);
		int y = s->y;
		s->x = y+1;
	}

	raise(SIGINT);
	// printf("unlocked\n");
	// sem_up(&s->s1);
	// printf("first\n");
	// sem_down(&s->s1);
	// printf("second\n");
	// sem_down(&s->s1);
	// printf("fourth\n");
}