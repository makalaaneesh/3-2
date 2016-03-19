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
#define SERVER_FAMOUS_PORT 9999


/*
 
struct sockaddr_in
{
  short   sin_family; //must be AF_INET 
  u_short sin_port;
  struct  in_addr sin_addr;
  char    sin_zero[8]; // Not used, must be zero 
};

*/
struct sockaddr_in* get_peer_ip(int sf){
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
	return s;
}
struct sockaddr_in* get_my_ip(int sf){
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
	return s;
}

void print_error(int val, char*msg){
	if(val < 0){
		perror(msg);
		exit(val);
	}
}


int sfd; // socket fd
int nsfd;
int my_port ;

void start_listening(){
	
	printf("Trying to open socket at %d\n", my_port);
	struct sockaddr_in server_addr, client_addr;
	int client_addr_len; // var to store len of the address of client. 
	char buffer[256];
	// shutdown(nsfd, 2);
	// shutdown(sfd,2);
	// close(nsfd);
	// close(sfd);
	// sleep(30);
	// sfd = socket(AF_INET, SOCK_STREAM, 0);
	// print_error(sfd, "error opening socket");

	// // setting resuseaddr so that we don't have to constantly wait to the socket to timeout out of it's TIMEWAIT state.
	// int set = setsockopt(sfd, SOL_SOCKET, SO_REUSEADDR, &(int){ 1 }, sizeof(int));
	// print_error(set, "setsockopt(SO_REUSEADDR) failed");


	int port = my_port;
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(my_port);
	// Computer networks are big endian. This means that when little endian computers are going to pass integers over the network (IP addresses for example), they need to convert them to network byte order. Likewise, when the receive integer values over the network, they need to convert them back to their own native representation.
	server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

	// binding the socket to the addr
	int b = bind(sfd, (struct sockaddr *)&server_addr, sizeof(server_addr));
	print_error(b, "Failed to bind.");

	listen(sfd, 3);
	printf("%s\n", "Server is listening for connections.");
	nsfd = accept(sfd,(struct sockaddr * )&client_addr, &client_addr_len );
	get_peer_ip(nsfd);
	print_error(nsfd, "Failed in accepting connection");
	printf("Accepted connection from %d\n", inet_ntoa(client_addr.sin_addr));

	nsfd = accept(sfd,(struct sockaddr * )&client_addr, &client_addr_len );
	get_peer_ip(nsfd);
	print_error(nsfd, "Failed in accepting connection");
	printf("Accepted connection from %d\n", inet_ntoa(client_addr.sin_addr));
}
void *reading_thread(void * arg){

	char * read_buf = (char *)malloc(sizeof(char)*100);
	printf("Reading thraed\n");
	fflush(stdout);
	while(1){
		int r = read(nsfd, read_buf, 100);
		
		print_error(r, "read failed");
		if (r >0)
			printf("Server->[%s]", read_buf);

		fflush(stdout);
		
		if (strcmp(read_buf,"exit\n") == 0){
			// _read_write();
			printf("Exiting read thread\n");
			start_listening();
			// pthread_exit(0);
			return;
		}
		memset(read_buf, 0, strlen(read_buf));
	}
	// close(sfd);
}


int main(int argc, char *argv[]){


	int port; //port on which server runs
	port = SERVER_FAMOUS_PORT;
	if (argc <= 2){
		printf("service required(lower/upper/return)\n");
		exit(1);
	}
	my_port = atoi(argv[2]);

	int client_addr_len; // var to store len of the address of client. 
	char *buffer, buffer1[256];
	char *service = argv[1];

	buffer = (char *)malloc(sizeof(char)*100);

	struct sockaddr_in server_addr;
	struct sockaddr_in my_addr;
	// creating a socket
	sfd = socket(AF_INET, SOCK_STREAM, 0);
	print_error(sfd, "error opening socket");




	// port = atoi(argv[1]);
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(port);
	//Computer networks are big endian. This means that when little endian computers are going to pass integers over the network (IP addresses for example), they need to convert them to network byte order. Likewise, when the receive integer values over the network, they need to convert them back to their own native representation.
	server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");


//binding before connecting
	// my_addr = server_addr;
	// my_addr.sin_port = htons(atoi(argv[2]));

	// int b = bind(sfd, (struct sockaddr *)&my_addr, sizeof(my_addr));
	// print_error(b, "Failed to bind.");
// binding before connecting


	// establishing the connection to server.
	int c = connect(sfd, (struct sockaddr *)&server_addr, sizeof(server_addr));
	print_error(c, "could not connect to server.");
	struct sockaddr_in *me = get_my_ip(sfd);
	my_port = ntohs(me->sin_port);


	printf("%s\n", "connected to server");


	// reading from stdin and sending to server.
	size_t write_size;
	int read_size;

	int s = send(sfd, service, sizeof(service), 0);
	print_error(s, "Failed to send initial message to server");
	int r = recv(sfd, buffer1, 256, 0);
	print_error(r, "Failed to receive a reply from the server");
	printf("Server:%s\n", buffer1);

	int service_port = atoi(buffer1);
	if(service_port == 0){
		printf("Service does not exist. \n");
		exit(1);
	}
	nsfd = sfd;

	if(service_port != 1){
		// port is returned. Need to take permission from the existing client.
		memset(buffer, 0, strlen(buffer));
		memset(buffer1, 0, strlen(buffer1));
		// sleep(15);
		close(sfd);
		
		printf("Trying to connect to other client\n");
		nsfd = socket(AF_INET, SOCK_STREAM, 0);
		print_error(nsfd, "error opening socket");

		server_addr.sin_port = htons(service_port);
		c = connect(nsfd, (struct sockaddr *)&server_addr, sizeof(server_addr));
		print_error(c, "could not connect to service.");
		printf("%s\n", "Connected to service");
	}

	pthread_t tt;
	pthread_create(&tt, NULL, reading_thread, (void*)0);
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
		int s = send(nsfd, buffer, strlen(buffer), 0);
		print_error(s, "Failed to send message to server");

		//receive a reply from the server
		// int r = recv(nsfd, buffer1, 256,0);
		// print_error(r, "Failed to receive a reply from the server");
		// printf("Server:%s", buffer1);
		memset(buffer, 0, 256);
		memset(buffer1, 0, 256);
	}


	printf("Client disconnected\n");
	fflush(stdout);
	return 0;

}