#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>


int main(){

	printf("This is p6\n");
	// fflush(stdout);
	char *str  = (char *)malloc(sizeof(char)*250);
	size_t write_size;

	while(1){
		
		ssize_t in  = getline(&str, &write_size, stdin);
		
		printf("p6->>>>>%s", str);
		fflush(stdout);
		memset(str, 0, sizeof(str));
	}
}