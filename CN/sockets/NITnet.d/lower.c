#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#define SHMTOKEN 123123
#define SERVICE lower
#define MAX_SERVICES 3
#include"../../helper.h"

struct service_client_info
{
	char name[10];
	int service_port;
	int max_clients;
	int current_clients;
}**shm;


void reduce_client_count_and_exit(){
	int shmid;
	shmid = allocateSharedMemory(sizeof(struct service_client_info*)*MAX_SERVICES, SHMTOKEN);
	shm = (struct service_client_info **) mapSharedMemory(shmid);
	int i;
	int index = -1;
	for (i = 0; i< 3; i++){
		if (strcmp(shm[i]->name, "lower") == 0){
			index = i;
			break;
		}
	}
	if (index == -1)
		exit(1);
	else{
		shm[index]->current_clients--;
		printf("Exiting\n");
		exit(1);
	}
}

int main(){

	char *str  = (char *)malloc(sizeof(char)*250);
	size_t write_size;

	while(1){
		
		ssize_t in  = getline(&str, &write_size, stdin);
		if(strcmp(str, "exit\n")==0)
			reduce_client_count_and_exit();
		int i;
		for(i = 0; str[i]!='\0'; i++){
			str[i] = tolower(str[i]);
		}
		printf("[%s]", str);
		fflush(stdout);
		memset(str, 0, sizeof(str));
	}
}
	
// int main()
// {
// 	while(1)
// 	{
// 		fflush(stdout);
// 		char buffer[100];
// 		bzero(buffer,100);
// 		scanf("%s",buffer);
// 		int i=0;
// 		while(buffer[i])
// 		{
// 			buffer[i]=tolower(buffer[i]);
// 			i++;
// 		}
// 		printf("%s\n",buffer);
// 	}
// }
