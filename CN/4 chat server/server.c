#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <limits.h>

#define MAX_CLIENTS 10
char *fifo = "server";
char *buf;

char *client_fifos[MAX_CLIENTS];
int client_fifos_fds[MAX_CLIENTS];

print_error(int val, char* msg){
	if (val < 0){
		printf("%s\n", msg);
		exit(1);
	}
}


int server_init(){
	buf = (char *)malloc(sizeof(char)*PIPE_BUF);
	mkfifo(fifo, 0666);
	int fd = open(fifo, O_RDWR); //read write so that the fifo does not get closed when there are no clients
	print_error(fd, "failed to open fifo");
	return fd;
}

int main(){

	int fd = server_init();
	printf("Server ON.\n");

	while(1){

		int r = read(fd, buf, PIPE_BUF);
		print_error(r, "read failed");
		
		printf("Server: %s", buf);
		memset(buf, 0, sizeof(buf));

	}
	close(fd);
	unlink(fifo);
	



}