#include "../../sock_helper.h"


#define MAX_LEN 7


int usfd;
int nsfd;
int fd;

void init(){
	usfd = c_socket(AF_LOCAL, SOCK_STREAM);
	_connect(usfd, AF_LOCAL, SOCK_STREAM, 0);

}

int main(int argc, char *argv[1]){
	init();
	char * readbuffer = (char *)malloc(sizeof(char)* MAX_LEN);

	while(1){
		fd = recv_fd(usfd);
		// printf("Received from p1\n");
		int r = read(fd, readbuffer, MAX_LEN);
		if(r <= 0){
			printf("EOF\n");
			break;
		}
		printf("[p2]%s", readbuffer);
		fflush(stdout);
		memset(readbuffer, 0, MAX_LEN);
		send_fd(usfd, fd);
		// printf("sent to p1\n");

	}
	close(fd);
}