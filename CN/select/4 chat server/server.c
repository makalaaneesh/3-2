#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <limits.h>
#include <poll.h>

#include <stropts.h>

#define MAX_CLIENTS 10
char *fifo = "server";
char *buf;

int client_fd_index;
int client_fifos_fds[MAX_CLIENTS];

int client_read_fds[MAX_CLIENTS];

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
    	fflush(stdout);
		token = strtok(NULL, "|");
		char * client_pid = (char *)malloc(sizeof(token));
		strcpy(client_pid,token);
		// client_pid[strlen(client_pid)-1] = '\0';

    	client_fifos_fds[client_fd_index++] = open(client_pid, O_WRONLY);
    	printf("New client added %s\n", client_pid);
    	fflush(stdout);
    	return;
    }	

    printf("%s", input);
	int i;
	for(i = 0; i<client_fd_index; i++){
		// printf("writing to fifo id%d\n", client_fifos_fds[i]);
		int w = write(client_fifos_fds[i], input, PIPE_BUF); //writing to each client
		// fflush(stdout);
		print_error(w, "write to fifo failed");
	}
	memset(input, 0, sizeof(input));
}


int main(int argc, char *argv[]){

	int no_of_clients = argc -1;
	char **client_fifos = argv+1;

	int i;
	for(i = 0; i<no_of_clients; i++){
		mkfifo(client_fifos[i], 0666);
		client_read_fds[i] = open(client_fifos[i], O_RDWR);
	}

	

	int fd = server_init();
	printf("Server ON.FD is %d\n", fd);


// POLL
	// int timeout = 20;
	// int ret;
	// struct pollfd fds[no_of_clients];
	// for(i= 0; i<no_of_clients; i++){
	// 	fds[i].fd = client_read_fds[i];
	// 	fds[i].events = POLLRDNORM;

	// }
	// // char *buf = (char *)malloc(sizeof(char)* 100);
	// printf("Polling services\n");
	// while(1){
	// 	// printf("Polling\n");
	// 	ret = poll(fds, no_of_clients, timeout);
	// 	if(ret > 0){
	// 		printf("Event occured\n");
	// 		for(i= 0; i<no_of_clients; i++){
	// 			if(fds[i].revents & POLLRDNORM){

	// 				int r = read(fds[i].fd, buf, PIPE_BUF);
	// 				print_error(r,"read failed");
	// 				process_input(buf);
	// 			}
	// 		}
	// 	}
	// }

// POLL END

// SELECT
	int ret;
	char *main_buf = (char *)malloc(sizeof(char)* PIPE_BUF);
	int max_fd = 0;
	// int i;
	for(i=0; i<no_of_clients;i++){
		if(client_read_fds[i] >= max_fd)
			max_fd = client_read_fds[i];

	}
	max_fd += 1; //the max fd that we give to select statement has to be 1 more than the maximum fd among the entire list.

	printf("Max fd is %d\n", max_fd);
	// int ret;
	struct timeval tv;
	tv.tv_usec = 0;
	tv.tv_sec = 0;
	while(1){
		fd_set rfds;
		FD_ZERO(&rfds);
		for(i = 0; i<no_of_clients;i++){
			FD_SET(client_read_fds[i], &rfds);
		}
		ret = select(max_fd , &rfds, NULL, NULL, &tv);
		print_error(ret, "Select statement failed");
		for(i= 0; i<no_of_clients; i++){
			
			if(FD_ISSET(client_read_fds[i], &rfds)){
				// printf("Reading from %d\n",all_fds[i] );
				int r = read(client_read_fds[i], main_buf, PIPE_BUF);
				main_buf[strlen(main_buf)]='\0';
				print_error(r,"read failed");
				process_input(main_buf);
				// if(strlen(main_buf) >0)
					// printf("log:%s\n", main_buf);
					// fflush(stdout);
				// memset(main_buf, 0, strlen(main_buf));
				FD_CLR(client_read_fds[i], &rfds);

			}


		}
	}


// SELECT END





	close(fd);
	unlink(fifo);
	



}