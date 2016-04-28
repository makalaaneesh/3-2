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

struct candidate{
	int id;
	int r1;
	int r2;
	int r3;
	int result;
	int mode; /* 1 for ask 1 for reply */
};

#define PORT 6000
#define PROTOCOL 143



int rsfd;
struct sockaddr_in client_addr;

int candidate_index = 0;
struct candidate candidates[5];
int sfd;

void print_candidate(struct candidate c){
	printf("%d : %d|%d|%d --- %d\n",c.id, c.r1, c.r2, c.r3, c.result );
}


void init_candidate(struct candidate *c){
	c->id =0;
	c->r1 = 0;
	c->r2 = 0;
	c->r3 = 0;
	c->result = 0;
	c->mode = 0;
}

int getCandidate(int id){
	int i ;
	for(i = 0; i< 5;i++){
		if (id == candidates[i].id)
			return i;
	}
	return -1;
}
int getCandidatesPassed(){
	int c = 0;
	int i ;
	for(i = 0; i< 5;i++){
		if (candidates[i].result > 25){
			c++;
		}
	}
	return c;
}
int printCandidatesPassed(){
	int i ;
	for(i = 0; i< 5;i++){
		if (candidates[i].result > 25){
			int index = i;
			char * buf = (char *)malloc(sizeof(char)*100);
			snprintf(buf, 100, "Result %d : %d", candidates[index].id, candidates[index].result);
			int s = sendto(rsfd, buf, strlen(buf), 0 , (struct sockaddr *)&client_addr, sizeof(client_addr));
			// int s = send(rsfd, msg, sizeof(msg), 0 );
			print_error(s, "Failed to send message");
		}
	}
}


void updateCandidate(struct candidate *c){
	int id = c->id;
	int val = -1;
	int i;
	for(i = 0; i< 5;i++){
		if (id == candidates[i].id){
			val = i;
			candidates[i] = *c;
		}
	}
	if (val == -1){
		candidates[candidate_index++] = *c;
	}
	printf("UPdated candidate %d\n", c->id);
}

void * handle_round(void *arg){
	int nsfd = *((int*)arg);
	while(1){


		
	////////

		char *buffer=(char *)malloc(sizeof(char)*100);
		char *buffer1 = (char *)malloc(sizeof(char)*100);
		memset(buffer, 0, 100);
		memset(buffer1, 0, 100);
		int r = recv(nsfd, buffer, 100, 0);
		printf("[%s]\n", buffer);
		print_error(r, "failed to receive from round");
		char * id = strtok(buffer, " ");
		char *rr = strtok(NULL, " ");
		char * score = strtok(NULL, " ");
		if (score == NULL){
			int index = getCandidate(atoi(id));
			if(index == -1){
				strcpy(buffer1, "1");
				int s = send(nsfd, buffer1, 100, 0);
				print_error(s, "failed to sendto round");
				candidates[candidate_index++].id = atoi(id);
				continue;
			}
			if(strcmp(rr, "r1") == 0){
				if (candidates[index].r1 == 0){
					strcpy(buffer1, "1");
					int s = send(nsfd, buffer1, 100, 0);
					print_error(s, "failed to sendto round");
				}
				else{
					strcpy(buffer1, "0");
					int s = send(nsfd, buffer1, 100, 0);
					print_error(s, "failed to sendto round");
				}
			}
			if(strcmp(rr, "r2") == 0){
				if (candidates[index].r2 == 0){
					strcpy(buffer1, "1");
					int s = send(nsfd, buffer1, 100, 0);
					print_error(s, "failed to sendto round");
				}
				else{
					strcpy(buffer1, "0");
					int s = send(nsfd, buffer1, 100, 0);
					print_error(s, "failed to sendto round");
				}
			}
			if(strcmp(rr, "r3") == 0){
				if (candidates[index].r3 == 0){
					strcpy(buffer1, "1");
					int s = send(nsfd, buffer1, 100, 0);
					print_error(s, "failed to sendto round");
				}
				else{
					strcpy(buffer1, "0");
					int s = send(nsfd, buffer1, 100, 0);
					print_error(s, "failed to sendto round");
				}
			}

		}
		else{
			int index = getCandidate(atoi(id));
			printf("Index is %d\n", index);
			if(strcmp(rr, "r1") == 0){
				candidates[index].r1 = atoi(score);
				candidates[index].result += atoi(score);
				
			}
			if(strcmp(rr, "r2") == 0){
				candidates[index].r2 =  atoi(score);
				candidates[index].result += atoi(score);
			}
			if(strcmp(rr, "r3") == 0){
				candidates[index].r3 =  atoi(score);
				candidates[index].result += atoi(score);
			}
			print_candidate(candidates[index]);


			//broadcastig!!!
			if(getCandidatesPassed()>=2 && candidates[index].r1 > 0 && candidates[index].r2 > 0  && candidates[index].r3 > 0 &&  candidates[index].result > 25){
				// char * buf = (char *)malloc(sizeof(char)*100);
				// snprintf(buf, 100, "Result %d : %d", candidates[index].id, candidates[index].result);
				// int s = sendto(rsfd, buf, strlen(buf), 0 , (struct sockaddr *)&client_addr, sizeof(client_addr));
				// // int s = send(rsfd, msg, sizeof(msg), 0 );
				// print_error(s, "Failed to send message");

				printCandidatesPassed();
			}
		}
		
		



		///////
		/*struct candidate *c = (struct candidate *)malloc(sizeof(struct candidate));
		int r = recv(nsfd, c, 100, 0);
		print_error(r, "failed to receive from round");
		struct candidate *c1;
		if(c->mode == 1){ //ask
			printf("Asking for candidate id %d\n", c->id);
			int index = getCandidate(c->id);
			if(index == -1){
				c1 = (struct candidate *)malloc(sizeof(struct candidate));	
				init_candidate(c1);

			}
			else{
				c1 = &candidates[index];
			}
			int s = send(nsfd, c1, sizeof(c1), 0);
			print_error(s, "Failed to send back to rround");
			
		}
		else if (c->mode == 2){
			print_candidate(*c);
			updateCandidate(c);
		}*/

	}
	pthread_exit(1);
}

int startAccepting(){

	while(1){
		int nsfd;

		struct sockaddr_in client_addr; // to store the address of the accepted client.
		socklen_t client_addr_len = sizeof(client_addr); // var to store len of the address of client. 

		nsfd = accept(sfd,(struct sockaddr * )&client_addr, &client_addr_len );
		// nsfd = accept(sfd,NULL, NULL);
		print_error(nsfd, "Failed in accepting connection");
		printf("Accepted connection. in J\n");
		pthread_t t;
		pthread_create(&t, NULL, handle_round, (void*)&nsfd);
	}
	
}


void init_broadcasting(){
	client_addr.sin_family = AF_INET;
	client_addr.sin_port = htons(9999);
	//Computer networks are big endian. This means that when little endian computers are going to pass integers over the network (IP addresses for example), they need to convert them to network byte order. Likewise, when the receive integer values over the network, they need to convert them back to their own native representation.
	client_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
	rsfd = socket(AF_INET, SOCK_RAW, PROTOCOL);
	print_error(rsfd, "Failed to create raw socket");

	// struct sockaddr_in client_addr;
	// client_addr.sin_family = AF_INET;
	// client_addr.sin_port = htons(9999);
	// //Computer networks are big endian. This means that when little endian computers are going to pass integers over the network (IP addresses for example), they need to convert them to network byte order. Likewise, when the receive integer values over the network, they need to convert them back to their own native representation.
	// client_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
	int b = bind(rsfd, (struct sockaddr *)&client_addr, sizeof(client_addr));
	print_error(b, "Failed to bind.");

	
	// int to_send = 0;
	// char *msg = (char*)malloc(sizeof(char)*100);
	// while(1){
	// 	printf("%s\n", "Enter message to send.");
	// 	scanf("%s", msg);
	// 	if (strcmp(msg, "exit") == 0)
	// 		break;

	// 	int s = sendto(rsfd, msg, sizeof(msg), 0 , (struct sockaddr *)&client_addr, sizeof(client_addr));
	// 	// int s = send(rsfd, msg, sizeof(msg), 0 );
	// 	print_error(s, "Failed to send message");

	// }
}

int main(){
	init_broadcasting();
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

	//binding the socket to the addr
	int b = bind(sfd, (struct sockaddr *)&server_addr, sizeof(server_addr));
	print_error(b, "Failed to bind.");

	listen(sfd, 3);

	printf("%s%d\n", "J is listening for connections at port ",  PORT);

	startAccepting();
}