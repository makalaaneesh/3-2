#include"helper.h"

struct sems
{
	int s1;
	int s2;
	int x;
	int y;
};

key_t token;
struct sems *s;

int main(){
	token = ftok(".",'r');
	int shmid = allocateSharedMemory(sizeof(struct sems), token, 0);
	s = (struct sems *) mapSharedMemory(shmid);
	s->x = 0;
	s->y = 1;
	
	printf("P2.this is x %d \n",s->x);
	printf("P2.this is y %d \n",s->y);

	// sem_init(&s->s1, 0, token+1);
	// sem_init(&s->s2, 0, token+2);

	// printf("P2.this is s1 %d \n",s->s1);
	// printf("P2.this is s2 %d \n",s->s2);
	int i;
	for(i = 0; i<5;i++){
		sem_down(&s->s1);
		int x = s->x;
		s->y = x + 1;
		printf("P2 just wrote Y as %d\n", s->y);
		sem_up(&s->s2);
	}
	// printf("third\n");
	// sem_up(&s->s1);
}