#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>


int main(){
	char* buf = (char *)malloc(sizeof(char)*100);
	scanf("%[^\n]", buf);
	printf("[final %s]\n", buf);

}