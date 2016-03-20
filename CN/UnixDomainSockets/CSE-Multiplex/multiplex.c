#include "../../sock_helper.h"


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


int ser_pid; //server pid


void init(){
	client_fd_list_index = 0;
	char * sockname = (char*)malloc(sizeof(char)*20);
	strcpy(sockname, type);
	strcat(sockname, ".sock");
	usfd = s_socket(AF_LOCAL, SOCK_STREAM, 9999, sockname);
	unsfd = ud_accept(usfd);
	printf("Connected to server. Ready to receive fds.\n");

////////// attempting to receive pid of the server
	char *init_msg = (char*)malloc(sizeof(char)*20);
	int r = read(unsfd, init_msg, 100);
	print_error(r, "read failed in reading thread");
	printf("Received server pid via messsage!!! -> %s\n", init_msg);
	ser_pid = atoi(init_msg);

/////////
	fflush(stdout);

}


void *service (void *arg){
	int fd = *(int *)arg;
	struct stat buf;
	int ff = fstat(fd, &buf);
	print_error(ff, "file path error");
	printf("Multiplex %s has started thread to serve inode %ld\n",type,  buf.st_ino);
	char *read_buf;
	read_buf = (char *)malloc(sizeof(char)*100);
	while(1){
		int r = read(fd, read_buf, 100);
		print_error(r, "read from client failed");
		char *msg = (char*)malloc(sizeof(char)*200);
		sprintf(msg, "%s-%s\n", type, read_buf);
		int s = send(fd, msg, strlen(msg), 0);
		print_error(s, "sending from client failed");
		memset(read_buf, 0, strlen(read_buf));
	}
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
	pthread_create(&clients, NULL, recv_fds, (void*)&unsfd); //thread to start receiving fds from the main server.


	pthread_join(clients);
	


	printf("Exiting\n");
	fflush(stdout);
	return 0;
}