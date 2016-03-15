//http://www.linuxhowtos.org/C_C++/socket.htm
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <signal.h>
#include <poll.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <limits.h>
#include <errno.h>
#include <error.h>

#include <stropts.h>
#include "../../../helper.h"

#define PORTS 3
#define PLATFORMS 2
#define SHMTOKEN 123123

int *platform_pid;


struct train{
	int train_no;
	int port;
	int sfd;
};

struct platform{
	int pno;//platform no
	int is_free;
	int pid;
	int port;
	int pfd;
};
void print_error(int val, char*msg){
	if(val < 0){
		perror(msg);
		exit(val);
	}
}
struct platform platforms[2] = { {1,1,0,10001,0}, {2,1,0,10002,0} };
struct train trains[PORTS] = { {1, 9997,0}, {2, 9998,0}, {3, 9999,0} };// corresponding to V, D, H


void train_leaving_signal(int signo){
	printf("Train has left\n");
	int plat_pid = *platform_pid;
	int index = -1;
	int i;
	for(i = 0; i< PLATFORMS; i++){
		if (plat_pid == platforms[i].pid){
			index = i;
			break;
		}
	}
	print_error(index, "No such platform exists!!!");
	printf("Train leaving from platform %d\n", platforms[index].pno);
	platforms[index].is_free = 1;
}


void init_platforms(){
	if(signal(SIGUSR1, train_leaving_signal) == SIG_ERR){
		printf("%s\n", "Error in catching SIGUSR1");
  	}
	int shmid;
	shmid = allocateSharedMemory(sizeof(int), SHMTOKEN);
   	platform_pid = (int *) mapSharedMemory(shmid);
	int i;
	for (i = 0; i< PLATFORMS; i++){
		int pipefd[2];
		pipe(pipefd);
		int pid = fork();
		print_error(pid, "Faield to fork");
		if (pid == 0){
			close(pipefd[1]);
			char *port_arg = (char*)malloc(sizeof(char)*20);
			sprintf(port_arg,"%d",platforms[i].port);
			char *pno = (char*)malloc(sizeof(char)*20);
			sprintf(pno, "%d", platforms[i].pno);
			dup2(pipefd[0],0);
			execl("platform.out", "platform.out", pno, port_arg, (char*)0);
		}
		else{
			close(pipefd[0]);
			platforms[i].pid = pid;
			platforms[i].pfd = pipefd[1];
		}
	}
}

void init(){
	struct sockaddr_in server_addr;
	int i; 
 	for( i = 0; i<PORTS; i++ ){
 		trains[i].sfd = socket(AF_INET, SOCK_STREAM, 0);
		print_error(trains[i].sfd, "error opening socket");
		//setting resuseaddr so that we don't have to constantly wait to the socket to timeout out of it's TIMEWAIT state.
		int set = setsockopt(trains[i].sfd, SOL_SOCKET, SO_REUSEADDR, &(int){ 1 }, sizeof(int));
		print_error(set, "setsockopt(SO_REUSEADDR) failed");


		server_addr.sin_family = AF_INET;
		server_addr.sin_port = htons(trains[i].port);
		server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

		int b = bind(trains[i].sfd, (struct sockaddr *)&server_addr, sizeof(server_addr));
		print_error(b, "Failed to bind.");
		listen(trains[i].sfd, 3);	
		printf("listening %d at port %d at socket fd %d\n", trains[i].train_no, trains[i].port, trains[i].sfd);
		fflush(stdout);

 	}
 	printf("%s\n", "Server is initiated.");
}
void wait_for_incoming_connections(){
	int ret;
	char *main_buf = (char *)malloc(sizeof(char)* PIPE_BUF);
	int max_fd = 0;
	struct sockaddr_in client_addr;
	int client_addr_len;
	int i;
	// for(i=0; i<PORTS;i++){
	// 	if(trains[i].sfd >= max_fd)
	// 		max_fd = trains[i].sfd;

	// }
	max_fd += 1; //the max fd that we give to select statement has to be 1 more than the maximum fd among the entire list.

	printf("Max fd is %d\n", max_fd);

	char buffer[256];
	struct timeval tv;
	tv.tv_usec = 0;
	tv.tv_sec = 0;
	int nsfd;
	int poll_max = PORTS;
	int timeout = 0;
	struct pollfd fds[PORTS];
	for(i = 0; i< PORTS; i++){
		fds[i].fd = trains[i].sfd;
		fds[i].events = POLLRDNORM;

	}
	while(1){
		// fd_set rfds;
		// FD_ZERO(&rfds);
		// for(i = 0; i<PORTS;i++){
		// 	FD_SET(trains[i].sfd, &rfds);
		// }
		// ret = select(max_fd , &rfds, NULL, NULL, NULL);
		ret = poll(fds, poll_max, timeout);
		if ( errno  == EINTR ){
			printf("interrupted. Continuing.\n");
			errno = 0;
			continue;
		}
		print_error(ret, "Poll statement failed!");
		for(i= 0; i<PORTS; i++){
			
			// if(FD_ISSET(trains[i].sfd, &rfds)){
			if ((fds[i].revents & POLLRDNORM)){
				// printf("About to accept connection %d\n",i);
				//accept a call if a free platform is available;
				int free_platform_index = -1;
				int x;
				for(x = 0; x< PLATFORMS; x++){
					if (platforms[x].is_free == 1){
						free_platform_index = x;
						break;
					}
				}
				if (free_platform_index == -1){
					// printf("No free platforms available\n");
					continue;
				}
				// free platform is available. accept.
				printf("Ready to accept at %d\n", trains[i].port);
				platforms[free_platform_index].is_free = 0;

				nsfd = accept(trains[i].sfd,(struct sockaddr * )&client_addr, &client_addr_len );
				print_error(nsfd, "Failed in accepting connection");
				printf("Accepted connection from %d\n", inet_ntoa(client_addr.sin_addr));


				//sending announcement via all the platforms pfds.
				char *msg = (char *)malloc(sizeof(char)* PIPE_BUF);
				sprintf(msg, "Train %d has arrived!!!", trains[i].train_no);
				for (i = 0; i< PLATFORMS; i++){
					int w = write(platforms[i].pfd, msg, strlen(msg)); //writing to server
					print_error(w, "write to platofrom pipes failed");
				}


				//send the free platform's listening port back to the train.
				sprintf(msg, "%d", platforms[free_platform_index].port);
				int s = send(nsfd, msg, strlen(msg), 0);
				print_error(s, "Failed to send free platform port back to the train");

				// FD_CLR(trains[i].sfd, &rfds);
				// close(nsfd);

			}


		}
	}

}

int main(){
	init();
	init_platforms();
	wait_for_incoming_connections();
}