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

#include "../../helper.h"
void print_error(int val, char*msg){
	if(val < 0){
		perror(msg);
		exit(val);
	}
}


#define CABLE_PROTOCOL 100
#define PORT 9999 
// dummy port. POrt isnt actually required.
int rsfd;

int main(){
	rsfd = socket(AF_INET, SOCK_RAW, CABLE_PROTOCOL);
	print_error(rsfd, "Failed to create raw socket");

	struct sockaddr_in client_addr;
	client_addr.sin_family = AF_INET;
	client_addr.sin_port = htons(PORT);
	//Computer networks are big endian. This means that when little endian computers are going to pass integers over the network (IP addresses for example), they need to convert them to network byte order. Likewise, when the receive integer values over the network, they need to convert them back to their own native representation.
	client_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
	int b = bind(rsfd, (struct sockaddr *)&client_addr, sizeof(client_addr));
	print_error(b, "Failed to bind.");

	
	int to_send = 0;
	char *msg = (char*)malloc(sizeof(char)*100);
	while(1){
		printf("%s\n", "Enter message to send.");
		scanf("%s", msg);
		if (strcmp(msg, "exit") == 0)
			break;

		int s = sendto(rsfd, msg, sizeof(msg), 0 , (struct sockaddr *)&client_addr, sizeof(client_addr));
		// int s = send(rsfd, msg, sizeof(msg), 0 );
		print_error(s, "Failed to send message");

	}
}