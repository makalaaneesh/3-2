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
		print_error(nsfd, "Failed in accepting connection");
		printf("Accepted connection from %d\n", inet_ntoa(client_addr.sin_addr));

		


		int pid = fork();
		print_error(pid, "fork failed");

		if (pid == 0){//child
			int r = recv(nsfd, buffer, 256, 0);
			print_error(r, "Read failed");
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
			printf("Starting service %s\n", buffer);
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