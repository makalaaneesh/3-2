#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>



void _signal(int signo){
	printf("%s\n", "exit");
	fflush(stdout);
}

int main(){

	if(signal(SIGUSR1, _signal) == SIG_ERR){
		printf("%s\n", "Error in catching SIGINT");
 	}

	// printf("Starting upper\n");
	// fflush(stdout);
	char *str  = (char *)malloc(sizeof(char)*250);
	size_t write_size;

	while(1){
		
		ssize_t in  = getline(&str, &write_size, stdin);
		
		printf("%s", str);
		fflush(stdout);
		memset(str, 0, sizeof(str));
	}
}