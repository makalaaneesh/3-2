#include "sock_helper.h"


char *type;
char *next;
char *prev;
int sfd;

int next_usfd; // unixdomain sock to send clients to the next multiplex
int next_unsfd; // accepted socket of the above kind.
int prev_usfd;
int usfd;
int unsfd;
#define PORT 9999
#define MAX_CLIENTS 10
#define SOCKPATH "backup.sock"
int client_fd_list[MAX_CLIENTS];
int client_fd_list_index;
int capacity; // actual capacity of the multiplex

int busy;
int jsfd;
#define JPORT 6000

int ser_pid; //server pid

struct candidate{
	char ip[100];
	int id;
	int r1;
	int r2;
	int r3;
	int result;
	int mode; /* 1 for ask 1 for reply */
};

void init_candidate(struct candidate *c){

	c->id =0;
	c->r1 = 0;
	c->r2 = 0;
	c->r3 = 0;
	c->result = 0;
	c->mode = 0;
}

void init(){
	busy = 0;
	jsfd = socket(AF_INET, SOCK_STREAM, 0);
	print_error(jsfd, "error opening socket");

	//setting resuseaddr so that we don't have to constantly wait to the socket to timeout out of it's TIMEWAIT state.
	int set = setsockopt(jsfd, SOL_SOCKET, SO_REUSEADDR, &(int){ 1 }, sizeof(int));
	print_error(set, "setsockopt(SO_REUSEADDR) failed");


	struct sockaddr_in server_addr;
	// port = atoi(argv[1]);
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(JPORT);
	//Computer networks are big endian. This means that when little endian computers are going to pass integers over the network (IP addresses for example), they need to convert them to network byte order. Likewise, when the receive integer values over the network, they need to convert them back to their own native representation.
	server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");


	int c = connect(jsfd, (struct sockaddr *)&server_addr, sizeof(server_addr));
	print_error(c, "could not connect to Jserver.");
	printf("%s\n", "connected to JJJJJJJJ server");




	////////////////////


	client_fd_list_index = 0;
	char * sockname = (char*)malloc(sizeof(char)*20);
	strcpy(sockname, type);
	strcat(sockname, ".sock");
	// usfd = s_socket(AF_LOCAL, SOCK_STREAM, 9999, sockname);
	usfd = c_socket(AF_LOCAL, SOCK_STREAM, sockname);
	_connect(usfd, AF_LOCAL, SOCK_STREAM, 0, sockname);
	// unsfd = ud_accept(usfd);
	printf("Connected to server. Ready to receive fds.\n");

	fflush(stdout);

}


void *service (void *arg){
	while(busy);
	busy = 1;

	int nsfd = *(int *)arg;


	printf("Round %s is conducting interview for %d\n", type, nsfd);
	sleep(5);


	int s,r;


	// char *idstr = (char *)malloc(sizeof(char)*100);
	// strcpy(idstr, "What is your id");
	// int s = send(nsfd, idstr, strlen(idstr), 0);
	// print_error(s, "FAILED TO SEND TO CANDIDATE id");
	// int r = recv(nsfd, idstr, 100, 0);
	// print_error(r, "FAILED TO RECEIVE TO CANDIDATE");
	// int id = atoi(idstr);

	int id;
	  get_peer_ip(nsfd, &id);
	////

	char *buf1 = (char *)malloc(sizeof(char)*100);
	snprintf(buf1, 100, "%d %s", id, type);
	// snprintf(buf1, 100, "%s %s", ip, type);
	s = send(jsfd, buf1, strlen(buf1), 0);
	print_error(s, "FAILED TO send to j");

	char *buf2 = (char *)malloc(sizeof(char)*100);
	r = recv(jsfd, buf2, 100, 0);
	print_error(r, "FAILED TO RECEIVE from j");
	int status = atoi(buf2);
	
	if (status == 0){
		busy = 0;
		pthread_exit(1);
	}	




	/////




	char * buffer = (char *)malloc(sizeof(char)*100);
	snprintf(buffer, 100, "Round %s-----What score do you want?", type);
	s = send(nsfd, buffer, strlen(buffer), 0);
	print_error(s, "FAILED TO SEND TO CANDIDATE");
	char *readbuffer  = (char *)malloc(sizeof(char)*100);
	r = recv(nsfd, readbuffer, 100, 0);
	print_error(r, "FAILED TO RECEIVE TO CANDIDATE");
	int score = atoi(readbuffer);
	printf("You received score of %d\n", score);





	// sneding to jsfd
	memset(buf1, 0, 100);
	snprintf(buf1, 100, "%d %s %d", id, type, score);

	s = send(jsfd, buf1, strlen(buf1), 0);
	print_error(s, "FAILED TO send to j");


	//ask j server if c already exists
	// struct candidate *c = (struct candidate *)malloc(sizeof(struct candidate));
	// init_candidate(c);
	// c->id = id;
	// c->mode = 1;
	// s = send(jsfd, c, sizeof(c), 0);
	// print_error(s, "failed to send to J");
	// r = recv(jsfd, c, sizeof(c), 0);
	// print_error(r, "failed to receve to J");
	// c->id = id;


	// if(strcmp(type, "r1") == 0){
	// 	c->r1 = score;
	// }
	// else if(strcmp(type, "r2") == 0){
	// 	c->r2 = score;
	// }
	// else if(strcmp(type, "r3") == 0){
	// 	c->r3 = score;
	// }
	// c->mode = 2;
	// s = send(jsfd, c, sizeof(c), 0);
	// print_error(s, "failed to send to J");

	// passing on the fd to the next round.
	if(strcmp(next, "server") != 0)
		send_fd(next_unsfd, nsfd);
	// 
	printf("Done with candidate %d\n", nsfd);
	fflush(stdout);
	busy = 0;
}








void *recv_fds(void * arg){
	int fd_to_read = *(int *)arg;
	printf("Multiplex %s waiting for new clients\n", type);

	int fd = 0;
	while(1){ 
		fd = recv_fd(fd_to_read);
		if(fd == 0)
			break;

		if(client_fd_list_index >= capacity){
			printf("MAXXXXXXXXX\n");
			if (strcmp(next, "server") != 0 ){ // except the multiplex e.
				send_fd(next_unsfd,fd);
				close(fd);
			}
			else{ //multiplex. 

				kill(ser_pid, SIGUSR1);
				printf("Sent a signal to the server asking it to close the counter!!!\n");
			}
			
		}
		else{
			client_fd_list[client_fd_list_index++] = fd;
			pthread_t t;
			pthread_create(&t, NULL, service, (void*)&fd);
		}
	}

	
}


void* init_previous_multiplex_connection(void * arg){

	char * p_sockname = (char*)malloc(sizeof(char)*20);
	strcpy(p_sockname, prev);
	strcat(p_sockname, type);
	strcat(p_sockname, ".sock");
	if(strcmp(prev, "r3") == 0){
		sleep(5);
	}

	printf("Attempting to connect to PREV %s\n", p_sockname);
	prev_usfd = c_socket(AF_LOCAL, SOCK_STREAM, p_sockname);
	printf("%s is waiting to connect to previous connection\n", type);
	_connect(prev_usfd, AF_LOCAL, SOCK_STREAM, 0, p_sockname);
	printf("%s is initiated and back connected to %s\n", type, prev);
	pthread_t pr;
	pthread_create(&pr, NULL, recv_fds, (void*)&prev_usfd);
}

void* init_next_multiplex_connection(void * arg){
	char * n_sockname = (char*)malloc(sizeof(char)*20);
	strcpy(n_sockname, type);
	strcat(n_sockname, next);
	strcat(n_sockname, ".sock");

	next_usfd = s_socket(AF_LOCAL, SOCK_STREAM, 9999, n_sockname);
	printf("%s is waiting for a connection from next\n", type);
	fflush(stdout);
	next_unsfd = ud_accept(next_usfd);
	printf("%s is initiated and forward connected to %s\n", type, next);
}

int main(int argc, char*argv[]){
	if(argc < 4){
		printf("Arguments required: name capacity previous next\n");
	}
	type = argv[1]; 
	prev = argv[3];
	next = argv[4];
	capacity = atoi(argv[2]);
	printf("%s %s %s %s\n", argv[1], argv[2], argv[3], argv[4]);
	init();
	sleep(10);
	if (strcmp(next, "server") != 0){
		printf("NEXT\n");
		pthread_t n;
		pthread_create(&n, NULL, init_next_multiplex_connection, (void*)0);
	}
	if (strcmp(prev, "server") != 0){
		printf("PREV\n");
		pthread_t p;
		pthread_create(&p, NULL, init_previous_multiplex_connection, (void*)0);

	}



	
	pthread_t clients;
	pthread_create(&clients, NULL, recv_fds, (void*)&usfd); //thread to start receiving fds from the main server.


	pthread_join(clients);
	


	printf("Exiting\n");
	fflush(stdout);
	return 0;
}