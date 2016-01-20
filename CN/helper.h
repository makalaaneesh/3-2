#include <stdlib.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/types.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <stdio.h>
#include <sys/types.h>


union semun{
	int val;    /* Value for SETVAL */
	struct semid_ds *buf;     /*Buffer for IPC_STAT, IPC_SET */
	unsigned short  *array;  /* Array for GETALL, SETALL */
	struct seminfo  *__buf;
};
/*-----------------------------Helper Functions -------------------------------------*/


int allocateSharedMemory(int n, int token){

	if (token == 0){
		token = IPC_PRIVATE;
	}
	return shmget(token, n, IPC_CREAT |0666);
	
}

void* mapSharedMemory(int id){
	void *addr;
	addr = shmat(id,NULL,0); // id, specify NULL so the system choses the space to map the memory, flags are usually 0
	return addr;
}

void cleanUpShm(int id){
	shmctl(id, IPC_RMID, NULL); // set the memory to be destroyed after the last process detaches it.
}

void cleanUpSem(int id){
	semctl(id,0,IPC_RMID);
}


void sem_down(int *semid){
	struct sembuf sb;
	sb.sem_num = 0;
	sb.sem_op = -1;
	sb.sem_flg = 0;
	int e;
	if((e =semop(*semid, &sb,1)) < 0){ //id, struct, number of semaphores
		printf("%s %d\n", "Error locking semaphore",e);
		exit(1);
	}
}
void sem_up(int *semid){
	struct sembuf sb;
	sb.sem_num = 0;
	sb.sem_op = 1;
	sb.sem_flg = 0;
	if((semop(*semid, &sb,1)) < 0){ //id, struct, number of semaphores
		printf("%s\n", "Error unlocking semaphore");
		exit(1);
	}
}


void sem_init(int *semaphore, int initial_value, int token){
	if (token == 0){
		token = IPC_PRIVATE;
	}
	if((*semaphore = semget(token, 1, 0666|IPC_CREAT)) < 0) {
		printf("%s\n", "Error creating sempahore");
		exit(1);
	}

	union semun temp;
	temp.val = initial_value;

	if((semctl(*semaphore, 0, SETVAL, temp)) < 0){
		printf("%s\n", "Error setting value to semaphore");
		exit(1);
	}
	printf("Assigned value to semaphore %d \n", *semaphore);
}
