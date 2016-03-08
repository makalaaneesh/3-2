//http://www.linuxhowtos.org/C_C++/socket.htm
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <signal.h>
#include <pthread.h>
#define PORT 10000

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

void _read();
void _read_write();


	char *read_buf;

void *reading_thread(void * arg){

	printf("Reading thraed\n");
	fflush(stdout);
	printf("1\n");
	
	printf("2\n");
	while(1){
		int r = read(sfd, read_buf, 100);
		
		print_error(r, "read failed");
		if (r >0)
			printf("Server->[%s]", read_buf);

		fflush(stdout);
		
		if (strcmp(read_buf,"exit") == 0){
			// _read_write();
			printf("Exiting read thread\n");
			// pthread_exit(0);
			return;
		}
		memset(read_buf, 0, strlen(read_buf));
	}
	// close(sfd);
}
char *buffer;
void *read_write_thread(void *arg){
	printf("Read_write thread\n");
	char buffer1[256];
	// char *service = argv[1];
	
	size_t write_size;
	int read_size;

	while(1){
		// printf("Enter word to convert \n");
		ssize_t in  = getline(&buffer, &write_size, stdin);
		// fgets(buffer,256,stdin);
		// gets(buffer);
		// scanf("%s",buffer);
		print_error(in, "Failed to read from stdin");

		

		// printf("Sending:[%s]\n", buffer);
		fflush(stdout);
		int s = send(sfd, buffer, strlen(buffer), 0);
		print_error(s, "Failed to send message to server");


		//receive a reply from the server
		int r = recv(sfd, buffer1, 256,0);
		print_error(r, "Failed to receive a reply from the server");
		printf("Server:%s", buffer1);
		if(strcmp(buffer, "exit\n") == 0){
			// _read();
			// pthread_exit(0);
			printf("Returning writing thread\n");
			return;
		}

		memset(buffer, 0, 256);
		memset(buffer1, 0, 256);
	}
}

void _read(){
	printf("%s\n", "creating read thread");
	reading_thread((void*)0);
	// pthread_t tt;
	// pthread_create(&tt, NULL, reading_thread, (void*)0);
	// pthread_join(tt, NULL);
	printf("created reading_thread\n");
}

void _read_write(){
	// pthread_t ttt;
	// pthread_create(&ttt, NULL, read_write_thread, (void *)0);
	read_write_thread((void*)0);
	// pthread_join(ttt, NULL);
}



int main(int argc, char *argv[]){
read_buf = (char *)malloc(sizeof(char)*100);
buffer = (char *)malloc(sizeof(char)*100);

	int port; //port on which server runs
	int client_addr_len; // var to store len of the address of client. 
	
	struct sockaddr_in server_addr;

	// creating a socket
	sfd = socket(AF_INET, SOCK_STREAM, 0);
	print_error(sfd, "error opening socket");

	// port = atoi(argv[1]);
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(PORT);
	//Computer networks are big endian. This means that when little endian computers are going to pass integers over the network (IP addresses for example), they need to convert them to network byte order. Likewise, when the receive integer values over the network, they need to convert them back to their own native representation.
	server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

	// establishing the connection to server.
	int c = connect(sfd, (struct sockaddr *)&server_addr, sizeof(server_addr));
	print_error(c, "could not connect to server.");

	printf("%s\n", "connected to server");


	while(1){
		_read();
		_read_write();
	}
	
	while(1);
	printf("Client disconnected\n");
	fflush(stdout);
	return 0;

}