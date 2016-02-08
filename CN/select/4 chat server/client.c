#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <limits.h>
#include <poll.h>



char *serverfifo ;
char myfifo[10];
char *write_buf = NULL;
char *read_buf = NULL;
// char *msg;
size_t write_bufsize;
char *username;
int client_to_server;
int server_to_client;

print_error(int val, char* msg){
	if (val < 0){
		printf("%s error code = %d\n", msg, val);
		exit(1);
	}
}




void* writing_thread(void * arg){
	while(1){
		ssize_t in  = getline(&write_buf, &write_bufsize, stdin);
		print_error(in, "failed to get input");
		// write_buf = (char*)malloc(sizeof(char)*PIPE_BUF);
		// fgets(write_buf,PIPE_BUF,stdin);

		printf("%s", write_buf);

		if(strcmp(write_buf, "exit") == 0){
			printf("Quitting.\n");
			exit(1);
		}

		char msg[PIPE_BUF];
		snprintf(msg, sizeof(msg), "%s: %s",myfifo, write_buf);
		int w = write(client_to_server, msg, sizeof(msg)); //writing to server
		print_error(w, "write to fifo failed");
		fflush(stdout);
		// printf("Wrote to server");

	}
	
}

void * reading_thread(void * arg){
	while(1){

		int r = read(server_to_client, read_buf, PIPE_BUF);
		print_error(r, "read failed");
		printf("Server->%s", read_buf);

		fflush(stdout);
		memset(read_buf, 0, sizeof(read_buf));

	}
	close(server_to_client);
	unlink(myfifo);
}

void client_init(){	

	// msg = (char *)malloc(sizeof(char)*100);


	int pid = getpid();
	sprintf(myfifo, "%d", pid);
	// username = (char*)malloc(sizeof(char)*10);
	// gets(username);
	client_to_server = open(serverfifo, O_WRONLY);
	printf("server fd is %d\n", client_to_server);
	//init to server
	
	// opening fifo that the server will use to reply to the client
	mkfifo(myfifo, 0666);
	server_to_client = open(myfifo, O_RDWR);
	read_buf = (char *)malloc(sizeof(char)*PIPE_BUF);
	printf("myfifo id is%d\n", server_to_client);

}



int main(int argc, char*argv[]){
	if(argc == 1 )
		print_error(-1, "Need client_to_server fifo name as an argument");

	serverfifo = argv[1];
	printf("Fifo to connect to server is %s\n", serverfifo);


	printf("%d\n", PIPE_BUF);
	client_init();

	char init_msg[PIPE_BUF];
	snprintf(init_msg, sizeof(init_msg), "%s %s|\n","init", myfifo);
	printf("Intiial message is %s\n", init_msg);
	int w = write(client_to_server, init_msg, PIPE_BUF);
	print_error(w, "initial write to fifo failed");


	pthread_t wt, rt;
	pthread_create(&wt, NULL, writing_thread, (void*)0);
	pthread_create(&rt, NULL, reading_thread, (void*)0);

	pthread_join(wt,NULL);
	pthread_join(rt,NULL);
	



}