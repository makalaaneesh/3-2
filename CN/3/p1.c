#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>


int main(){
	char* buf = (char *)malloc(sizeof(char)*10);
	strcpy(buf, "hello");
	printf("[1 to 2 -> %s]\n", buf);

}