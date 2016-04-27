#include <stdlib.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
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
#include <sys/time.h>
#include <sys/un.h>
#include <stropts.h>
#include <poll.h>



#define CONTROL_PORT 2000
int control_sfd;
int current_port = 7000; // random data ports to assign to client.




void print_error(int val, char*msg){
	if(val < 0){
		perror(msg);
		exit(val);
	}
}

void initFTP(){
	control_sfd =  socket(AF_INET, SOCK_STREAM, 0);
	print_error(control_sfd, "FAILED TO CREATE SOCKET");

	struct sockaddr_in myaddr;
	myaddr.sin_family = AF_INET;
	myaddr.sin_port = CONTROL_PORT;
	myaddr.sin_addr.s_addr = inet_addr("127.0.0.1");

	// server binds to this address and listens for incoming connections
	int b = bind(control_sfd, (struct sockaddr *)&myaddr, sizeof(myaddr));
	print_error(b, "FAILED TO BIND");

	listen(control_sfd, 3);
	printf("FTP server started.\n");

}

char* _read(char* filename){
	char *data;
	data = (char*)malloc(sizeof(char)*1024);
    int fd = open(filename, O_RDONLY);
 
    int r = read(fd, data, 1024);
    print_error(r, "FAILED TO READ FILE.");
    close(fd);
    return data;
}

void* spawnConnection(void *arg){
	int ccontrolfd;// client fd;
	ccontrolfd = *((int*)arg);
	char *buffer = (char*)malloc(sizeof(char)* 100);
	int r = recv(ccontrolfd, buffer, 100, 0);
	printf("Received command -- %s\n", buffer);
	if(strcmp(buffer, "PASV") != 0){ // some other command. unsupported as of now.
		pthread_exit(1);
	}

	// -----------data socket to exchange data.-------------//
	int datasfd =  socket(AF_INET, SOCK_STREAM, 0);
	print_error(datasfd, "FAILED TO CREATE SOCKET");

	struct sockaddr_in myaddr;
	myaddr.sin_family = AF_INET;
	myaddr.sin_port = current_port++;
	myaddr.sin_addr.s_addr = inet_addr("127.0.0.1");

	// server binds to this address  and sends the port number back to the client to connect to.
	int b = bind(datasfd, (struct sockaddr *)&myaddr, sizeof(myaddr));
	print_error(b, "FAILED TO BIND");

	listen(datasfd, 3);



	// ----------------sending the port number to the client-----------------//

	char *port_number = (char*)malloc(sizeof(char)*10);
	sprintf(port_number, "%d", myaddr.sin_port);
	int s = send(ccontrolfd, port_number, sizeof(port_number),0);
	print_error(s, "FAILED TO SEND PORT NUMBER TO THE CLIENT");
	printf("Sent port number %s back to client\n", port_number);



	// ---- -----------accepting the conncetion for the data port just opened -------------//
	struct sockaddr_in client_addr; // to store the address of the accepted client.
	socklen_t client_addr_len = sizeof(client_addr); // var to store len of the address of client. 

	int cdatasfd = accept(datasfd,(struct sockaddr * )&client_addr, &client_addr_len );


	printf("Received data connection with client. \n");
	printf("Listening for commands now\n");
	fflush(stdout);

	// --------------Start receiving commands and sending back data---------------//

	while(1){
		char *cmd = (char*)malloc(sizeof(char)*100);
		int r = recv(ccontrolfd, cmd, 100, 0);
		printf("Received command [%s]\n", cmd);
		print_error(r, "FAILED TO RECEVE COMMAND");


		char *c = strtok(cmd, " ");
		if(strcmp(c, "read") == 0){
			char *filename = strtok(NULL, " ");
			char *data = _read(filename);
			int s = send(cdatasfd, data, strlen(data), 0);
			printf("Sent data back to client\n");
		}
		memset(cmd, 0, sizeof(cmd));
	}
	



}

void waitForIncomingConnections(){
	while(1){
		int nsfd;
		
		struct sockaddr_in client_addr; // to store the address of the accepted client.
		socklen_t client_addr_len = sizeof(client_addr); // var to store len of the address of client. 

		nsfd = accept(control_sfd,(struct sockaddr * )&client_addr, &client_addr_len );
		// nsfd = accept(sfd,NULL, NULL);
		print_error(nsfd, "Failed in accepting connection");
		printf("Accepted connection.\n");
		pthread_t t;
 		pthread_create(&t, NULL, spawnConnection, (void*)&nsfd);

	}
}

int main(){
	initFTP();
	waitForIncomingConnections();

}