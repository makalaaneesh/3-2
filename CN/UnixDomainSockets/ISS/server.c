#include "../../sock_helper.h"


#define MAX_SERVICES 3

struct service{
	char *name;
	int port;
	int sfd;
	int usfd; // unix domain socket 
	int unsfd; // unix domain socket accepted from the service once started.
	int started;

}services[MAX_SERVICES];

void server_init(){

	services[0].port = 9997;
	services[1].port = 9998;
	services[2].port = 9999;
	services[0].name = (char*)malloc(sizeof(char)*20);
	services[1].name = (char*)malloc(sizeof(char)*20);
	services[2].name = (char*)malloc(sizeof(char)*20);
	strcpy(services[0].name, "upper");
	strcpy(services[1].name, "lower");
	strcpy(services[2].name, "echo");
	services[0].usfd = 0;
	services[1].usfd = 0;
	services[2].usfd = 0;
	services[0].unsfd = 0;
	services[1].unsfd = 0;
	services[2].unsfd = 0;
	services[0].started = 0;
	services[1].started = 0;
	services[1].started = 0;
	// services[0].name = "upper";
	// services[1].name = "lower";
	// services[2].name = "return";

	// signal (ctrl c) to clean up.


	

 	int i; 
 	for( i = 0; i<MAX_SERVICES; i++ ){
 		services[i].sfd = s_socket(AF_INET, SOCK_STREAM, services[i].port, "");
 		char * sockname = (char*)malloc(sizeof(char)*20);
 		strcpy(sockname, services[i].name);
 		strcat(sockname, ".sock");
 		services[i].usfd = s_socket(AF_LOCAL, SOCK_STREAM, 9999, sockname);

 	}
 	printf("%s\n", "Server is initiated.");
 	


}

int get_service_index(int sfd){
	int i, index;
	index = -1;
	for(i = 0; i< MAX_SERVICES; i++){
		if (sfd == services[i].sfd){
			index  = i;
		}
	}
	print_error(index, "No such service!");
	return index;
}


void on_set(int sfd){
	int client_sfd = _accept(sfd);
	int index = get_service_index(sfd);
	printf("Client requested for service %s\n", services[index].name);
	if (services[index].started == 0){
		int pid = fork();
		if (pid == 0){
			execl(services[index].name, services[index].name, services[index].name, (char*)0);
		}
		else{
			services[index].unsfd = _accept(services[index].usfd);
		}
		services[index].started = 0;
	}
	send_fd(services[index].unsfd, client_sfd); // sending the accepted client sfd to the service.
	printf("Sent the fd\n");
}

void * wait_for_incoming_connections(void * arg){
	struct select_fds fds[MAX_SERVICES];
	int len = MAX_SERVICES;
	int i;
	for(i = 0; i< MAX_SERVICES; i++){
		fds[i].fd = services[i].sfd;
		fds[i].type = READ_ARRAY;
		fds[i].function = on_set;
	}
	_select(fds, &len);
}


int main(){
	server_init();
	pthread_t clients;
	pthread_create(&clients, NULL, wait_for_incoming_connections, (void*)0);


	pthread_join(clients);

	printf("Exiting\n");
	fflush(stdout);
	return 0;
}