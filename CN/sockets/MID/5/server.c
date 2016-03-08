#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <limits.h>
#include <poll.h>
#include <signal.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include"../../../helper.h"


#define STDIN 0
#define STDOUT 1
#define TOKEN 123321
#define pipe_fd 1
#define fifo_fd 2
#define popen_fd 3
#define SOCKET_FD 4

struct pollfd fds[10]; // keyboard, pipe, fifo , popen
int poll_max;
int timeout = 200;
struct tem{
	int nsfd;
	int pid;
};
struct tem nsfds[100];
int nsfdindex;
char *p;
int *pid;

int nsfdqueue[100];
int nsfdqueue_index;
int nsfdqueue_index2;

print_error(int val, char* msg){
	if (val < 0){
		printf("%s error code = %d\n", msg, val);
		exit(1);
	}
}



void _pipe(){//p1
	int pfds[2];
	pipe(pfds);
	int pid = fork();
	print_error(pid, "failed to fork");
	if (pid == 0){ //child
		printf("In child\n");
		close(pfds[0]);
		sleep(10);
		char * msg = "This is PIPE";
		write(pfds[1], msg, strlen(msg));
		exit(0);
	}
	else{
		close(pfds[1]);
		fds[pipe_fd].fd = pfds[0];
		fds[pipe_fd].events = POLLRDNORM;

	}
	
}


void _fifo(){//p2
	char * fifo = "fifo";
	mkfifo(fifo, 0666);
	int ffd = open(fifo, O_RDWR);
	fds[fifo_fd].fd = ffd;
	fds[fifo_fd].events = POLLRDNORM;
}

void _popen(){//p3
	FILE * p3 = popen("./p3", "r");
	fds[popen_fd].fd = fileno(p3);
	fds[popen_fd].events = POLLRDNORM;
}

int spawn(int nsfd){ 
	printf("Forwarding %d connection to echo server \n", nsfd);
	
	
	char *main_buf;
	main_buf = (char*)malloc(sizeof(char)*20);
	sprintf(main_buf, "%s", "exit");
	printf("main_buf value is %s\n", main_buf);
	int s = send(nsfd, main_buf, strlen(main_buf), 0);

	print_error(s, "Failed to send message to all nsfds");
	// sleep(5);
	int pid = fork();
	print_error(pid, "fork failed");
	if (pid == 0){//child

		// printf("1-%d\n", nsfd);
		if(nsfd != STDIN){
			printf("Duping input\n");
			if(dup2(nsfd, STDIN)!= STDIN){
				print_error(-1, "Failed to dup2 stdin");
			}

		}
		// printf("2-%d\n", nsfd);
		if(nsfd != STDOUT){
			printf("Duping output\n");
			if(dup2(nsfd, STDOUT)!= STDOUT){
				print_error(-1, "Failed to dup2 stdout");
			}

		}
		close(nsfd);

		
		execl("echo", "echo", (char*)0);



	}
	else{
		//pid is the pid of the 
		// close(nsfd);
		return pid;
	}
}
void __signal(int signo){
	printf("Signal that echo server has ended\n");
	int to_search = *(pid);
	int index = -1;
	int i;
	for(i = 0; i<nsfdindex; i++){
		if(nsfds[i].pid == to_search){
			index = i;
			break;
		}
	}
	if(index == -1){
		printf("%s\n", "Couldnt find the same fd in the array. Something is majorly wrong.");
	}
	nsfdqueue[nsfdqueue_index++] = nsfds[index].nsfd;

}

void _signal(int signo){// p4
	printf("This is SIGNAL\n");
	if (nsfdqueue_index2 >= nsfdqueue_index)
		return;
	int nsfd_to_process = nsfdqueue[nsfdqueue_index2++];
	int forked_pid = spawn(nsfd_to_process);
	nsfds[nsfdindex].nsfd = nsfd_to_process;
	nsfds[nsfdindex].pid = forked_pid;
	nsfdindex++;
	nsfdqueue[nsfdqueue_index2-1] = -1;
	fflush(stdout);
}

void init(){

	int shmid;
	shmid = allocateSharedMemory(sizeof(int), TOKEN);
	pid = (int *) mapSharedMemory(shmid);

	struct sockaddr_in server_addr;

	int sfd;
	sfd = socket(AF_INET, SOCK_STREAM, 0);
	print_error(sfd, "error opening socket");
	//setting resuseaddr so that we don't have to constantly wait to the socket to timeout out of it's TIMEWAIT state.
	int set = setsockopt(sfd, SOL_SOCKET, SO_REUSEADDR, &(int){ 1 }, sizeof(int));
	print_error(set, "setsockopt(SO_REUSEADDR) failed");


	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(10000);
	server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

	int b = bind(sfd, (struct sockaddr *)&server_addr, sizeof(server_addr));
	print_error(b, "Failed to bind.");
	listen(sfd, 3);	
	printf("listening at port %d at socket fd %d\n", 10000, sfd);
	fflush(stdout);

	fds[SOCKET_FD].fd = sfd;
	fds[SOCKET_FD].events = POLLRDNORM;
	nsfdqueue_index = 0;
	nsfdqueue_index2 = 0;
	nsfdindex = 0;
	printf("%s\n", "Server is initiated.");

	poll_max = 5;
	fds[0].fd = 0;
	fds[0].events = POLLRDNORM;
	if(signal(SIGUSR1, _signal) == SIG_ERR){
		printf("%s\n", "Error in catching SIGINT");
	}
	if(signal(SIGUSR2, __signal) == SIG_ERR){
		printf("%s\n", "Error in catching SIGINT");
	}
}


int main(){
	// init_p5();
	printf("Process id is %d\n", getpid());
	init();
	_pipe();
	_popen();
	_fifo();
	int ret;
	// char *main_buf = (char *)malloc(sizeof(char)* PIPE_BUF);
	int max_fd = 0;
	struct sockaddr_in client_addr;
	int client_addr_len;

	char buffer[256];
	int nsfd;
	char *readbuffer, *writebuffer;
	readbuffer = (char *)malloc(sizeof(char)*100);
	writebuffer = (char *)malloc(sizeof(char)*100);
	char *main_buf = (char *)malloc(sizeof(char)* 100);
	while(1){
		// printf("Polling\n");
		ret = poll(fds, poll_max, timeout);
		if(ret > 0){
			// printf("Event occured\n");
			int i;
			for(i= 0; i<poll_max; i++){
				if (i == SOCKET_FD && ((fds[i].revents & POLLRDNORM))){

					int nsfd;
					nsfd = accept(fds[i].fd,(struct sockaddr * )&client_addr, &client_addr_len );
					print_error(nsfd, "Failed in accepting connectionn");
					printf("Accepted connection from %d\n", inet_ntoa(client_addr.sin_addr));
					nsfdqueue[nsfdqueue_index++] = nsfd;
					// raise(SIGUSR1);
				}
				else if(fds[i].revents & POLLRDNORM){

					int r = read(fds[i].fd, main_buf, PIPE_BUF);
					// fgets(main_buf, PIPE_BUF, popens[i]);
					print_error(r,"read failed");
					printf("%s\n", main_buf);
					int j;
					printf("%d:%d\n",nsfdqueue_index2,nsfdqueue_index );
					for(j = nsfdqueue_index2; j< nsfdqueue_index; j++){
						if (nsfdqueue[j] == -1)
							continue;
						printf("%d\n", nsfdqueue[j]);
						int s = send(nsfdqueue[j], main_buf, strlen(main_buf), 0);
						print_error(s, "Failed to send message to all nsfds");
						printf("%s\n", "Sent message to all nsfds");
					}
					fflush(stdout);
					memset(main_buf, 0, strlen(main_buf));
				}
			}
		}
	}
}