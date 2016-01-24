#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <limits.h>

#define MAX_CLIENTS 10
char *fifo = "server";
char *buf;

int client_fd_index;
int client_fifos_fds[MAX_CLIENTS];

print_error(int val, char* msg){
	if (val < 0){
		printf("%s error code = %d\n", msg, val);
		exit(1);
	}
}


int server_init(){
	buf = (char *)malloc(sizeof(char)*PIPE_BUF);
	mkfifo(fifo, 0666);
	int fd = open(fifo, O_RDWR); //read write so that the fifo does not get closed when there are no clients
	print_error(fd, "failed to open fifo");
	client_fd_index = 0;
	return fd;
}


void process_input(char *input){
	char *stringp = (char *)malloc(sizeof(input));
	strcpy(stringp, input);
    const char *delim = " ";
    char *token;
 
    //stringp is updated to point to the next token 
    token = strtok(stringp, delim); 
    // printf("%s\n", token);
    if(strcmp(token, "init") == 0 && stringp != NULL){
    	printf("Adding new client\n");
		token = strtok(NULL, delim);
		char * client_pid = token;

    	client_fifos_fds[client_fd_index++] = open(client_pid, O_WRONLY);
    	printf("New client added %s", client_pid);
    	return;
    }	

    printf("%s", input);
	// int i;
	// for(i = 0; i<client_fd_index; i++){
	// 	int w = write(client_fifos_fds[i], buf, sizeof(buf)); //writing to each client
	// 	print_error(w, "write to fifo failed");
	// }
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