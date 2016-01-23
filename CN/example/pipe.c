#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

int main(){
	
	
	int fd[2];
	if (pipe(fd) < 0){
		printf("pipe error");
		exit(1);
	}
	int pid = fork();
	if (pid < 0){
		printf("fork error");
		exit(1);
	}
	else if (pid == 0){ // child
		printf("this is child\n");
		close(fd[1]); // closing the writing end of the child
		char * buf = (char*)malloc(sizeof(char)*10);
		read(fd[0], buf, 12);
		printf("Received message-  %s\n", buf);
	}
	else{
		printf("this is parent\n");
		close(fd[0]); //closing the reading end of the child
		char * buf = (char*)malloc(sizeof(char)*10);
		strcpy(buf, "hello child");
		printf("Sent message-  %s\n", buf);
		write(fd[1],buf,12);

	}

}