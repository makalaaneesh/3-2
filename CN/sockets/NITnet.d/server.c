//http://www.linuxhowtos.org/C_C++/socket.htm
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <signal.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <limits.h>
#include"../../helper.h"

#include <stropts.h>
#define STDIN 0
#define STDOUT 1
#define MAX_SERVICES 3
#define SHMTOKEN 123123

/*

struct sockaddr_in
{
  short   sin_family; //must be AF_INET 
  u_short sin_port;
  struct  in_addr sin_addr;
  char    sin_zero[8]; // Not used, must be zero 
};

*/

struct service{
	char *name;
	int port;
	int sfd;

}services[MAX_SERVICES];

// int sfd; // socket fd
// int nsfd; //new fd when accept is called
int service_read_fds[20];
int service_read_fds_index = 0;


void clean(int signo){
	printf("Shutting down server.\n");
	// shutdown(nsfd, SHUT_RDWR);
	// shutdown(sfd, SHUT_RDWR);
	exit(1);
}

void print_error(int val, char*msg){
	if(val < 0){
		perror(msg);
		exit(val);
	}
}

struct service_client_info
{
	char name[10];
	int service_port;
	int max_clients;
	int current_clients;
}**shm;

void server_init(){
	int shmid;
	shmid = allocateSharedMemory(sizeof(struct service_client_info*)*MAX_SERVICES, SHMTOKEN);
	shm = (struct service_client_info **) mapSharedMemory(shmid);
	shm[0] = (struct service_client_info*)malloc(sizeof(struct service_client_info));
	shm[1] = (struct service_client_info*)malloc(sizeof(struct service_client_info));
	shm[2] = (struct service_client_info*)malloc(sizeof(struct service_client_info));

	services[0].port = 9997;
	services[1].port = 9998;
	services[2].port = 9999;
	services[0].name = (char*)malloc(sizeof(char)*20);
	services[1].name = (char*)malloc(sizeof(char)*20);
	services[2].name = (char*)malloc(sizeof(char)*20);
	strcpy(services[0].name, "upper");
	strcpy(services[1].name, "lower");
	strcpy(services[2].name, "echo");
	int i;
	for (i = 0; i<3; i++){
		strcpy(shm[i]->name, services[i].name);
		shm[i]->service_port = services[i].port;
		shm[i]->max_clients = 3;
		shm[i]->current_clients = 0;
	}
	// services[0].name = "upper";
	// services[1].name = "lower";
	// services[2].name = "return";
	// for(i =0; i<3; i++){
	// 	printf("%s%d\n", "port is ",shm[i]->service_port);
	// }
	// signal (ctrl c) to clean up.
	if(signal(SIGINT, clean) == SIG_ERR){
  		printf("%s\n", "Error in catching SIGINT");
 	}


	struct sockaddr_in server_addr;

 	for( i = 0; i<MAX_SERVICES; i++ ){
 		services[i].sfd = socket(AF_INET, SOCK_STREAM, 0);
		print_error(services[i].sfd, "error opening socket");
		//setting resuseaddr so that we don't have to constantly wait to the socket to timeout out of it's TIMEWAIT state.
		int set = setsockopt(services[i].sfd, SOL_SOCKET, SO_REUSEADDR, &(int){ 1 }, sizeof(int));
		print_error(set, "setsockopt(SO_REUSEADDR) failed");


		server_addr.sin_family = AF_INET;
		server_addr.sin_port = htons(services[i].port);
		server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

		int b = bind(services[i].sfd, (struct sockaddr *)&server_addr, sizeof(server_addr));
		print_error(b, "Failed to bind.");
		listen(services[i].sfd, 3);	
		printf("listening %s at port %d at socket fd %d\n", services[i].name, services[i].port, services[i].sfd);
		fflush(stdout);

 	}
 	printf("%s\n", "Server is initiated.");
 	


}

void print_read_fds(){
	int i;
	for(i = 0; i< service_read_fds_index; i++){
		printf("%d,,", service_read_fds[i]);

	}
	printf("\n");
	fflush(stdout);
}

void *wait_for_incoming_connections(void * arg){
	int ret;
	char *main_buf = (char *)malloc(sizeof(char)* PIPE_BUF);
	int max_fd = 0;
	struct sockaddr_in client_addr;
	int client_addr_len;
	int i;
	for(i=0; i<MAX_SERVICES;i++){
		if(services[i].sfd >= max_fd)
			max_fd = services[i].sfd;

	}
	max_fd += 1; //the max fd that we give to select statement has to be 1 more than the maximum fd among the entire list.

	printf("Max fd is %d\n", max_fd);

	char buffer[256];
	struct timeval tv;
	tv.tv_usec = 0;
	tv.tv_sec = 0;
	int nsfd;
	while(1){
		fd_set rfds;
		FD_ZERO(&rfds);
		for(i = 0; i<MAX_SERVICES;i++){
			FD_SET(services[i].sfd, &rfds);
		}
		ret = select(max_fd , &rfds, NULL, NULL, &tv);
		print_error(ret, "Select statement failed at accept");
		for(i= 0; i<MAX_SERVICES; i++){
			
			if(FD_ISSET(services[i].sfd, &rfds)){
				// printf("%s service max-%d, current-%d\n", services[i].name, shm[i]->max_clients, shm[i]->current_clients);
				if (shm[i]->current_clients >= shm[i]->max_clients){
					continue;
				}
				//accept a call
				nsfd = accept(services[i].sfd,(struct sockaddr * )&client_addr, &client_addr_len );
				print_error(nsfd, "Failed in accepting connection");
				printf("Accepted connection from %d\n", inet_ntoa(client_addr.sin_addr));

				
				int pfd[2];
				pipe(pfd);

				int pid = fork();
				print_error(pid, "fork failed");

				if (pid == 0){//child
					close(pfd[0]); //closing the reading end
					// int r = recv(nsfd, buffer, 256, 0);
					// print_error(r, "Read failed");
					close(services[i].sfd);
					// printf("1-%d\n", nsfd);
					if(nsfd != STDIN){
						printf("Duping input\n");
						if(dup2(nsfd, STDIN)!= STDIN){
							print_error(-1, "Failed to dup2 stdin");
						}

					}
					// printf("2-%d\n", nsfd);
					if(pfd[1] != STDOUT){
						printf("Duping output\n");
						if(dup2(pfd[1], STDOUT)!= STDOUT){
							print_error(-1, "Failed to dup2 stdout");
						}

					}
					close(nsfd);
					// printf("3-%d\n", nsfd);
					// fflush(stdout);
					// close(nsfd);
					// printf("Starting service %s\n", buffer);
					// fflush(stdout);
					
					execl(services[i].name, services[i].name, (char*)0);
					// int read_size;
					// while( (read_size = recv(nsfd, buffer, 256, 0)) > 0){
					// 	printf("Message:%s\n", buffer);
					// 	int s = send(nsfd, buffer, sizeof(buffer), 0);
					// 	print_error(s, "Failed to send message to client");
					// 	memset(buffer, 0, 256);
					// }
		                   

				}
				else{
					
					close(nsfd);
					close(pfd[1]); //closing the writing end
					service_read_fds[service_read_fds_index++] = pfd[0];
					shm[i]->current_clients++;
					print_read_fds();

				}

				// FD_CLR(services[i].sfd, &rfds);

			}


		}
	}

}


void *read_from_services(void *arg){
	int ret;
	char *main_buf = (char *)malloc(sizeof(char)* PIPE_BUF);
	int max_fd = 0;
	int i;
	for(i=0; i<service_read_fds_index;i++){
		if(service_read_fds[i] >= max_fd)
			max_fd = service_read_fds[i];

	}
	max_fd += 1; //the max fd that we give to select statement has to be 1 more than the maximum fd among the entire list.

	printf("Max fd is %d\n", max_fd);
	struct timeval tv;
	tv.tv_usec = 0;
	tv.tv_sec = 0;
	while(service_read_fds_index == 0);
	printf("%s\n", "reading from services");

	while(1){
		fd_set rfds;
		FD_ZERO(&rfds);
		for(i = 0; i<service_read_fds_index;i++){
			FD_SET(service_read_fds[i], &rfds);
		}
		for(i=0; i<service_read_fds_index;i++){
			if(service_read_fds[i] >= max_fd)
					max_fd = service_read_fds[i];

		}
		max_fd += 1;
		// printf("max fd at the reading end is %d\n", max_fd);
		ret = select(max_fd , &rfds, NULL, NULL, &tv);
		print_error(ret, "Select statement failed at reading from fds.");
		for(i= 0; i<service_read_fds_index; i++){
			
			if(FD_ISSET(service_read_fds[i], &rfds)){
				// printf("Reading from %d\n",all_fds[i] );
				int r = read(service_read_fds[i], main_buf, PIPE_BUF);
				main_buf[strlen(main_buf)]='\0';
				print_error(r,"read failed");
				if(strlen(main_buf) >0)
					printf("log:%s\n", main_buf);
					fflush(stdout);
				memset(main_buf, 0, strlen(main_buf));
				FD_CLR(service_read_fds[i], &rfds);

			}


		}
	}
}

int main(int argc, char *argv[]){
	
	server_init();
	pthread_t services, clients;
	pthread_create(&clients, NULL, wait_for_incoming_connections, (void*)0);
	pthread_create(&services, NULL, read_from_services, (void*)0);


	pthread_join(services);
	pthread_join(clients);

	printf("Exiting\n");
	fflush(stdout);
	return 0;

}