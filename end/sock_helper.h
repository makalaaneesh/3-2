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



#define CONTROLLEN  CMSG_LEN(sizeof(int))
#define READ_ARRAY 1
#define WRITE_ARRAY 2
#define EXCEPT_ARRAY 3
#define SOCKET_PATH "./unix.sock"


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

int get_peer_ip(int sf, int *id){
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
	*id = port;
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
int c_socket(int domain, int type, char*path){
	s_socket(domain, type, 0 , ""); // to indicate that client has made the call.
}

int s_socket(int domain, int type ,int port, char *path){
	// domain - AF_INET, AF_UNIX
	// type - SOCK_STREAM, SOCK_DGRAM

	int is_server = 1;
	if(port == 0)
		is_server = 0;

	int sfd;

	char buffer[256];

	
	
	sfd = socket(domain, type, 0);
	print_error(sfd, "error opening socket");

	//setting resuseaddr so that we don't have to constantly wait to the socket to timeout out of it's TIMEWAIT state.
	int set = setsockopt(sfd, SOL_SOCKET, SO_REUSEADDR, &(int){ 1 }, sizeof(int));
	print_error(set, "setsockopt(SO_REUSEADDR) failed");

	if (is_server){

		if (domain == AF_INET){
			struct sockaddr_in server_addr;
			// port = atoi(argv[1]);
			server_addr.sin_family = domain;
			server_addr.sin_port = htons(port);
			//Computer networks are big endian. This means that when little endian computers are going to pass integers over the network (IP addresses for example), they need to convert them to network byte order. Likewise, when the receive integer values over the network, they need to convert them back to their own native representation.
			server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

			//binding the socket to the addr
			int b = bind(sfd, (struct sockaddr *)&server_addr, sizeof(server_addr));
			print_error(b, "Failed to bind.");

			listen(sfd, 3);

			printf("%s%d\n", "Server is listening for connections at port ",  port);
		}
		else if (domain == AF_UNIX || domain == AF_LOCAL){
			struct sockaddr_un addr;
			memset(&addr, 0, sizeof(addr));
			addr.sun_family = AF_LOCAL;
			unlink(path);
			strcpy(addr.sun_path, path);

			int b = bind(sfd, (struct sockaddr *)&addr, sizeof(addr));
			print_error(b, "Failed to bind.");

			int l = listen(sfd, 3);
			print_error(l , "Failed to listen");

			printf("%s%s\n", "Server is listening for connections at path ",  path);


		}
	}
	return sfd;

}

int ud_accept(int sfd){
	int nsfd;
	
	
	struct sockaddr_un client_addr; // to store the address of the accepted client.
	socklen_t client_addr_len = sizeof(client_addr); // var to store len of the address of client. 

	nsfd = accept(sfd,(struct sockaddr * )&client_addr, &client_addr_len );
	print_error(nsfd, "Failed in accepting connection");
	printf("Accepted connection.\n");
	// get_peer_ip(nsfd);
	return nsfd;
}

int _accept(int sfd){
	int nsfd;
	
	
	struct sockaddr_in client_addr; // to store the address of the accepted client.
	socklen_t client_addr_len = sizeof(client_addr); // var to store len of the address of client. 

	nsfd = accept(sfd,(struct sockaddr * )&client_addr, &client_addr_len );
	// nsfd = accept(sfd,NULL, NULL);
	print_error(nsfd, "Failed in accepting connection");
	printf("Accepted connection.\n");
	// get_peer_ip(nsfd);
	return nsfd;
}


void _connect(int sfd, int domain, int type, int port, char *path){
	int is_AF_INET = 1;
	if(port == 0)
		is_AF_INET = 0;

	if(is_AF_INET){
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
	else {
		// af_unix unix domain sockets
		struct sockaddr_un addr;
		memset(&addr, 0, sizeof(addr));
		addr.sun_family = AF_LOCAL;
		strcpy(addr.sun_path, path);
		int c = connect(sfd, (struct sockaddr *)&addr, sizeof(addr));
		print_error(c, "could not connect to server.");
		printf("%s\n", "connected to server");
	}
}

void * reading_thread(void * arg){
	int sfd = *(int *)(arg);
	char *read_buf;
	read_buf = (char *)malloc(sizeof(char)*100);
	while(1){

		int r = read(sfd, read_buf, 100);
		print_error(r, "read failed in reading thread");
		if (r >0)
			printf("Server->[%s]\n", read_buf);

		fflush(stdout);
		memset(read_buf, 0, strlen(read_buf));

	}
	
}


int _select(struct select_fds * fds, int *len){
	int ret;
	int i;
	struct timeval tv;
	tv.tv_usec = 0;
	tv.tv_sec = 0;

	int max_fd = 0;
	for(i = 0; i< *len; i++){
		if(fds[i].fd > max_fd)
			max_fd = fds[i].fd;
	}

	while(1){
		fd_set rfds, wfds, efds;
		FD_ZERO(&rfds);
		FD_ZERO(&wfds);
		FD_ZERO(&efds);
		for(i = 0; i< *len; i++){
			switch(fds[i].type){
				case READ_ARRAY: FD_SET(fds[i].fd, &rfds); break;
				case WRITE_ARRAY: FD_SET(fds[i].fd, &wfds); break;
				case EXCEPT_ARRAY: FD_SET(fds[i].fd, &efds); break;
			}
		}

		ret = select(max_fd + 1 , &rfds, &wfds, &efds, &tv);
		print_error(ret, "Select statement failed");


		for(i = 0; i< *len; i++){
			switch(fds[i].type){
				case READ_ARRAY: if (FD_ISSET(fds[i].fd, &rfds)){ fds[i].function(fds[i].fd); } break;
				case WRITE_ARRAY: if (FD_ISSET(fds[i].fd, &wfds)){ fds[i].function(fds[i].fd); } break;
				case EXCEPT_ARRAY: if (FD_ISSET(fds[i].fd, &efds)){ fds[i].function(fds[i].fd); } break;
			}
		}	
		
		
	}


}

//////////////////////////////////////////////////////////////////////////////////////////////


/* size of control buffer to send/recv one file descriptor */


int send_fd(int sfd, int fd_to_send){
	struct msghdr msg;
	struct iovec iov[1]; //http://www.gnu.org/software/libc/manual/html_node/Scatter_002dGather.html
	struct cmsghdr * cmptr = (struct cmsghdr *)malloc(sizeof(struct cmsghdr));
	char buf[2]; /* send_fd()/recv_fd() 2-byte protocol */


	iov[0].iov_base = buf;
	iov[0].iov_len = 2;
	msg.msg_iov = iov;
	msg.msg_iovlen = 1;
	msg.msg_name = NULL;
	msg.msg_namelen = 0;

	if (fd_to_send < 0){
		msg.msg_control = NULL;
		msg.msg_controllen = 0;
		buf[1] = -fd_to_send; // nonzero status means error
		if (buf[1] == 0)
            buf[1] = 1; /* -256, etc. would screw up protocol */
	}
	else{
		cmptr->cmsg_level = SOL_SOCKET;
		cmptr->cmsg_type = SCM_RIGHTS; //to indicate that we are passing access rights. (SCM stands for socket-level control message.)
		cmptr->cmsg_len = CONTROLLEN;
		msg.msg_control = cmptr;
		msg.msg_controllen = CONTROLLEN;
		*(int *)CMSG_DATA(cmptr) = fd_to_send;
		buf[1] = 0; //status is ok

	}
	buf[0] = 0;
	int s = sendmsg(sfd, &msg, 0);
	print_error(s, "Failed to send fd!");

	printf("Sent fd %d\n", fd_to_send);
	return 0; //success
}


int recv_fd(int sfd){
	int nr, newfd, status;
	char *ptr;
	char buf[1024];
	struct iovec iov[1];
	struct msghdr msg;
	struct cmsghdr * cmptr = (struct cmsghdr *)malloc(sizeof(struct cmsghdr));

	status = -1;

	iov[0].iov_base = buf;
	iov[0].iov_len = 2;
	msg.msg_iov = iov;
	msg.msg_iovlen = 1;
	msg.msg_name = NULL;
	msg.msg_namelen = 0;
	msg.msg_control = cmptr;
	msg.msg_controllen = CONTROLLEN;

	nr = recvmsg(sfd, &msg, 0);
	print_error(nr, "Failed to recive fd");


	newfd = *(int *)CMSG_DATA(cmptr);

	printf("Received fd %d\n", newfd);
	return newfd;

}
