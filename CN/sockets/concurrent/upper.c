#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>


int main(){

	char *str  = (char *)malloc(sizeof(char)*250);
	size_t write_size;

	while(1){
		
		ssize_t in  = getline(&str, &write_size, stdin);
		int i;
		for(i = 0; str[i]!='\0'; i++){
			str[i] = toupper(str[i]);
		}
		printf("%s", str);
		fflush(stdout);
		memset(str, 0, sizeof(str));
	}
}