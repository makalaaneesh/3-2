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





int pno;

void print_error(int val, char*msg){
	if(val < 0){
		perror(msg);
		exit(val);
	}
}

void * reading_thread(void * arg){
	char *read_buf;
	read_buf = (char *)malloc(sizeof(char)*100);
	while(1){

		int r = read(0, read_buf, 100);
		print_error(r, "read failed");
		if (r >0)
			printf("[%d]Announcement ->%s\n", pno,read_buf);

		fflush(stdout);
		memset(read_buf, 0, strlen(read_buf));

	}

}



int main(int argc, char *argv[]){
	pthread_t t;
	pthread_create(&t, NULL, reading_thread, (void*)0);

	int nsfd, sfd;

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

    pno = atoi(argv[1]);
    int port = atoi(argv[2]);

	
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(port);
	//Computer networks are big endian. This means that when little endian computers are going to pass integers over the network (IP addresses for example), they need to convert them to network byte order. Likewise, when the receive integer values over the network, they need to convert them back to their own native representation.
	server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

	//binding the socket to the addr
	int b = bind(sfd, (struct sockaddr *)&server_addr, sizeof(server_addr));
	print_error(b, "Failed to bind.");

	listen(sfd, 3);

	printf("Platform %d is listening for connections.\n", pno);
	fflush(stdout);

	//accept connection from incoming client.
	while(1){

		nsfd = accept(sfd,(struct sockaddr * )&client_addr, &client_addr_len );
		print_error(nsfd, "Failed in accepting connection");
		printf("[%d]Accepted connection from %d\n",pno, inet_ntoa(client_addr.sin_addr));

		char *readbuffer, *writebuffer;
		readbuffer = (char *)malloc(sizeof(char)*100);
		writebuffer = (char *)malloc(sizeof(char)*100);
		//receive info from train directly until the train sends an "exit" message indicating that it is leaving.
		while(1){
			int r = recv(nsfd, readbuffer, 100 ,0);
			print_error(r, "Failed to receive a message from the client");
			printf("[%d](%s)\n",pno, readbuffer);
			if (strcmp(readbuffer, "exit") == 0){
				close(nsfd);
				//signal that platform is now free!!

				break;
			}
			memset(readbuffer, 0, strlen(readbuffer));
		}

	}
	

	printf("Clients disconnected\n");
	fflush(stdout);
	return 0;



}
	
