#include "../../sock_helper.h"



int sfd;
int usfd;
#define PORT 9999
#define MAX_CLIENTS 10
#define SOCKPATH "backup.sock"
int client_fd_list[MAX_CLIENTS];
int client_fd_list_index;

void switch_to_backup(int signo){
	int unsfd = _accept(usfd);
	send_fd(unsfd, sfd);
	int i;
	for(i = 0; i< client_fd_list_index; i++){
		send_fd(unsfd, client_fd_list[i]);
		sleep(2);
	}
	// send_fd(unsfd, 100);
	exit(1);
}


void init(){
	client_fd_list_index = 0;
	
}
void server_init(){
	
	if(signal(SIGUSR1, switch_to_backup) == SIG_ERR){
   		printf("%s\n", "Error in catching SIGUSR1");
 	}
	sfd = s_socket(AF_INET, SOCK_STREAM, PORT, "");
 	printf("%s\n", "Server is initiated.");
 	
 	// signal handler
 	
 	char *sockname = (char *)malloc(sizeof(char)*100);
 	strcpy(sockname, SOCKPATH);
 	usfd = s_socket(AF_LOCAL, SOCK_STREAM, 9999, sockname);
}

void *service (void *arg){
	int fd = *(int *)arg;
	struct stat buf;
	int ff = fstat(fd, &buf);
	print_error(ff, "file path error");
	printf("Started thread to serve inode %ld\n", buf.st_ino);
	char *read_buf;
	read_buf = (char *)malloc(sizeof(char)*100);
	while(1){
		int r = read(fd, read_buf, 100);
		print_error(r, "read from client failed");
		int s = send(fd, read_buf, strlen(read_buf), 0);
		print_error(s, "sending from client failed");
		memset(read_buf, 0, strlen(read_buf));
	}
}




void * wait_for_incoming_connections(void * arg){
	while(1){
		int nsfd = _accept(sfd);
		client_fd_list[client_fd_list_index++] = nsfd;
		pthread_t t;
		pthread_create(&t, NULL, service, (void*)&nsfd);
	}
}



//code the backup server will be executing
void backup_server(){
	usfd = c_socket(AF_LOCAL, SOCK_STREAM, "backup.sock");
	_connect(usfd, AF_LOCAL, SOCK_STREAM, 0, "backup.sock");

	sfd = recv_fd(usfd); // backup server will first receive the main sfd.
	pthread_t clients;
	pthread_create(&clients, NULL, wait_for_incoming_connections, (void*)0);

	int fd = 0;
	while(1){ // 100 will be used to indcate the end of transfering fds.
		fd = recv_fd(usfd);
		if(fd == 0)
			break;
		client_fd_list[client_fd_list_index++] = fd;
		pthread_t t;
		pthread_create(&t, NULL, service, (void*)&fd);
	}

	pthread_join(clients);
}



int main(int argc, char*argv[]){
	char *type = argv[1]; //main or backup
	int is_backup = 0;
	if (strcmp(type, "backup") == 0)
		is_backup = 1;

	init();

	if(!is_backup){
		server_init();
		pthread_t clients;
		pthread_create(&clients, NULL, wait_for_incoming_connections, (void*)0);
		pthread_join(clients);
	}
	else{
		backup_server();
	}

	printf("Exiting\n");
	fflush(stdout);
	return 0;
}