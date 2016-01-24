#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>


int main(){
	FILE *filter = popen("./filter", "r");
	if(filter == NULL){
		printf("popen error\n");
		exit(1);
	}

	char line[1024];
	while(1){
		fputs("prompt>", stdout);
		fflush(stdout);
		fgets(line, 1024, filter);
		fputs(line, stdout);
	}
	pclose(filter);
	exit(1);
}