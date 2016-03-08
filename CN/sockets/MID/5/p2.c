#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <limits.h>
#include <poll.h>
#include <stropts.h>

#define MAX_CLIENTS 10
#define NO_OF_SERVICES 2

char *fifo = "fifo";
int fd;
char * buf;

print_error(int val, char* msg){
	if (val < 0){
		printf("%s error code = %d\n", msg, val);
		exit(1);
	}
}


void init(){
	buf = (char *)malloc(sizeof(char)*PIPE_BUF);
	fd = open(fifo, O_WRONLY);
	print_error(fd, "opening fifo error");

}

int main(){
	init();
	char * msg = "This is FIFO";
	// sleep(6);
	write(fd, msg, strlen(msg));

}
