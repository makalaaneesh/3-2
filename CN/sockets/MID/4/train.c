//http://www.linuxhowtos.org/C_C++/socket.htm
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <signal.h>
// #define LOWER_PORT 9998
// #define UPPER_PORT 9997
// #define ECHO_PORT 9999
#define TRAIN_PORT 9998


/*
 
struct sockaddr_in
{
  short   sin_family; //must be AF_INET 
  u_short sin_port;
  struct  in_addr sin_addr;
  char    sin_zero[8]; // Not used, must be zero 
};

*/

void get_peer_ip(int sf){
	socklen_t len;
	struct sockaddr_storage addr;
	char ipstr[INET6_ADDRSTRLEN];
	int port;

	len = sizeof addr;
	getpeername(sf, (struct sockaddr*)&addr, &len);

	    struct sockaddr_in *s = (struct sockaddr_in *)&addr;
	    port = ntohs(s->sin_port);
	    inet_ntop(AF_INET, &s->sin_addr, ipstr, sizeof ipstr);

	// printf("Peer IP address: %s\n", ipstr);
	char ip_port[100];
	sprintf(ip_port, "Connected peer is ->>>>>> %s:%d", ipstr, port);
	printf("%s\n", ip_port);
}
void get_my_ip(int sf){
	socklen_t len;
	struct sockaddr_storage addr;
	char ipstr[INET6_ADDRSTRLEN];
	int port;

	len = sizeof addr;
	getsockname(sf, (struct sockaddr*)&addr, &len);

	    struct sockaddr_in *s = (struct sockaddr_in *)&addr;
	    port = ntohs(s->sin_port);
	    inet_ntop(AF_INET, &s->sin_addr, ipstr, sizeof ipstr);

	// printf("Peer IP address: %s\n", ipstr);
	char ip_port[100];
	sprintf(ip_port, "My socket address is ->>>>>> %s:%d", ipstr, port);
	printf("%s\n", ip_port);
}

int sfd; // socket fd
int service_port;
int service_sfd;




void print_error(int val, char*msg){
	if(val < 0){
		perror(msg);
		exit(val);
	}
}



int main(int argc, char *argv[]){


	int port; //port on which server runs
	port = TRAIN_PORT;
	printf("port is %d\n", port);
	// if (argc <= 1){
	// 	printf("group id required.\n");
	// 	exit(1);
	// }


	int client_addr_len; // var to store len of the address of client. 
	char *buffer, buffer1[256];
	char *service = argv[1]; // this will now be the group id to connect to OR start(Change variables later!)

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
	// get_my_ip(sfd);
	// get_peer_ip(sfd);
	print_error(c, "could not connect to station");

	printf("%s\n", "connected to station");


	// reading from stdin and sending to server.
	size_t write_size;
	int read_size;

	sleep(2);
	int s;
	printf("Waiting for reply from the station\n");
	fflush(stdout);
	// int s = send(sfd, service, sizeof(service), 0);
	// print_error(s, "Failed to send initial message to server");
	int r = recv(sfd, buffer1, 256, 0);
	print_error(r, "Failed to receive a reply from the server");
	printf("Station:[%s]\n", buffer1);
	fflush(stdout);

	int service_port = atoi(buffer1);
	if(service_port == 0){
		printf("Service does not exist. \n");
		exit(1);
	}


	memset(buffer, 0, strlen(buffer));
	memset(buffer1, 0, strlen(buffer1));

	close(sfd);
	sleep(5);
	int nsfd = socket(AF_INET, SOCK_STREAM, 0);
	print_error(nsfd, "error opening socket");

	server_addr.sin_port = htons(service_port);
	c = connect(nsfd, (struct sockaddr *)&server_addr, sizeof(server_addr));
	print_error(c, "could not connect to platform.");
	printf("%s\n", "Connected to platofrm");
	// service_sfd = nsfd;



	sprintf(buffer, "%s", "Compartment Details!");
	s = send(nsfd, buffer, strlen(buffer), 0);
	print_error(s, "Failed to send message to server");

	sleep(2);// train is arriving.

	// train is leaving
	sprintf(buffer, "%s", "exit");
	s = send(nsfd, buffer, strlen(buffer), 0);
	print_error(s, "Failed to send message to server");



	// while(1){
	// 	printf("Enter message to send to group \n");
	// 	ssize_t in  = getline(&buffer, &write_size, stdin);
	// 	// fgets(buffer,256,stdin);
	// 	// gets(buffer);
	// 	// scanf("%s",buffer);
	// 	print_error(in, "Failed to read from stdin");
	// 	if(strcmp(buffer, "exit") == 0)
	// 		exit(1);

	// 	// printf("Sending:[%s]\n", buffer);
	// 	fflush(stdout);
	// 	int s = send(nsfd, buffer, strlen(buffer), 0);
	// 	print_error(s, "Failed to send message to server");

	// 	//receive a reply from the server
	// 	// int r = recv(nsfd, buffer1, 256,0);
	// 	// print_error(r, "Failed to receive a reply from the server");
	// 	// printf("Server:%s", buffer1);
	// 	memset(buffer, 0, 256);
	// 	memset(buffer1, 0, 256);
	// }


	printf("Train leaving disconnected\n");
	fflush(stdout);
	sleep(5);
	close(nsfd);
	return 0;

}