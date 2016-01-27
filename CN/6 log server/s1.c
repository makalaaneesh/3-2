#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <limits.h>


print_error(int val, char* msg){
	if (val < 0){
		printf("%s error code = %d\n", msg, val);
		exit(1);
	}
}

char *service_fifo = "s1fifo";
int service_fifo_fd;
char *buf;


int init(){

	mkfifo(service_fifo, 0666);
	service_fifo_fd = open(service_fifo, O_RDWR);

	buf = (char *)malloc(sizeof(char)*PIPE_BUF);
}

int main(){

	init();

	while(1){
		// reading from fifo to which client will connect.

		int r = read(service_fifo_fd, buf, PIPE_BUF);
		print_error(r, "read failed");
		// printing to stdout which will be redirected to log server
		printf("Service 1-> %s", buf);
		fflush(stdout);
		memset(buf, 0, sizeof(buf));
	}

}