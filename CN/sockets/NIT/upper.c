#include "includes.h"


int main()
{
	while(1){
		fflush(stdout);
		char buffer[100];
		bzero(buffer,100);
		scanf("%s",buffer);
		int i=0;
		while(buffer[i])
		{
			buffer[i]=toupper(buffer[i]);
			i++;
		}
		printf("%s\n",buffer);
	}
}