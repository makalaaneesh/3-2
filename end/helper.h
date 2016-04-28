
#define CONTROLLEN  CMSG_LEN(sizeof(int))



void print_error(int val, char*msg){
	if(val < 0){
		perror(msg);
		exit(val);
	}
}
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
