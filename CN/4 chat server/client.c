#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <limits.h>



char *serverfifo = "server";
char *buf = NULL;
size_t bufsize;
char *username;


print_error(int val, char* msg){
	if (val < 0){
		printf("%s\n", msg);
		exit(1);
	}
}

int main(){

	// username = (char*)malloc(sizeof(char)*10);
	// gets(username);

	int fd = open(serverfifo, O_WRONLY);

	while(1){
		ssize_t in  = getline(&buf, &bufsize, stdin);
		print_error(in, "error reading!\n");
		// printf("%s", buf);

		int w = write(fd, buf, bufsize);
		print_error(w, "write to fifo failed");
		buf = NULL;


	}
	



}