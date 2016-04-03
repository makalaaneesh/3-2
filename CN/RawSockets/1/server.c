#include "../../sock_helper.h"


#define PROTOCOL 100
#define PORT 9999 
// dummy port. POrt isnt actually required.
int rsfd;



int main(){
	rsfd = socket(AF_INET, SOCK_RAW, PROTOCOL);
	print_error(rsfd, "Failed to create raw socket");

	struct sockaddr_in client_addr;
	client_addr.sin_family = AF_INET;
	client_addr.sin_port = htons(PORT);
	//Computer networks are big endian. This means that when little endian computers are going to pass integers over the network (IP addresses for example), they need to convert them to network byte order. Likewise, when the receive integer values over the network, they need to convert them back to their own native representation.
	client_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
	int b = bind(rsfd, (struct sockaddr *)&client_addr, sizeof(client_addr));
	print_error(b, "Failed to bind.");

	// binding on 127.0.0.1. Sending to 127.0.0.2 just to check!! The client will accept anyway because it is not connected or bound to any address.

	client_addr.sin_addr.s_addr = inet_addr("127.0.0.2");
	int to_send = 0;
	while(1){
		printf("%s\n", "Enter 1 to send. 0 to stop");
		scanf("%d", &to_send);
		if(to_send == 0)
			break;
		char *msg = (char*)malloc(sizeof(char)*100);
		strcpy(msg, "Hello world.");
		int s = sendto(rsfd, msg, sizeof(msg), 0 , (struct sockaddr *)&client_addr, sizeof(client_addr));
		// int s = send(rsfd, msg, sizeof(msg), 0 );
		print_error(s, "Failed to send message");

	}

}