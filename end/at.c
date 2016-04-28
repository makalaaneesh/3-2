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
#include "helper.h"



struct round{
	char name[10];
	int usfd;
	int unsfd;
	int free;
};
struct round rounds[3] = { {"r1", 0 ,0 ,1},{"r2", 0 ,0 ,1},{"r3", 0 ,0,1 } };

#define PASS_SCORE 5


void *recv_status(void* arg){
	int sfd = *((int*)arg);
	while(1){
		char * buf = (char *)malloc(sizeof(char)* 100);
		int r = recv(sfd, buf, 100, 0);
		if(strcmp(buf, "1") == 0){

		}
	}
}


#define FAMOUS_PORT 5000


int apti_init(){
	int i;
	for(i = 0; i< 3; i++){
		rounds[i].usfd = socket(AF_LOCAL, SOCK_STREAM, 0);
		print_error(rounds[i].usfd, "error opening socket");

		//setting resuseaddr so that we don't have to constantly wait to the socket to timeout out of it's TIMEWAIT state.
		int set = setsockopt(rounds[i].usfd, SOL_SOCKET, SO_REUSEADDR, &(int){ 1 }, sizeof(int));
		print_error(set, "setsockopt(SO_REUSEADDR) failed");
		char *path = (char*)malloc(sizeof(char)*100);
		strcpy(path, rounds[i].name);
		strcat(path,".sock");


		struct sockaddr_un addr;
		memset(&addr, 0, sizeof(addr));
		addr.sun_family = AF_LOCAL;
		unlink(path);
		strcpy(addr.sun_path, path);

		int b = bind(rounds[i].usfd, (struct sockaddr *)&addr, sizeof(addr));
		print_error(b, "Failed to bind.");

		int l = listen(rounds[i].usfd, 3);
		print_error(l , "Failed to listen");

		printf("Unix domain socket has been created for round %d at path %s\n", i, path);


		// accepting the connection from the r process!


		int nsfd;
		struct sockaddr_un client_addr; // to store the address of the accepted client.
		socklen_t client_addr_len = sizeof(client_addr); // var to store len of the address of client. 

		rounds[i].unsfd = accept(rounds[i].usfd,(struct sockaddr * )&client_addr, &client_addr_len );
		print_error(rounds[i].usfd, "Failed in accepting connection");
		printf("Accepted connection. Unix domain connection for round %d established\n", i);



	}
}

int getFreeRound(){
	int val = -1;
	int i;
	for(i = 0; i< 3; i++){
		if (rounds[i].free == 1){
			return i;
		}
	}
	return val;
}
int conductAptitude(int nsfd){
	char * buffer = (char *)malloc(sizeof(char)*100);
	strcpy(buffer, "What score do you want?");
	int s = send(nsfd, buffer, strlen(buffer), 0);
	print_error(s, "FAILED TO SEND TO CANDIDATE");
	char *readbuffer  = (char *)malloc(sizeof(char)*100);
	int r = recv(nsfd, readbuffer, 100, 0);
	print_error(r, "FAILED TO RECEIVE TO CANDIDATE");
	int score = atoi(readbuffer);
	return score;
}

void *handle_candidate(void *arg){
	int nsfd = *((int*)arg);
	int score = conductAptitude(nsfd);
	if(score >= PASS_SCORE){
		int index = getFreeRound();
		while(index == -1){
			index = getFreeRound();
		}

		printf("Free round is %s\n", rounds[index].name);
		fflush(stdout);
		send_fd(rounds[index].unsfd, nsfd);
		rounds[index].free = 0;
	}
	else{
		char *buf = (char*)malloc(sizeof(char)*100);
		strcpy(buf, "You have not passed the apti!");
		int s = send(nsfd, buf, strlen(buf), 0);
		print_error(s, "FAILED TO SEND MSG TO CANDIDATE");
	}

}



int sfd;
int main(){

	apti_init();
	sfd = socket(AF_INET, SOCK_STREAM, 0);
	print_error(sfd, "error opening socket");

	//setting resuseaddr so that we don't have to constantly wait to the socket to timeout out of it's TIMEWAIT state.
	int set = setsockopt(sfd, SOL_SOCKET, SO_REUSEADDR, &(int){ 1 }, sizeof(int));
	print_error(set, "setsockopt(SO_REUSEADDR) failed");


	struct sockaddr_in server_addr;
	// port = atoi(argv[1]);
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(FAMOUS_PORT);
	//Computer networks are big endian. This means that when little endian computers are going to pass integers over the network (IP addresses for example), they need to convert them to network byte order. Likewise, when the receive integer values over the network, they need to convert them back to their own native representation.
	server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

	//binding the socket to the addr
	int b = bind(sfd, (struct sockaddr *)&server_addr, sizeof(server_addr));
	print_error(b, "Failed to bind.");

	listen(sfd, 3);

	printf("%s%d\n", "Aptitude Server is listening for connections at port ",  FAMOUS_PORT);


	while(1){
		int nsfd;
		
		
		struct sockaddr_in client_addr; // to store the address of the accepted client.
		socklen_t client_addr_len = sizeof(client_addr); // var to store len of the address of client. 

		nsfd = accept(sfd,(struct sockaddr * )&client_addr, &client_addr_len );
		// nsfd = accept(sfd,NULL, NULL);
		print_error(nsfd, "Failed in accepting connection");
		printf("Accepted connection.\n");
		pthread_t t;
		pthread_create(&t, NULL, handle_candidate, (void*)&nsfd);
	}

}