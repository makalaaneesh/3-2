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

#include <stropts.h>
#define STDIN 0
#define STDOUT 1
#define MAX_SERVICES 3
#define FAMOUS_PORT 10000


/*

struct sockaddr_in
{
  short   sin_family; //must be AF_INET 
  u_short sin_port;
  struct  in_addr sin_addr;
  char    sin_zero[8]; // Not used, must be zero 
};

*/


int sfd;

struct service{
	char *name;
	int port;
	int sfd;
	int started;
}services[MAX_SERVICES];


int get_service_index(char * service_name){
	int i;
	for(i = 0; i< MAX_SERVICES; i++){
		if(strcmp(services[i].name, service_name) == 0){
			return i;
		}
	}
	return -1;
}


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

void server_init(){
	services[0].port = 9997;
	services[1].port = 9998;
	services[2].port = 9999;
	services[0].name = (char*)malloc(sizeof(char)*20);
	services[1].name = (char*)malloc(sizeof(char)*20);
	services[2].name = (char*)malloc(sizeof(char)*20);
	strcpy(services[0].name, "group1");
	strcpy(services[1].name, "group2");
	strcpy(services[2].name, "group3");
	services[0].started = 0;
	services[1].started = 0;
	services[2].started = 0;

	// signal (ctrl c) to clean up.
	if(signal(SIGINT, clean) == SIG_ERR){
  		printf("%s\n", "Error in catching SIGINT");
 	}




	struct sockaddr_in server_addr;


	sfd = socket(AF_INET, SOCK_STREAM, 0);
	print_error(sfd, "error opening socket");
	//setting resuseaddr so that we don't have to constantly wait to the socket to timeout out of it's TIMEWAIT state.
	int set = setsockopt(sfd, SOL_SOCKET, SO_REUSEADDR, &(int){ 1 }, sizeof(int));
	print_error(set, "setsockopt(SO_REUSEADDR) failed");


	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(FAMOUS_PORT);
	server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

	int b = bind(sfd, (struct sockaddr *)&server_addr, sizeof(server_addr));
	print_error(b, "Failed to bind.");
	listen(sfd, 3);	
	printf("listening at port %d at socket fd %d\n", FAMOUS_PORT, sfd);
	fflush(stdout);

 	
 	printf("%s\n", "Server is initiated.");
 	


}


void *wait_for_incoming_connections(void * arg){
	int ret;
	char *main_buf = (char *)malloc(sizeof(char)* PIPE_BUF);
	int max_fd = 0;
	struct sockaddr_in client_addr;
	int client_addr_len;
	int i;
	max_fd  = sfd+1; //the max fd that we give to select statement has to be 1 more than the maximum fd among the entire list.

	printf("Max fd is %d\n", max_fd);

	char buffer[256];
	struct timeval tv;
	tv.tv_usec = 0;
	tv.tv_sec = 0;
	int nsfd;
	char *readbuffer, *writebuffer;
	readbuffer = (char *)malloc(sizeof(char)*100);
	writebuffer = (char *)malloc(sizeof(char)*100);
	while(1){
		fd_set rfds;
		FD_ZERO(&rfds);
		FD_SET(sfd, &rfds);
		ret = select(max_fd , &rfds, NULL, NULL, &tv);
		print_error(ret, "Select statement failed at accept");
			
		if(FD_ISSET(sfd, &rfds)){
			//accept a call
			nsfd = accept(sfd,(struct sockaddr * )&client_addr, &client_addr_len );
			print_error(nsfd, "Failed in accepting connectionn");
			printf("Accepted connection from %d\n", inet_ntoa(client_addr.sin_addr));

			
			int r = recv(nsfd, readbuffer, 100 ,0);
			print_error(r, "Failed to receive initial message from the client!");
			printf("[%s]\n", readbuffer);
			int index = get_service_index(readbuffer);
			printf("index is %d\n", index);
			memset(readbuffer, 0, strlen(readbuffer));
			printf("Group chat  requested is %s", services[index].name);
			fflush(stdout);
			if(services[index].started == 0){

				int pid = fork();
				print_error(pid, "Faield to fork");
				if (pid == 0){
					char *port_arg = (char*)malloc(sizeof(char)*20);
					sprintf(port_arg,"%d",services[index].port);
					fflush(stdout);
					execl(services[index].name, services[index].name, port_arg, (char*)0);
				}
				else{
					services[index].started = 1;
				}
				
			}
			printf("exec done\n");
			fflush(stdout);
			// //else the service will handle it directly

			sprintf(writebuffer, "%d", services[index].port);
			int s = send(nsfd, writebuffer, strlen(writebuffer), 0);
			print_error(s, "Failed to send message to the client.");



		}
		
	}

}




int main(int argc, char *argv[]){
	
	server_init();
	pthread_t services, clients;
	pthread_create(&clients, NULL, wait_for_incoming_connections, (void*)0);


	pthread_join(clients);

	printf("Exiting\n");
	fflush(stdout);
	return 0;

}