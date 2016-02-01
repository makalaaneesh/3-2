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

#define pipe_fd 1
#define fifo_fd 2
#define popen_fd 3

struct pollfd fds[4]; // keyboard, pipe, fifo , popen
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
		sleep(5);
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
	init_p5();
	printf("Process id is %d\n", getpid());
	init();
	_pipe();
	_popen();
	_fifo();
	int ret;
	char *main_buf = (char *)malloc(sizeof(char)* 100);
	while(1){
		// printf("Polling\n");
		ret = poll(fds, 4, timeout);
		if(ret > 0){
			// printf("Event occured\n");
			int i;
			for(i= 0; i<4; i++){
				if(fds[i].revents & POLLRDNORM){

					int r = read(fds[i].fd, main_buf, PIPE_BUF);
					// fgets(main_buf, PIPE_BUF, popens[i]);
					print_error(r,"read failed");
					printf("%s\n", main_buf);
					fflush(stdout);
					memset(main_buf, 0, strlen(main_buf));
				}
			}
		}
	}
}