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


char *name;
int num;

int apti_usfd;
int prev_usfd;
int next_usfd;


int connect_server(){
	apti_usfd = socket(AF_LOCAL, SOCK_STREAM, 0);
	print_error(rounds[i].usfd, "error opening socket");

	//setting resuseaddr so that we don't have to constantly wait to the socket to timeout out of it's TIMEWAIT state.
	int set = setsockopt(rounds[i].usfd, SOL_SOCKET, SO_REUSEADDR, &(int){ 1 }, sizeof(int));
	print_error(set, "setsockopt(SO_REUSEADDR) failed");

	char *path = (char*)malloc(sizeof(char)*100);
	strcpy(path, name);
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

}	


int main(){
	num = 1;
	name = (char*)malloc(sizeof(char)*10);
	strcpy(name, "r1");



}