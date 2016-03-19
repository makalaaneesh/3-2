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
#include <stropts.h>
#include <poll.h>

#define READ_ARRAY 1
#define WRITE_ARRAY 2
#define EXCEPT_ARRAY 3

struct select_fds{
	int type;
	int fd;
	void (*function)(int fd);
};


void print_error(int val, char*msg){
	if(val < 0){
		perror(msg);
		exit(val);
	}
}

void get_peer_ip(int sf){
	socklen_t len;
	struct sockaddr_storage addr;
	char ipstr[INET6_ADDRSTRLEN];
	int port;

	len = sizeof addr;
	getpeername(sf, (struct sockaddr*)&addr, &len);

	    struct sockaddr_in *s = (struct sockaddr_in *)&addr;
	    port = ntohs(s->sin_port);
	    inet_ntop(AF_INET, &s->sin_addr, ipstr, sizeof ipstr);

	// printf("Peer IP address: %s\n", ipstr);
	char ip_port[100];
	sprintf(ip_port, "Connected peer is ->>>>>> %s:%d", ipstr, port);
	printf("%s\n", ip_port);
}


void get_my_ip(int sf){
	socklen_t len;
	struct sockaddr_storage addr;
	char ipstr[INET6_ADDRSTRLEN];
	int port;

	len = sizeof addr;
	getsockname(sf, (struct sockaddr*)&addr, &len);

	    struct sockaddr_in *s = (struct sockaddr_in *)&addr;
	    port = ntohs(s->sin_port);
	    inet_ntop(AF_INET, &s->sin_addr, ipstr, sizeof ipstr);

	// printf("Peer IP address: %s\n", ipstr);
	char ip_port[100];
	sprintf(ip_port, "My socket address is ->>>>>> %s:%d", ipstr, port);
	printf("%s\n", ip_port);
}


// socket call for a client
int _socket(int domain, int type){
	_socket(domain, type, 0); // to indicate that client has made the call.
}

int _socket(int domain, int type ,int port){
	// domain - AF_INET, AF_UNIX
	// type - SOCK_STREAM, SOCK_DGRAM

	int is_server = 1;
	if(port == 0)
		is_server = 0;

	int sfd;
	int client_addr_len; // var to store len of the address of client. 
	char buffer[256];

	struct sockaddr_in server_addr;
	
	sfd = socket(domain, int type, 0);
	print_error(sfd, "error opening socket");

	//setting resuseaddr so that we don't have to constantly wait to the socket to timeout out of it's TIMEWAIT state.
	int set = setsockopt(sfd, SOL_SOCKET, SO_REUSEADDR, &(int){ 1 }, sizeof(int));
	print_error(set, "setsockopt(SO_REUSEADDR) failed");

	if (is_server){
		// port = atoi(argv[1]);
		server_addr.sin_family = domain;
		server_addr.sin_port = htons(port);
		//Computer networks are big endian. This means that when little endian computers are going to pass integers over the network (IP addresses for example), they need to convert them to network byte order. Likewise, when the receive integer values over the network, they need to convert them back to their own native representation.
		server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

		//binding the socket to the addr
		int b = bind(sfd, (struct sockaddr *)&server_addr, sizeof(server_addr));
		print_error(b, "Failed to bind.");

		listen(sfd, 3);

		printf("%s\n", "Server is listening for connections.");
	}
	return sfd;

}

int _accept(int sfd){
	int nsfd;
	int client_addr_len; // var to store len of the address of client. 
	struct sockaddr_in client_addr; // to store the address of the accepted client.

	nsfd = accept(sfd,(struct sockaddr * )&client_addr, &client_addr_len );
	print_error(nsfd, "Failed in accepting connection");
	printf("Accepted connection.\n");
	get_peer_ip();
	return nsfd;
}


int _connect(int sfd, int domain, int type, int port){
	struct sockaddr_in server_addr;
	// port = atoi(argv[1]);
	server_addr.sin_family = domain;
	server_addr.sin_port = htons(port);
	//Computer networks are big endian. This means that when little endian computers are going to pass integers over the network (IP addresses for example), they need to convert them to network byte order. Likewise, when the receive integer values over the network, they need to convert them back to their own native representation.
	server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");


	int c = connect(sfd, (struct sockaddr *)&server_addr, sizeof(server_addr));
	print_error(c, "could not connect to server.");
	printf("%s\n", "connected to server");
}

void * reading_thread(void * arg){
	int sfd = *(int *)(arg);
	char *read_buf;
	read_buf = (char *)malloc(sizeof(char)*100);
	while(1){

		int r = read(service_sfd, read_buf, 100);
		print_error(r, "read failed in reading thread");
		if (r >0)
			printf("Server->[%s]\n", read_buf);

		fflush(stdout);
		memset(read_buf, 0, strlen(read_buf));

	}
	close(service_sfd);
}


int _select(struct *select_fds, int len){
	int ret;
	int i;
	struct timeval tv;
	tv.tv_usec = 0;
	tv.tv_sec = 0;

	int max_fd = 0;
	for(i = 0; i< len; i++){
		if(select_fds[i].fd > max_fd)
			max_fd = select_fds[i].fd;
	}

	while(1){
		fd_set rfds, wfds, efds;
		FD_ZERO(&rfds);
		FD_ZERO(&wfds);
		FD_ZERO(&efds);
		for(i = 0; i< len; i++){
			switch(select_fds[i].type){
				case READ_ARRAY: FD_SET(select_fds[i].fd, &rfds); break;
				case WRITE_ARRAY: FD_SET(select_fds[i].fd, &wfds); break;
				case EXCEPT_ARRAY: FD_SET(select_fds[i].fd, &efds); break;
			}
		}

		ret = select(max_fd + 1 , &rfds, &wfds, &efds, &tv);
		print_error(ret, "Select statement failed");
		

		for(i = 0; i< len; i++){
			switch(select_fds[i].type){
				case READ_ARRAY: if (FD_ISSET(select_fds[i].fd, &rfds)){ function(select_fds[i].fd); } break;
				case WRITE_ARRAY: if (FD_ISSET(select_fds[i].fd, &wfds)){ function(select_fds[i].fd); } break;
				case EXCEPT_ARRAY: if (FD_ISSET(select_fds[i].fd, &efds)){ function(select_fds[i].fd); } break;
			}
		}	
		
		
	}


}