#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <limits.h>
#include"../helper.h"


int status = 0; // for to wait . 1 to proceed.
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


void incoming_client(int signo){

}

int init(){
	shmid_client = allocateSharedMemory(sizeof(struct process_id), token_client);
	printf("Shared memory for client id is %d\n", shmid_client);
	client_pid = (struct process_id *) mapSharedMemory(shmid);

	shmid_service = allocateSharedMemory(sizeof(struct process_id), token_service);
	printf("Shared memory for service id is %d\n", shmid_service);
	service_pid = (struct process_id *) mapSharedMemory(shmid-service);

	if(signal(SIGUSR1, incoming_client) == SIG_ERR){
		printf("%s\n", "Error in catching SIGUSR1");
	}
}

int main(){
	while(1){
		while(status == 0);

	}
}