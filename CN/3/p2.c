#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>


int main(){
	char* buf = (char *)malloc(sizeof(char)*10);
	scanf("%[^\n]", buf);
	printf("[2 to 3 -> %s]\n", buf);

}