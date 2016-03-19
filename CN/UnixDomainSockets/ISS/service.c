#include "../../sock_helper.h"


int usfd;
char * service_name;

void *service (void *arg){
	int fd = *(int *)arg;
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


void * reading_fds(void * arg){
	while(1){
		int fd = recv_fd(usfd);
		printf("Received an fd %d\n", fd);
		pthread_t t;
		pthread_create(&t, NULL, service, (void*)&fd);
	}
}

int main(int argc, char *argv[]){
	if (argc < 1){
		print_error(-1, "Service name arg required");
	}

	service_name = argv[1];

	printf("This is service %s\n", service_name);
	strcat(service_name, ".sock");
	usfd = c_socket(AF_LOCAL, SOCK_STREAM, service_name);
	_connect(usfd, AF_LOCAL, SOCK_STREAM, 0, service_name);
	pthread_t r;
	pthread_create(&r, NULL, reading_fds, (void*)0);

	pthread_join(r, NULL);


}