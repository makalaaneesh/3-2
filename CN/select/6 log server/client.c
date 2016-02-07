#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <limits.h>

char *serverfifo = "server";
int client_to_server;
char myfifo[10];
int client_reply_fd;
int service_fifo_fd;
char *write_buf = NULL;
char *read_buf = NULL;


print_error(int val, char* msg){
	if (val < 0){
		printf("%s error code = %d\n", msg, val);
		exit(1);
	}
}

// reads the reply from the server. gets the service fifo to connect to. opens the fifo and stores the fd in service_fifo_fd
void get_service_fifo(){
	char *read_buf ;
	read_buf =(char *)malloc(sizeof(char)*PIPE_BUF);
	read(client_reply_fd ,read_buf, PIPE_BUF);
	char *service_fifo = strtok(read_buf, "|");
	printf("Server fifo received. %s\n", service_fifo);
	fflush(stdout);
	service_fifo_fd = open(service_fifo, O_WRONLY);
	print_error(service_fifo_fd, "could not open service fifo");
}

void service_exec(){
	char *write_buf ;
	char *read_buf ;
	size_t write_bufsize;
	write_buf = (char *)malloc(sizeof(char)*PIPE_BUF);
	read_buf =(char *)malloc(sizeof(char)*PIPE_BUF);

	get_service_fifo();


	while(1){


		// read input from stdin
		ssize_t in  = getline(&write_buf, &write_bufsize, stdin);
		// gets(write_buf);
		print_error(in, "failed to get input");
		// char test[PIPE_BUF];
		// fgets(test, PIPE_BUF, stdin);

		if(strcmp(write_buf, "exit") == 0){
			printf("Quitting.\n");
			exit(1);
		}
		//write to service via fifo
		

		char msg[PIPE_BUF];
		snprintf(msg, sizeof(msg), "%s\n",write_buf);
		printf("Writing %s[]", msg);
		fflush(stdout);
		int w = write(service_fifo_fd, msg, strlen(msg)); 

		fflush(stdout);

		
		// int w = write(service_fifo_fd, write_buf, PIPE_BUF); //writing to server
		// write_buf = NULL;
		print_error(w, "write to fifo failed");
		fflush(stdout);

	}
}

void register_for_service(char * service){
	char msg[PIPE_BUF];
	snprintf(msg, sizeof(msg), "%s|%s|",service, myfifo);
	int w = write(client_to_server, msg, sizeof(msg)); //writing to server
	print_error(w, "write to fifo failed");
	fflush(stdout);
	service_exec();

}

void client_init(){	

	int pid = getpid();
	sprintf(myfifo, "%d", pid);
	// username = (char*)malloc(sizeof(char)*10);
	// gets(username);

	client_to_server = open(serverfifo, O_WRONLY);
	//init to server
	
	// opening fifo that the server will use to reply to the client
	mkfifo(myfifo, 0666);
	client_reply_fd = open(myfifo, O_RDWR);
	read_buf = (char *)malloc(sizeof(char)*PIPE_BUF);
	printf("myfifo id is%d\n", service_fifo_fd);

}

int main(int argc, char *argv[]){
	client_init();
	char* service = argv[1];
	register_for_service(service);
}