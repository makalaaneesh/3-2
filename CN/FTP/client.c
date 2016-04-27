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
int data_sfd;
int data_port;


void print_error(int val, char*msg){
	if(val < 0){
		perror(msg);
		exit(val);
	}
}

void sendPASVcommand(){
	int s = send(control_sfd, "PASV", sizeof("PASV"), 0);
	print_error(s, "FAILED TO SEND PASV COMMAND");
	printf("Sent PASV command\n");

}

void recvPORTnumber(){
	char *buffer = (char *)malloc(sizeof(char)*10);
	int r = recv(control_sfd, buffer, 10, 0);
	print_error(r, "FAILED TO RECEIVE PORT NUMBER FROM THE FTP SERVER");
	data_port = atoi(buffer);
	printf("Received port number %d\n", data_port);

}

void startPASVconn(){
	control_sfd = socket(AF_INET, SOCK_STREAM, 0);
	print_error(control_sfd, "FAILED TO CREATE SOCKET");

	struct sockaddr_in server_control;
	server_control.sin_family = AF_INET;
	server_control.sin_port = CONTROL_PORT;
	server_control.sin_addr.s_addr = inet_addr("127.0.0.1");

	int c = connect(control_sfd, (struct sockaddr *)&server_control, sizeof(server_control));
	print_error(c, "FAILED TO CONNECT TO SERVER(pasv COMMAND)");

	// connection accepted.
	// client will send the pasv command and wait for the port number that the FTP server has opened for the data transfer
	sendPASVcommand();
	recvPORTnumber();

}

void connectDATAconn(){
	data_sfd = socket(AF_INET, SOCK_STREAM, 0);
	print_error(data_sfd, "FAILED TO CREATE SOCKET");

	struct sockaddr_in server_control;
	server_control.sin_family = AF_INET;
	server_control.sin_port = data_port;

	int c = connect(data_sfd, (struct sockaddr *)&server_control, sizeof(server_control));
	print_error(c, "FAILED TO CONNECT TO SERVER(DATA)");
	printf("%s\n", "Succesfully connected to the ftp server. Now issue a command");
}


void sendCommand(char * cmd){
	int s = send(control_sfd, cmd, strlen(cmd), 0);
	print_error(s, "FAILED TO SEND COMMAND TO SERVER");
}

void recvData(){
	char *buffer = (char *)malloc(sizeof(char)*1000);
	int r = recv(data_sfd, buffer, 1000, 0);
	print_error(r, "FAILED TO RECEIVE DATA FROM THE FTP SERVER");
	printf("============================data=======================\n%s\n============================================\n",buffer);
}

void command(){
	char *cmd = (char *)malloc(sizeof(char)*100);
	while(1){
		printf("\n#ftp>");
		fflush(stdout);
		fgets(cmd, 100, stdin);
		if(strlen(cmd)<=0)
			continue;
		int i = strlen(cmd)-1;
		if( cmd[i] == '\n') 
			cmd[i] = '\0';
		printf("[%s]\n", cmd);
		sendCommand(cmd);
		recvData();
		memset(cmd, 0, sizeof(cmd));


	}
	

}

int main(){
	startPASVconn();
	connectDATAconn();
	command();

}