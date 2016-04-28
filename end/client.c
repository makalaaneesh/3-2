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
#include "sock_helper.h"


int sfd;
#define PORT 5000

#define PROTOCOL 143

void *brod_reading_thread(void *arg){
	int rsfd;
	rsfd = socket(AF_INET, SOCK_RAW, PROTOCOL);
	print_error(rsfd, "Failed to create raw socket");

	struct sockaddr_in client_addr;
	socklen_t client_len = sizeof(client_addr);

	char *msg = (char *)malloc(sizeof(char)*100);
	while(1){
		// printf("%s\n", "Waiting to receive message");
		int s = recvfrom(rsfd, msg, 100, 0 , (struct sockaddr *)&client_addr, &client_len);
		print_error(s, "Failed to receive");
		// printf("MSG->[%s]\n", msg);
		// int p = print_ip_header(msg);
		char *msg_rcvd = msg+20;
		printf("ANNOUNCEMENT!![%s]\n", msg_rcvd);
		fflush(stdout);
		memset(msg, 0 , sizeof(msg));

	}
}



int main(){
	sfd = socket(AF_INET, SOCK_STREAM, 0);
	print_error(sfd, "error opening socket");

	//setting resuseaddr so that we don't have to constantly wait to the socket to timeout out of it's TIMEWAIT state.
	int set = setsockopt(sfd, SOL_SOCKET, SO_REUSEADDR, &(int){ 1 }, sizeof(int));
	print_error(set, "setsockopt(SO_REUSEADDR) failed");


	struct sockaddr_in server_addr;
	// port = atoi(argv[1]);
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(PORT);
	//Computer networks are big endian. This means that when little endian computers are going to pass integers over the network (IP addresses for example), they need to convert them to network byte order. Likewise, when the receive integer values over the network, they need to convert them back to their own native representation.
	server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");


	int c = connect(sfd, (struct sockaddr *)&server_addr, sizeof(server_addr));
	print_error(c, "could not connect to Jserver.");
	printf("%s\n", "connected to main server");
	get_my_ip(sfd);

	pthread_t t;
	pthread_create(&t, NULL, &brod_reading_thread, (void*)0);


	char *buf = (char *)malloc(sizeof(char)*100);
	while(1){
		memset(buf, 0, 100);
		
		int r = recv(sfd, buf, 100, 0);
		print_error(r, "failed to recv");
		printf("[%s]\n", buf);
		fflush(stdout);
		fgets(buf, 100, stdin);
		int s = send(sfd, buf, strlen(buf), 0);
		print_error(s, "failed to send");
		
	}

}

