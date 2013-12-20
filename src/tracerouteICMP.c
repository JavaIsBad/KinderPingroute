#include "tracerouteICMP.h"
#include "timeuh.h"
#include "tools.h"
#include "const.h"
#include <netinet/ip_icmp.h>
#include <netinet/ip.h>
#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <string.h>
#include <netdb.h>
#include <time.h>

extern int sockfd;
extern struct sockaddr_in destination;
extern long unsigned int nbrReceive;
extern struct timespec tbef;
extern int ttl;
extern unsigned int sizeData;

void tracertICMP(unsigned char*buf, unsigned int size, struct sockaddr_in* doctorWho){
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

void pingerICMP(void){
	unsigned int i;
	int nbs;
	unsigned char packet[MAXPACKET];
	struct icmp *icmpPacket=(struct icmp*) packet;
	unsigned char *data=&packet[8];
	
	memset(packet, 0, MAXPACKET);
	
	icmpPacket->icmp_type = ICMP_ECHO;
	icmpPacket->icmp_code = 0;
	icmpPacket->icmp_cksum = 0;
	icmpPacket->icmp_seq = 0;
	icmpPacket->icmp_id = 0;
	
	for(i=0; i<sizeData; i++){ // remplie le packet
		*data++=i;
	}
	clock_gettime(CLOCK_REALTIME, &tbef);
	icmpPacket->icmp_cksum=checksum((unsigned short*) icmpPacket, 8+sizeData);
	nbs=sendto(sockfd, packet, 8+sizeData, 0, (struct sockaddr*) &destination, sizeof(struct sockaddr));
	
	if(nbs<0 || (unsigned int) nbs< 8+sizeData){
		if(nbs<0)
			perror("sendto :");
		fprintf(stderr, "ping : sendto %s %d chars, achieve %d\n", inet_ntoa(destination.sin_addr), 8+sizeData, nbs);
		fflush(stderr);
	}
}
