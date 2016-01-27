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
#define NO_OF_SERVICES 1

char *fifo = "server";
char *buf;

int register_fd;
int client_fd_index;
int client_fifos_fds[MAX_CLIENTS];
char *list_of_services[NO_OF_SERVICES];
// int service_fifos[NO_OF_SERVICES];

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

// storing the service names
	char * service1 = (char *)malloc(sizeof(char)*4);
	strcpy(service1, "s1");
	char * service2 = (char *)malloc(sizeof(char)*4);
	strcpy(service2, "s2");
	char * service3 = (char *)malloc(sizeof(char)*4);
	strcpy(service3, "s3");
	list_of_services[0] = service1;
	// list_of_services[1] = service2;
	// list_of_services[2] = service3;

// storing the fifo names
	// char * servicefifo1 = (char *)malloc(sizeof(char)*4);
	// strcpy(servicefifo1, "s1fifo");
	// char * servicefifo2 = (char *)malloc(sizeof(char)*4);
	// strcpy(servicefifo2, "s2fifo");
	// char * servicefifo3 = (char *)malloc(sizeof(char)*4);
	// strcpy(servicefifo3, "s3fifo");
	// service_fifos[0] = servicefifo1;
	// service_fifos[1] = servicefifo2;
	// service_fifos[2] = servicefifo3;

	register_fd = fd;


	int i;
	for(i =0 ; i<NO_OF_SERVICES; i++){
		//popening each server

		char *service = list_of_services[i];

    	char command[50];
    	snprintf(command, sizeof(command), "./%s",service);
    	FILE *s = popen(command, "r");
		if(s == NULL){
			printf("popen error\n");
			exit(1);
		}
	}

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
    	// client_fifos_fds[client_fd_index++] = client_fifo_fd;
    	

    	// generating the service fifo to send back to the client.
    	char* servicefifo = (char *)malloc(sizeof(char)* 100);
    	strcpy(servicefifo, service);
    	strcat(servicefifo, "fifo|\n");

    	

		// sending the service fifo back to the client
    	int server_to_client = open(client_fifo, O_WRONLY);
    	int w = write(server_to_client, servicefifo, sizeof(servicefifo));
    	print_error(w, "write to fifo failed");
		fflush(stdout);



    }
    else{
    	printf("Service does not exist\n");
    }

	memset(input, 0, sizeof(input));
}

void* register_clients(void * arg){
	int fd = server_init();
	printf("Server ON.FD is %d\n", fd);

	while(1){

		int r = read(fd, buf, PIPE_BUF);
		print_error(r, "read failed");
		process_input(buf);


	}
	close(fd);
	unlink(fifo);
	pthread_exit(0);
}


int main(){
	pthread_t reg;
	pthread_create(&reg, NULL, register_clients, (void*)0);

	pthread_join(reg,NULL);
	



	
	



}