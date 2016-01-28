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


char *fifo = "server";
int fifo_fd;
char *buf;

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
	pid_t pid;
};

struct process_id *client_pid;
struct process_id *service_pid;


void service_completed(int signo){
	// one of the services has signaled the server that it has completed. it's pid is stored in shared memory @ service_pid

	printf("%d has signaled completion of work\n", service_pid->pid);

	int i;
	for(i = 0; i< MAX_SERVICES; i++){
		if (services[i].pid == (service_pid->pid)){// type cast to pid_t before comparing
			services[i].available = 1;
		}
	}
	if(signal(SIGUSR1, service_completed) == SIG_ERR){
		printf("%s\n", "Error in catching SIGUSR1");
	}

}

int server_init(){
	// opening a fifo for the clients to connect to.
	buf = (char *)malloc(sizeof(char)*PIPE_BUF);
	mkfifo(fifo, 0666);
	fifo_fd = open(fifo, O_RDWR); //read write so that the fifo does not get closed when there are no clients
	print_error(fifo_fd, "failed to open fifo");


	shmid_client = allocateSharedMemory(sizeof(struct process_id), token_client);
	printf("Shared memory for client id is %d\n", shmid_client);
	client_pid = (struct process_id *) mapSharedMemory(shmid_client);

	shmid_service = allocateSharedMemory(sizeof(struct process_id), token_service);
	printf("Shared memory for service id is %d\n", shmid_service);
	service_pid = (struct process_id *) mapSharedMemory(shmid_service);

	if(signal(SIGUSR1, service_completed) == SIG_ERR){
		printf("%s\n", "Error in catching SIGUSR1");
	}
	int i;
	for(i = 0 ; i<MAX_SERVICES; i++){
		int pid = fork();
		print_error(pid, "FAiled to fork");
		if (pid == 0){
			execl("s","s",(char *)0);
		}
		else{
			services[i].pid = pid;
			services[i].available = 1;


		}
	}

}


void handle_client(char* client_fifo){
	int s_id = 0;
	int i;
	for(i = 0 ; i<MAX_SERVICES; i++){
		if (services[i].available){
			s_id = services[i].pid;
			break;
		}

	}
	if (s_id == 0){
		printf("NO service available\n");
		return;
	}


	int pid = atoi(client_fifo);
	client_pid->pid = pid;
	printf("Assigning process %d to service %d\n", pid, s_id);
	service_pid->pid = getpid();
	kill(s_id, SIGUSR1);
	services[i].available = 0;

}
void process_input(char *input){
	printf("%s\n", input);
	char *stringp = (char *)malloc(sizeof(input));
	strcpy(stringp, input);
    const char *delim = "|";
    char *token;
 
    //stringp is updated to point to the next token 
    token = strtok(stringp, delim); 

	char * client_pid = (char *)malloc(sizeof(token));
	strcpy(client_pid,token);

	printf("New client added %s\n", client_pid);
	fflush(stdout);
    handle_client(client_pid);
    
	memset(input, 0, sizeof(input));
}

int main(){
	server_init();
	printf("Server ON.FD is %d\n", fifo_fd);


	while(1){

		int r = read(fifo_fd, buf, PIPE_BUF);
		print_error(r, "read failed");
		process_input(buf);


	}
	close(fifo_fd);
	unlink(fifo);
	
}
