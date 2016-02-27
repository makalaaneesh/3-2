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


#define MAX_CLIENTS 2
int current_no_of_clients = 0;
pthread_t threads[MAX_CLIENTS];

#define PORT 9997

void print_error(int val, char*msg){
	if(val < 0){
		perror(msg);
		exit(val);
	}
}



void *run(void * arg){
	int sfd = *((int *)arg);
	printf("Thread sfd is %d\n", sfd);
	char *readbuffer, *writebuffer;
	readbuffer = (char *)malloc(sizeof(char)*100);
	writebuffer = (char *)malloc(sizeof(char)*100);

	while(1){
		
		int r = recv(sfd, readbuffer, 100 ,0);
		print_error(r, "Failed to receive a message from the client");
		int i;
		for(i = 0; readbuffer[i]!='\0'; i++){
			readbuffer[i] = toupper(readbuffer[i]);
		}
		int s = send(sfd, readbuffer, strlen(readbuffer), 0);
		print_error(s, "Failed to send message to the client.");

		memset(readbuffer, 0, strlen(readbuffer));
	}



}


int main(){
	int nsfd, sfd;
	int port; //port on which server runs
	int client_addr_len; // var to store len of the address of client. 
	char *readbuffer, *writebuffer;
	readbuffer = (char *)malloc(sizeof(char)*100);
	writebuffer = (char *)malloc(sizeof(char)*100);

	struct sockaddr_in server_addr, client_addr;
	// creating a socket
	sfd = socket(AF_INET, SOCK_STREAM, 0);
	print_error(sfd, "error opening socket");

	//setting resuseaddr so that we don't have to constantly wait to the socket to timeout out of it's TIMEWAIT state.
	int set = setsockopt(sfd, SOL_SOCKET, SO_REUSEADDR, &(int){ 1 }, sizeof(int));
    	print_error(set, "setsockopt(SO_REUSEADDR) failed");


	
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(PORT);
	//Computer networks are big endian. This means that when little endian computers are going to pass integers over the network (IP addresses for example), they need to convert them to network byte order. Likewise, when the receive integer values over the network, they need to convert them back to their own native representation.
	server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

	//binding the socket to the addr
	int b = bind(sfd, (struct sockaddr *)&server_addr, sizeof(server_addr));
	print_error(b, "Failed to bind.");

	listen(sfd, 3);

	printf("%s\n", "Server is listening for connections.");

	//accept connection from incoming client.
	while(1){
		nsfd = accept(sfd,(struct sockaddr * )&client_addr, &client_addr_len );
		print_error(nsfd, "Failed in accepting connection");
		printf("Accepted connection from %d\n", inet_ntoa(client_addr.sin_addr));

		if (current_no_of_clients >= MAX_CLIENTS){
			printf("Disconnecting\n");
			strcpy(writebuffer, "Max client limit. Connect later.");
			// int s = send(sfd, writebuffer, strlen(writebuffer), 0);
			// print_error(s, "Failed to send message to the client.");
			memset(writebuffer, 0, strlen(writebuffer));
		}
		else{
			printf("Connectinf\n");
			strcpy(writebuffer, "Welcome to the lower service!");
			// int s = send(sfd, writebuffer, strlen(writebuffer), 0);
			// print_error(s, "Failed to send message to the client.");
			memset(writebuffer, 0, strlen(writebuffer));

			pthread_create(&(threads[current_no_of_clients++]), NULL, run, (void*)(&nsfd));
		}

	}
	

	printf("Clients disconnected\n");
	fflush(stdout);
	return 0;



}
	
