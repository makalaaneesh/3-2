// http://www.questionscompiled.com/named-pipes-fifo.html
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <limits.h>

#define MAX_CLIENTS 10
#define NO_OF_SERVICES 2

char *fifo = "server";
char *buf;

int client_fd_index;
int client_fifos_fds[MAX_CLIENTS];
char *list_of_services[NO_OF_SERVICES];

print_error(int val, char* msg){
	if (val < 0){
		printf("%s error code = %d\n", msg, val);
		exit(1);
	}
}


int exists_in(char * str, char* list[], int size){
	int i;
	for(i = 0; i < size; i++){
		if (strcmp(str, list[i]) == 0)
			return i;
	}
	return -1;
}

int server_init(){
	buf = (char *)malloc(sizeof(char)*PIPE_BUF);
	mkfifo(fifo, 0666);
	int fd = open(fifo, O_RDWR); //read write so that the fifo does not get closed when there are no clients
	print_error(fd, "failed to open fifo");
	client_fd_index = 0;

	char * service1 = (char *)malloc(sizeof(char)*4);
	strcpy(service1, "s1");
	char * service2 = (char *)malloc(sizeof(char)*4);
	strcpy(service2, "s2");
	list_of_services[0] = service1;
	list_of_services[1] = service2;


	return fd;
}


void process_input(char *input){
	char *stringp = (char *)malloc(sizeof(input));
	strcpy(stringp, input);
    const char *delim = "|";
    char *service;
 
    //stringp is updated to point to the next token 
    service = strtok(stringp, delim); 


    int service_index = exists_in(service, list_of_services, NO_OF_SERVICES);
    if(service_index != -1){
    	char *client_fifo = strtok(NULL, delim);
    	printf("Client %s requested for service %s\n", client_fifo, service);
    	int client_fifo_fd = open(client_fifo, O_RDWR);
    	client_fifos_fds[client_fd_index++] = client_fifo_fd;
    	int pid = fork();
    	if (pid == 0){
    		// int in = client_fifo_fd;
    		int out = client_fifo_fd;


			if (out != 1){
				dup2(out,1);
				close(out);
			} // child will write to the pipe

			execl(service, service, (char*)0);
    	}

    }
    else{
    	printf("Service does not exist\n");
    }

	memset(input, 0, sizeof(input));
}


int main(){

	int fd = server_init();
	printf("Server ON.FD is %d\n", fd);


	while(1){

		int r = read(fd, buf, PIPE_BUF);
		print_error(r, "read failed");
		process_input(buf);


	}
	close(fd);
	unlink(fifo);
	



}