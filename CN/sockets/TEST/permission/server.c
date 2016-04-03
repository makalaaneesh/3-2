//http://www.linuxhowtos.org/C_C++/socket.htm
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <signal.h>
#define PORT 9999
#define STDIN 0
#define STDOUT 1


int clients;
int client_nsfd;
int echo_pid = 0;
int client_port;
struct sockaddr_in* client;

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
int nsfd; //new fd when accept is called

void clean(int signo){
	printf("Shutting down server.\n");
	shutdown(nsfd, SHUT_RDWR);
	shutdown(sfd, SHUT_RDWR);
}

void print_error(int val, char*msg){
	if(val < 0){
		perror(msg);
		exit(val);
	}
}

int main(int argc, char *argv[]){
	clients = 0;
	if(signal(SIGINT, clean) == SIG_ERR){
  		printf("%s\n", "Error in catching SIGINT");
 	}

	int port; //port on which server runs
	int client_addr_len; // var to store len of the address of client. 
	char buffer[256];

	struct sockaddr_in server_addr, client_addr;
	// server_addr = (struct sockaddr_in *)malloc(sizeof(struct sockaddr_in));
	// client_addr = (struct sockaddr_in *)malloc(sizeof(struct sockaddr_in));

	// if(argc < 2){
	// 	printf("No port given.\n");
	// 	exit(1);
	// }

	// creating a socket
	sfd = socket(AF_INET, SOCK_STREAM, 0);
	print_error(sfd, "error opening socket");

	//setting resuseaddr so that we don't have to constantly wait to the socket to timeout out of it's TIMEWAIT state.
	int set = setsockopt(sfd, SOL_SOCKET, SO_REUSEADDR, &(int){ 1 }, sizeof(int));
    	print_error(set, "setsockopt(SO_REUSEADDR) failed");


	// port = atoi(argv[1]);
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
		print_error(sfd, "Failed in accepting connection");
		printf("Accepted connection from %d\n", inet_ntoa(client_addr.sin_addr));
		int r = recv(nsfd, buffer, 256, 0);
		print_error(r, "Read fail");
		if (clients > 0){
			int port = client_port;
			printf("Client port is %d\n", port);
			char * msg = (char *)malloc(sizeof(char)*100);
			sprintf(msg, "%d", port);
			int s = send(nsfd, msg, strlen(msg), 0);
			print_error(s, "Failed to send back initial messag!!!");
			close(nsfd);
			kill(echo_pid, SIGUSR1);
			continue;
		}
		else{
			char * msg = (char *)malloc(sizeof(char)*100);
			sprintf(msg, "%d", 1);
			int s = send(nsfd, msg, strlen(msg), 0);
			print_error(s, "Failed to send back initial messageqqq");
			clients = 1;
			client = (get_peer_ip(nsfd));
			int port = ntohs(client->sin_port);
			client_port = port;
			printf("port%d", client_port);

		}


		printf("Ready to fork\n");
		fflush(stdout);
		int pid = fork();
		print_error(pid, "fork failed");

		if (pid == 0){//child
			
			close(sfd);
			// printf("1-%d\n", nsfd);
			if(nsfd != STDIN){
				printf("Duping input\n");
				if(dup2(nsfd, STDIN)!= STDIN){
					print_error(-1, "Failed to dup2 stdin");
				}

			}
			printf("2-%d\n", nsfd);
			if(nsfd != STDOUT){
				printf("Duping output\n");
				if(dup2(nsfd, STDOUT)!= STDOUT){
					print_error(-1, "Failed to dup2 stdout");
				}

			}
			close(nsfd);
			// printf("3-%d\n", nsfd);
			// fflush(stdout);
			// close(nsfd);
			// printf("Starting service %s\n", buffer);
			fflush(stdout);
			
			execl(buffer, buffer, (char*)0);
			// int read_size;
			// while( (read_size = recv(nsfd, buffer, 256, 0)) > 0){
			// 	printf("Message:%s\n", buffer);
			// 	int s = send(nsfd, buffer, sizeof(buffer), 0);
			// 	print_error(s, "Failed to send message to client");
			// 	memset(buffer, 0, 256);
			// }
                   

		}
		else{
			echo_pid = pid;
			close(nsfd);
			continue;
		}
	}
	

	// // reading messages from the client.
	// int read_size;
	// while( (read_size = recv(nsfd, buffer, 256, 0)) > 0){
	// 	printf("Message:%s\n", buffer);
	// 	int s = send(nsfd, buffer, sizeof(buffer), 0);
	// 	print_error(s, "Failed to send message to client");
	// 	memset(buffer, 0, 256);
	// }


	printf("Client disconnected\n");
	fflush(stdout);
	return 0;

}