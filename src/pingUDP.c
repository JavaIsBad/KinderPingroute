/**
 * \file pingUDP.c (source code file)
 * \author SCHIMCHOWITSCH PLANTE Raphaël François Guillaume, SCHMITT Maxime Joël
 * \brief Code source pour la fonction pingUDP
 */


#include "tools.h"
#include "const.h"
#include "pingUDP.h"
#include "timeuh.h"
#include <netinet/udp.h>
#include <netinet/ip_icmp.h>
#include <netinet/ip.h>
#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <string.h>


extern u_int16_t LocalPort;
extern u_int16_t DistantPort;
extern char* hostname;
extern int pid;
extern int sockfd;
extern long unsigned int timeMin;
extern long unsigned int timeMax;
extern long unsigned int timeOverall;
extern long unsigned int nbrReceive;
extern long unsigned int nbrSend;
extern long unsigned int limitePing;
extern unsigned int sizeData;
extern struct sockaddr_in destination;
extern struct sockaddr_in moi;
extern unsigned char* buffer;
extern char nameDest[INET6_ADDRSTRLEN];

static struct timespec tbefore;

void pingerUDP(void){
	unsigned char packet[MAXPACKET];
	struct udphdr *head=(struct udphdr*) packet;
	unsigned int i;
	unsigned char *data=&packet[sizeof(struct udphdr)];
	int nbs;
	memset(head, 0, sizeof(struct udphdr));
	head->source=LocalPort;
	head->dest=DistantPort;
	head->len=htons(sizeof(struct udphdr));
	for(i=0; i<sizeData; i++)
		*data++=i;
	head->check=checksum_tcp_udp(IPPROTO_UDP ,moi.sin_addr.s_addr, destination.sin_addr.s_addr, packet, sizeof(struct udphdr));
	nbs=sendto(sockfd, packet, sizeof(struct udphdr), 0, (struct sockaddr*) &destination, sizeof(struct sockaddr));
	if(nbs<0 || (unsigned int) nbs< sizeof(struct udphdr)){
		if(nbs<0)
			perror("sendto :");
		fprintf(stderr, "ping : sendto %s %d chars, achieve %d\n", hostname, 8+sizeData, nbs);
		fflush(stderr);
	}
	clock_gettime(CLOCK_REALTIME, &tbefore);
	nbrSend++;
}

void lirePacketUDP(unsigned char* buf, unsigned int size, struct sockaddr_in* doctorWho){
	struct ip* ip;
	struct icmp* icmpPacket;
	struct timespec tnow;
	struct timespec diff;
	unsigned int ipheaderlen;
	unsigned int timems;
	
	clock_gettime(CLOCK_REALTIME, &tnow);
	ip=(struct ip*) buf;
	ipheaderlen=ip->ip_hl<<2; // passage de bits en octets (*32/8)==> *4 ==> <<2 EZ
	if(size < ipheaderlen + ICMP_MINLEN){
		printf("Vodoo magic happened, the size of the packet must be %d a minima\n", ipheaderlen + ICMP_MINLEN);
		return;
	}
	icmpPacket=(struct icmp*) (buf+ipheaderlen);
	if(icmpPacket->icmp_type!=ICMP_DEST_UNREACH){
		fprintf(stderr, "%d bytes from %s : icmp_type=%d, icmp_code=%d\n", size, inet_ntoa(doctorWho->sin_addr), icmpPacket->icmp_type, icmpPacket->icmp_code);
		return;
	}
	diff=time_diff(&tbefore, &tnow);
	timems=diff.tv_sec*1000+(diff.tv_nsec/1000000);
	timeOverall+=timems;
	if(timems>timeMax)
		timeMax=timems;
	if(timems<timeMin)
		timeMin=timems;
	fprintf(stdout, "message received from %s: time %dms\n", inet_ntoa(doctorWho->sin_addr), timems); 
	nbrReceive++;
}
