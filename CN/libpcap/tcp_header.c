#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pcap.h> 
#include <errno.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netinet/if_ether.h>
#include <netdb.h>
#include <netinet/ip.h>
#include <netinet/tcp.h> 

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


int print_ip_header(const char* msg){

	struct sockaddr_in source,dest;
    unsigned short iphdrlen;
         
    struct iphdr *iph = (struct iphdr *)msg;
    iphdrlen =iph->ihl*4;

    memset(&source, 0, sizeof(source));
    source.sin_addr.s_addr = iph->saddr;
     
    memset(&dest, 0, sizeof(dest));
    dest.sin_addr.s_addr = iph->daddr;

     
    printf("\n");
    printf("****************************IP Header*************************\n");
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
   
   	int offset =  (iph->ihl*4); // stored as DWORDS
   	// printf("%d\n", offset);
   	return offset;
    
}


void print_tcp_packet(const u_char *buffer){
	unsigned short iphdrlen;
	struct iphdr *iph = (struct iphdr *)( buffer  + sizeof(struct ethhdr) );
	iphdrlen = iph->ihl*4;

	struct tcphdr *tcph = (struct tcphdr *)(buffer + sizeof(struct ethhdr) + iphdrlen);

	int tcphdrlen = tcph->doff*4;

	printf("\n");
    printf("****************************TCP Header*************************\n");


    printf("   |-Source Port      : %u\n",ntohs(tcph->source)); 
    printf("   |-Destination Port : %u\n",ntohs(tcph->dest));
    printf("   |-Sequence Number    : %u\n",ntohl(tcph->seq));
    printf("   |-Acknowledge Number : %u\n",ntohl(tcph->ack_seq));
    printf("   |-Header Length      : %d DWORDS or %d BYTES\n" ,(unsigned int)tcph->doff,(unsigned int)tcph->doff*4);
    printf("   |-Urgent Flag          : %d\n",(unsigned int)tcph->urg);
    printf("   |-Acknowledgement Flag : %d\n",(unsigned int)tcph->ack);
    printf("   |-Push Flag            : %d\n",(unsigned int)tcph->psh);
    printf("   |-Reset Flag           : %d\n",(unsigned int)tcph->rst);
    printf("   |-Synchronise Flag     : %d\n",(unsigned int)tcph->syn);
    printf("   |-Finish Flag          : %d\n",(unsigned int)tcph->fin);
    printf("   |-Window         : %d\n",ntohs(tcph->window));
    printf("   |-Checksum       : %d\n",ntohs(tcph->check));
    printf("   |-Urgent Pointer : %d\n",tcph->urg_ptr);
    printf("\n");
    printf("                        DATA Dump                         ");
    printf("\n");
    const char *data = buffer + sizeof(struct ethhdr) + iphdrlen + tcphdrlen;
    printf("%s\n", data);
}

char *dev;
char *net; // network address
char *mask; //network mask address
int ret;
char errbuf[PCAP_ERRBUF_SIZE];
bpf_u_int32 netp; // IP
bpf_u_int32 maskp; // subnet mask
struct in_addr addr;
pcap_t *handle;


void print_error(int val, char*buf){
	if(val < 0){
		printf("%s\n", buf);
		exit(1);
	}
}




void process_packet(u_char *args, const struct pcap_pkthdr *header, const u_char *packet){
	//struct pcap_pkthdr {
	//	struct timeval ts; /* time stamp */
	//	bpf_u_int32 caplen; /* length of portion present */
	//	bpf_u_int32 len; /* length this packet (off wire) */
	//};

	/* packet points to the first byte of chunk of data containing the entire packet.
	A collection of structures. a TCP/IP packet would have an Ethernet header, an IP header, a TCP header,
	 and lastly, the packet's payload */

	int size = header->len;

	
	struct iphdr *iph = (struct iphdr *)(packet + sizeof(struct ethhdr));
	// printf("   |-Protocol : %d\n",(unsigned int)iph->protocol);
	switch((unsigned int)iph->protocol){
		case 6:
			print_ip_header(packet + sizeof(struct ethhdr));
			print_tcp_packet(packet);
			printf("A TCP packet has been sniffed.\n");
			printf("%s\n", "==============================================");
			break;
		default:
			// printf("A packet with protocol other than TCP has been sniffed. Not supported yet.\n");
			break;
	}


	




}


void open_device(){
	/*
		pcap_t *pcap_open_live(char *device, int snaplen, int promisc, int to_ms,
	    char *ebuf)

	    snaplen - max bytes to be capture
	    promisc - promiscuous mode (sniffs all traffic on the wire)
	    to_ms - timeout in milliseconds 
	    ebuf - buffer for error messages

	*/
	handle = pcap_open_live(dev, BUFSIZ, 1, 1000, errbuf);
	if (handle == NULL) {
		fprintf(stderr, "Couldn't open device %s: %s\n", dev, errbuf);
		exit(2);
	}
	if (pcap_datalink(handle) != DLT_EN10MB) {
		fprintf(stderr, "Device %s doesn't provide Ethernet headers - not supported\n", dev);
		exit(2);
	}

	/*
	 int pcap_loop(pcap_t *p, int cnt, pcap_handler callback, u_char *user)
	 cnt- how many packets to sniff. negative value means sniff till an error occurs
	 callback value

	*/
	pcap_loop(handle, -1, process_packet, NULL);



}

int main(int argc, char **argv){
	if(argc < 2){
		printf("Usage: ./program <network-device>\n");
		exit(1);
	}
	dev = argv[1];


	printf("DEV: %s\n",dev);


	/* ask pcap for the network address and mask of the device */
	ret = pcap_lookupnet(dev, &netp, &maskp, errbuf);
	print_error(ret, errbuf);


	/* making it human readable */
	addr.s_addr = netp;
	net = inet_ntoa(addr);


  	printf("NET: %s\n",net);

	addr.s_addr = maskp;
	mask = inet_ntoa(addr);

	if(mask == NULL)
	{
		perror("inet_ntoa");
		exit(1);
	}

	printf("MASK: %s\n",mask);


	open_device();
}