#include "../../sock_helper.h"


int main(){
	int sv[2];
	int s = socketpair(AF_UNIX, SOCK_STREAM, 0, sv);
	print_error(s, "Failed to create socket pair.");
	int pid = fork();
	print_error(pid, "Failed to fork");


	if(pid == 0){ //child
		char * writebuffer = (char *)malloc(sizeof(char)* 100);
		while(1){
			printf("%s\n", "Please enter a message to send to your parent");
			gets(writebuffer);
			int r = write(sv[1], writebuffer, 100);
			print_error(r, "read failed");
			// if (r >0)
			// 	printf("Child->[%s]\n", writebuffer);

			fflush(stdout);
			memset(writebuffer, 0, strlen(writebuffer));
		}
	}
	else{ // parent
		char * readbuffer = (char *)malloc(sizeof(char)* 100);
		while(1){
			int r = read(sv[0], readbuffer, 100);
			print_error(r, "read failed");
			if (r >0)
				printf("Parent->[%s]\n", readbuffer);

			fflush(stdout);
			memset(readbuffer, 0, strlen(readbuffer));
		}

	}

}