#include "../../sock_helper.h"


#define MAX_LEN 7


int usfd;
int nsfd;
int fd;
void init(){
	usfd = s_socket(AF_LOCAL, SOCK_STREAM, 9999, "unix.sock");
	nsfd = _accept(usfd);
}

int main(int argc, char *argv[1]){
	init();
	printf("%s\n", argv[1]);
	fd = open(argv[1], O_RDONLY);
	char * readbuffer = (char *)malloc(sizeof(char)* MAX_LEN);

	while(1){

		int r = read(fd, readbuffer, MAX_LEN);
		if(r <= 0){
			printf("EOF\n");
			break;
		}
		printf("[p1]%s", readbuffer);
		fflush(stdout);
		memset(readbuffer, 0, MAX_LEN);
		send_fd(nsfd, fd);
		// printf("Sent to p2\n");
		fd = recv_fd(nsfd);
		// printf("Received from p2\n");

	}
	close(fd);
}