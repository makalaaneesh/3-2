#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <limits.h>

#define MAX_SERVICES 3
struct service{
	pid_t pid;
	int available;
}services[MAX_SERVICES];

int token_client = 54321; //for shared memory
int token_service = 12345;
char * client_fifo;
int shmid_client, shmid_service;


print_error(int val, char* msg){
	if (val < 0){
		printf("%s error code = %d\n", msg, val);
		exit(1);
	}
}

struct process_id{
	pid_t;
};

struct process_id *client_pid;
struct process_id *service_pid;


void service_completed(int signo){
	// one of the services has signaled the server that it has completed. it's pid is stored in shared memory @ service_pid
	
	if(signal(SIGUSR1, service_completed) == SIG_ERR){
		printf("%s\n", "Error in catching SIGUSR1");
	}
}

int server_init(){
	shmid_client = allocateSharedMemory(sizeof(struct process_id), token_client);
	printf("Shared memory for client id is %d\n", shmid_client);
	client_pid = (struct process_id *) mapSharedMemory(shmid);

	shmid_service = allocateSharedMemory(sizeof(struct process_id), token_service);
	printf("Shared memory for service id is %d\n", shmid_service);
	service_pid = (struct process_id *) mapSharedMemory(shmid-service);

	if(signal(SIGUSR1, service_completed) == SIG_ERR){
		printf("%s\n", "Error in catching SIGUSR1");
	}

}


print_error(int val, char* msg){
	if (val < 0){
		printf("%s error code = %d\n", msg, val);
		exit(1);
	}
}