#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

int main(){

	int c;
	while((c = getchar()) != EOF){
		if( isupper(c) ){
			c = tolower(c);
		}
		if (putchar(c) == EOF){
			printf("Output error\n");
			exit(1);
		}
		if (c == '\n')
			fflush(stdout);
	}
	exit(0);
}
