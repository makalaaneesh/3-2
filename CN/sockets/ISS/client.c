//http://www.linuxhowtos.org/C_C++/socket.htm
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <signal.h>
#define LOWER_PORT 9998
#define UPPER_PORT 9997
#define ECHO_PORT 9999

/*
 
struct sockaddr_in
{
  short   sin_family; //must be AF_INET 
  u_short sin_port;
  struct  in_addr sin_addr;
  char    sin_zero[8]; // Not used, must be zero 
};

*/

int sfd; // socket fd




void print_error(int val, char*msg){
	if(val < 0){
		perror(msg);
		exit(val);
	}
}

int main(int argc, char *argv[]){


	int port; //port on which server runs
	if (argc <= 1){
		printf("service required(lower/upper/return)\n");
		exit(1);
	}


	int client_addr_len; // var to store len of the address of client. 
	char *buffer, buffer1[256];
	char *service = argv[1];
	if(strcmp(service,"upper") == 0){
		port = UPPER_PORT;
	}
	else if(strcmp(service,"lower") == 0){
		port = LOWER_PORT;
	}
	else{
		port = ECHO_PORT;
	}
	buffer = (char *)malloc(sizeof(char)*100);

	struct sockaddr_in server_addr;

	// creating a socket
	sfd = socket(AF_INET, SOCK_STREAM, 0);
	print_error(sfd, "error opening socket");




	// port = atoi(argv[1]);
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(port);
	//Computer networks are big endian. This means that when little endian computers are going to pass integers over the network (IP addresses for example), they need to convert them to network byte order. Likewise, when the receive integer values over the network, they need to convert them back to their own native representation.
	server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

	// establishing the connection to server.
	int c = connect(sfd, (struct sockaddr *)&server_addr, sizeof(server_addr));
	print_error(c, "could not connect to server.");

	printf("%s\n", "connected to server");


	// reading from stdin and sending to server.
	size_t write_size;
	int read_size;

	// int s = send(sfd, service, sizeof(service), 0);
	// print_error(s, "Failed to send initial message to server");
	// int r = recv(sfd, buffer1, 256, 0);
	// print_error(r, "Failed to receive a reply from the server");
	// printf("Server:%s\n", buffer1);
	// memset(buffer, 0, 256);
	// memset(buffer1, 0, 256);

	while(1){
		printf("Enter word to convert \n");
		ssize_t in  = getline(&buffer, &write_size, stdin);
		// fgets(buffer,256,stdin);
		// gets(buffer);
		// scanf("%s",buffer);
		print_error(in, "Failed to read from stdin");
		if(strcmp(buffer, "exit") == 0)
			exit(1);

		// printf("Sending:[%s]\n", buffer);
		fflush(stdout);
		int s = send(sfd, buffer, strlen(buffer), 0);
		print_error(s, "Failed to send message to server");

		//receive a reply from the server
		// int r = recv(sfd, buffer1, 256,0);
		// print_error(r, "Failed to receive a reply from the server");
		// printf("Server:%s", buffer1);
		memset(buffer, 0, 256);
		memset(buffer1, 0, 256);
	}


	printf("Client disconnected\n");
	fflush(stdout);
	return 0;

}