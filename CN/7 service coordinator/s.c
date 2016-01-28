#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <limits.h>
#include"../helper.h"
#include <signal.h>
#define KYEL  "\x1B[33m"

pid_t server_pid;
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
	pid_t pid;
};

struct process_id *client_pid;
struct process_id *service_pid;


void incoming_client(int signo){
	server_pid = service_pid->pid;
	status = 1;

}

void signal_completed(){
	service_pid->pid = getpid();
	status = 0;
	printf("Sending complted signal to server\n");
	kill(server_pid, SIGUSR1);
	if(signal(SIGUSR1, incoming_client) == SIG_ERR){
		printf("%s\n", "Error in catching SIGUSR1");
	}

}

int init(){
	shmid_client = allocateSharedMemory(sizeof(struct process_id), token_client);
	printf("Shared memory for client id is %d\n", shmid_client);
	client_pid = (struct process_id *) mapSharedMemory(shmid_client);

	shmid_service = allocateSharedMemory(sizeof(struct process_id), token_service);
	printf("Shared memory for service id is %d\n", shmid_service);
	service_pid = (struct process_id *) mapSharedMemory(shmid_service);


	if(signal(SIGUSR1, incoming_client) == SIG_ERR){
		printf("%s\n", "Error in catching SIGUSR1");
	}
}

int main(){
	init();
	char client_fifo[10];
	while(1){
		printf("%d:Waiting for client.\n", getpid());
		while(status == 0);
		sprintf(client_fifo, "%d", client_pid->pid);
		int o = open(client_fifo, O_WRONLY);
		print_error(o, "Failed to open clientfifo");

		char msg[PIPE_BUF];
		snprintf(msg, sizeof(msg), "Hello %s! This is from the service.\n", client_fifo);
		printf("%d:Sending a message to client\n", getpid());
		sleep(5);
		int w = write(o, msg, sizeof(msg)); //writing to server
		print_error(w, "write to fifo failed");
		fflush(stdout);

		signal_completed();
	}
}