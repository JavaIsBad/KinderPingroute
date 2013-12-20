#include "tracerouteUDP.h"
#include "const.h"
#include "tools.h"
#include "timeuh.h"

#include <netinet/udp.h>
#include <netinet/ip_icmp.h>
#include <netinet/ip.h>
#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <string.h>
#include <netdb.h>

extern int sockfd;
extern struct sockaddr_in destination;
extern struct sockaddr_in moi;
struct timespec tbef;
extern int ttl;


extern u_int16_t LocalPort;
extern u_int16_t DistantPort;
static int first=1;

void pingerUDP(void){
	unsigned char packet[MAXPACKET];
	struct udphdr *head=(struct udphdr*) packet;
	int nbs;
	if(first){
		first=!first;
	}
	memset(head, 0, sizeof(struct udphdr));
	head->source=LocalPort;
	LocalPort=htons(ntohs(LocalPort)+1);
	head->dest=DistantPort;
	DistantPort=htons(ntohs(DistantPort)+1);
	head->len=htons(sizeof(struct udphdr));
	head->check=checksum_tcp_udp(IPPROTO_UDP ,moi.sin_addr.s_addr, destination.sin_addr.s_addr, packet, sizeof(struct udphdr));
	nbs=sendto(sockfd, packet, sizeof(struct udphdr), 0, (struct sockaddr*) &destination, sizeof(struct sockaddr));
	if(nbs<0 || (unsigned int) nbs< sizeof(struct udphdr)){
		if(nbs<0)
			perror("sendto :");
		fprintf(stderr, "ping : sending %d chars, achieved %d\n", 8, nbs);
		fflush(stderr);
	}
	clock_gettime(CLOCK_REALTIME, &tbef);
}

void tracertUDP(unsigned char*buf, unsigned int size, struct sockaddr_in* doctorWho){
	struct ip* ip;
	struct timespec tnow;
	struct timespec diff;
	unsigned int ipheaderlen;
	unsigned int timems;
	char host[50];	
	clock_gettime(CLOCK_REALTIME, &tnow);
	ip=(struct ip*) buf;
	ipheaderlen=ip->ip_hl<<2; // passage de bits en octets (*32/8)==> *4 ==> <<2 EZ
	if(size < ipheaderlen + ICMP_MINLEN){
		printf("Vodoo magic happened, the size of the packet must be %d a minima\n", ipheaderlen + ICMP_MINLEN);
		return;
	}
	diff=time_diff(&tbef, &tnow);
	timems=diff.tv_sec*1000+(diff.tv_nsec/1000000);
	if(getnameinfo((struct sockaddr*) doctorWho, sizeof(struct sockaddr_in), host, 50, NULL, 0, 0)<0){ //try to get server name if possible !
		perror("genameinfo");
	}
	fprintf(stdout, "%d %s (%s) time %dms\n", ttl, host, inet_ntoa(doctorWho->sin_addr), timems); 
}

