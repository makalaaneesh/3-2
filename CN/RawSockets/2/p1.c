#include "../../sock_helper.h"
#include <netinet/ip.h>
#include <netinet/tcp.h>
#include <netinet/udp.h>
#include <netinet/if_ether.h>
#include <netdb.h>

#define PROTOCOL 100
#define PORT 9999 
// dummy port. POrt isnt actually required.
#define PACKET_MAX_LEN 8192
int rsfd;


int construct_IP(char * buffer, char *msg){

	char *dst_addr="127.0.0.3";
    char *src_addr="127.0.0.1";


	struct iphdr *iph = (struct iphdr *)buffer;
	iph->ihl = 5; // 5 DWORDS or 20 bytes
	iph->version = 4;
	iph->tos = 0x0;
	iph->frag_off = htons(0x4000); //Don't fragment
	iph->ttl = 64;
	iph->tot_len = htons(sizeof(struct iphdr) + sizeof(msg));
	iph->protocol = PROTOCOL;
	iph->saddr = inet_addr(src_addr);
	iph->daddr = inet_addr(dst_addr);

	char *msg_to_send = buffer + (iph->ihl * 4);
	strcpy(msg_to_send, msg);
	return iph->tot_len;

}


int main(){
	rsfd = socket(AF_INET, SOCK_RAW, PROTOCOL);
	print_error(rsfd, "Failed to create raw socket");

	struct sockaddr_in client_addr;
	client_addr.sin_family = AF_INET;
	client_addr.sin_port = htons(PORT);
	//Computer networks are big endian. This means that when little endian computers are going to pass integers over the network (IP addresses for example), they need to convert them to network byte order. Likewise, when the receive integer values over the network, they need to convert them back to their own native representation.
	// client_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
	// int b = bind(rsfd, (struct sockaddr *)&client_addr, sizeof(client_addr));
	// print_error(b, "Failed to bind.");

	// binding on 127.0.0.1. Sending to 127.0.0.2 just to check!! The client will accept anyway because it is not connected or bound to any address.

	int optval = 1;
	int o = setsockopt(rsfd, IPPROTO_IP, IP_HDRINCL, &optval, sizeof(optval));
	print_error(o, "Failed to set ip header include");

	client_addr.sin_addr.s_addr = inet_addr("127.0.0.2");
	int to_send = 0;
	char *buffer = (char*)malloc(sizeof(char)*PACKET_MAX_LEN);
	char *msg = (char*)malloc(sizeof(char)*100);
	while(1){
		printf("%s\n", "Enter message to send.");
		scanf("%s", msg);
		if (strcmp(msg, "exit") == 0)
			break;
		// if(to_send == 0)
		// 	break;
		
		// strcpy(msg, "Hello world.");
		int tot = construct_IP(buffer, msg);

		int s = sendto(rsfd, buffer,tot , 0 , (struct sockaddr *)&client_addr, sizeof(client_addr));
		// int s = send(rsfd, buffer, sizeof(buffer), 0 );
		print_error(s, "Failed to send message");

	}

}