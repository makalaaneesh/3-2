// http://www.cs.toronto.edu/~krueger/csc209h/f05/lectures/Week11-Select.pdf


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
#include <sys/select.h> 
#include <sys/time.h>


#define pipe_fd 1
#define fifo_fd 2
#define popen_fd 3

struct pollfd fds[4]; // keyboard, pipe, fifo , popen
int all_fds[4];
int timeout = 200;
char *p;

print_error(int val, char* msg){
	if (val < 0){
		printf("%s error code = %d\n", msg, val);
		exit(1);
	}
}




void init_p5(){
	FILE *p5 = popen("./p5", "w");
	if(p5 == NULL){
		printf("popen error\n");
		exit(1);
	}
	if (fileno(p5) != 1){
		dup2(fileno(p5), 1);
		close(fileno(p5));
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
		sleep(6);
		char * msg = "This is PIPE\n";
		write(pfds[1], msg, strlen(msg));
		close(pfds[1]);
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

void _signal(int signo){// p4
	printf("This is SIGNAL\n");
	fflush(stdout);
}

void init(){
	fds[0].fd = 0;
	fds[0].events = POLLRDNORM;
	if(signal(SIGUSR1, _signal) == SIG_ERR){
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

	// int ret;
	char *main_buf = (char *)malloc(sizeof(char)* 100);
	// while(1){
	// 	// printf("Polling\n");
	// 	ret = poll(fds, 4, timeout);
	// 	if(ret > 0){
	// 		// printf("Event occured\n");
	// 		int i;
	// 		for(i= 0; i<4; i++){
	// 			if(fds[i].revents & POLLRDNORM){

	// 				int r = read(fds[i].fd, main_buf, PIPE_BUF);
	// 				// fgets(main_buf, PIPE_BUF, popens[i]);
	// 				print_error(r,"read failed");
	// 				printf("%s\n", main_buf);
	// 				fflush(stdout);
	// 				memset(main_buf, 0, strlen(main_buf));
	// 			}
	// 		}
	// 	}
	// }
	int max_fd = 0;
	int i;
	for(i=0; i<4;i++){
		all_fds[i] = fds[i].fd;
		if(all_fds[i] >= max_fd)
			max_fd = all_fds[i];

	}
	max_fd += 1; //the max fd that we give to select statement has to be 1 more than the maximum fd among the entire list.

	printf("Max fd is %d\n", max_fd);
	int ret;
	// fd_set rfds;
	struct timeval tv;
	// FD_ZERO(&rfds);
	// for(i = 0; i<4;i++){
	// 	FD_SET(all_fds[i], &rfds);
	// }
	tv.tv_usec = 0;
	tv.tv_sec = 100;
	while(1){
		fd_set rfds;
		FD_ZERO(&rfds);
		for(i = 0; i<4;i++){
			FD_SET(all_fds[i], &rfds);
		}
		ret = select(max_fd , &rfds, NULL, NULL, &tv);
		print_error(ret, "Select statement failed");
		for(i= 0; i<4; i++){
			
			if(FD_ISSET(all_fds[i], &rfds)){
				// printf("Reading from %d\n",all_fds[i] );
				int r = read(all_fds[i], main_buf, PIPE_BUF);
				main_buf[strlen(main_buf)]='\0';
				print_error(r,"read failed");
				if(r > 0){ // AN FD IS ALSO READY ON EOF!!!!!!!!!!!!!!!!!!!!!!!!!
					printf("message:%s\n", main_buf);
					fflush(stdout);
				}
				memset(main_buf, 0, strlen(main_buf));
				FD_CLR(all_fds[i], &rfds);

			}


		}
	}
	
}