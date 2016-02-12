#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <limits.h>

char *serverfifo;
int client_to_server;
char myfifo[10];
int service_fifo_fd;
char *write_buf = NULL;
char *read_buf = NULL;


print_error(int val, char* msg){
	if (val < 0){
		printf("%s error code = %d\n", msg, val);
		exit(1);
	}
}

void service_exec(){
	char *write_buf ;
	char *read_buf ;
	int write_bufsize;
	write_buf = (char *)malloc(sizeof(char)*PIPE_BUF);
	read_buf =(char *)malloc(sizeof(char)*PIPE_BUF);

	while(1){
		// read what service writes.
		read(service_fifo_fd, read_buf, PIPE_BUF);
		printf("%s\n", read_buf);
		memset(read_buf, 0, sizeof(read_buf));
		
/*
		// read input from stdin
		ssize_t in  = getline(&write_buf, &write_bufsize, stdin);
		print_error(in, "failed to get input");

		if(strcmp(write_buf, "exit") == 0){
			printf("Quitting.\n");
			exit(1);
		}
		//write back to service via fifo
		int w = write(service_fifo_fd, write_buf, sizeof(write_buf)); //writing to server
		print_error(w, "write to fifo failed");
		fflush(stdout);
*/
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
	service_fifo_fd = open(myfifo, O_RDWR);
	read_buf = (char *)malloc(sizeof(char)*PIPE_BUF);
	printf("myfifo id is%d\n", service_fifo_fd);

}

int main(int argc, char *argv[]){
	client_init();
	char* service = argv[1];
	serverfifo = argv[2];
	register_for_service(service);
}