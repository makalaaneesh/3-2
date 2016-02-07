#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <limits.h>
#include <poll.h>


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

	// int fd = open("log.txt", O_WRONLY);

	struct pollfd fds[1];
	fds[0].fd = service_fifo_fd;
	fds[0].events = POLLIN;
	int timeout = 500;
	int ret;
	while(1){
		// reading from fifo to which the client is connectedc.
		// ret = poll(fds, 1, timeout);
		// if(ret > 0){
		// 	// printf("Event occured\n");
		// 	int i;
		// 	for(i= 0; i<1; i++){
		// 		if(fds[i].revents & POLLIN){

		// 			int r = read(fds[i].fd, buf, PIPE_BUF);
		// 			print_error(r,"read failed");
		// 			// write(fd, buf, strlen(buf));
		// 			// fgets(log_buf, PIPE_BUF, popens[i]);
		// 			write(1,buf,r);
		// 			//fflush(stdout);
		// 			//memset(buf, 0, strlen(buf));
		// 		}
		// 	}
		// }
		int r = read(service_fifo_fd, buf, PIPE_BUF);
		print_error(r, "read failed");
		// int w = write(fd, buf, strlen(buf));
		// printing to stdout which will be redirected to log server
		printf("Service 1->%s\n", buf);
		fflush(stdout);
		memset(buf, 0, strlen(buf));
	}
	// close(fd);

}