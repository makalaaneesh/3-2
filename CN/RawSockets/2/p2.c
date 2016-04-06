#include "../../sock_helper.h"
#include <netinet/ip.h>
#include <netinet/tcp.h>
#include <netinet/udp.h>
#include <netinet/if_ether.h>
#include <netdb.h>

#define PROTOCOL 100
#define PORT 9999 
// dummy port. POrt isnt actually required.
int rsfd;


void print_ip(struct sockaddr_in *s){
	// socklen_t len;
	// struct sockaddr_storage addr;
	char ipstr[INET6_ADDRSTRLEN];
	// int port;

	// len = sizeof addr;
	// getpeername(sf, (struct sockaddr*)&addr, &len);

	    // port = ntohs(s->sin_port);
	    inet_ntop(AF_INET, &s->sin_addr, ipstr, sizeof ipstr);

	// printf("Peer IP address: %s\n", ipstr);
	// char ip_port[100];
	// sprintf(ip_port, "Connected peer is ->>>>>> %s:%d", ipstr, port);
	// printf("%s\n", ip_port);
	    printf("%s\n", ipstr);
	    fflush(stdout);

	
}


int print_ip_header(char* msg){

	struct sockaddr_in source,dest;
    unsigned short iphdrlen;
         
    struct iphdr *iph = (struct iphdr *)msg;
    iphdrlen =iph->ihl*4;
     


    memset(&source, 0, sizeof(source));
    source.sin_addr.s_addr = iph->saddr;
     
    memset(&dest, 0, sizeof(dest));
    dest.sin_addr.s_addr = iph->daddr;




     
    printf("\n");
    printf("IP Header\n");
    printf("   |-IP Version        : %d\n",(unsigned int)iph->version);
    printf("   |-IP Header Length  : %d DWORDS or %d Bytes\n",(unsigned int)iph->ihl,((unsigned int)(iph->ihl))*4); //DWORD is two 2 words meaning 2*2 bytes
    printf("   |-Type Of Service   : %d\n",(unsigned int)iph->tos);
    printf("   |-IP Total Length   : %d  Bytes(Size of Packet)\n",ntohs(iph->tot_len));
    printf("   |-Identification    : %d\n",ntohs(iph->id));
    // printf("   |-Reserved ZERO Field   : %d\n",(unsigned int)iph->ip_reserved_zero);
    // printf("   |-Dont Fragment Field   : %d\n",(unsigned int)iph->ip_dont_fragment);
    // printf("   |-More Fragment Field   : %d\n",(unsigned int)iph->ip_more_fragment);
    printf("   |-Fragment Offset  : %d\n",(unsigned int)iph->frag_off); 
    printf("   |-TTL      : %d\n",(unsigned int)iph->ttl);
    printf("   |-Protocol : %d\n",(unsigned int)iph->protocol);
    printf("   |-Checksum : %d\n",ntohs(iph->check));
    printf("   |-Source IP        :");
    print_ip(&source);
    printf("   |-Destination IP   :");
    print_ip(&dest);
   
   	int offset =  (iph->ihl*4);
   	// printf("%d\n", offset);
   	return offset;
    
}


int main(){
	rsfd = socket(AF_INET, SOCK_RAW, PROTOCOL);
	print_error(rsfd, "Failed to create raw socket");

	struct sockaddr_in client_addr;
	socklen_t client_len = sizeof(client_addr);
	int to_send = 0;

	char *msg = (char *)malloc(sizeof(char)*100);
	while(1){
		printf("%s\n", "Waiting to receive message");
		int s = recvfrom(rsfd, msg, 100, 0 , (struct sockaddr *)&client_addr, &client_len);
		print_error(s, "Failed to receive");
		// printf("MSG->[%s]\n", msg);
		int p = print_ip_header(msg);
		char *msg_rcvd = msg+p;
		printf("Message received is -> (%s)\n", msg_rcvd);
		fflush(stdout);
		memset(msg, 0 , sizeof(msg));

	}

}