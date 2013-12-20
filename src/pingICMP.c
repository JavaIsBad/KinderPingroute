/**
 * \file pingICMP.c (source code file)
 * \author SCHIMCHOWITSCH PLANTE Raphaël François Guillaume, SCHMITT Maxime Joël
 * \brief Code source pour la fonction pingICMP
 */

#include "const.h"
#include "pingICMP.h"
#include "timeuh.h"
#include "tools.h"
#include <netinet/ip_icmp.h>
#include <netinet/ip.h>
#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <string.h>


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
extern unsigned char* buffer;
extern char nameDest[INET6_ADDRSTRLEN];


void pingerICMP(void){
	unsigned int i;
	int nbs;
	unsigned char packet[MAXPACKET];
	struct icmp *icmpPacket=(struct icmp*) packet;
	struct timespec *timee=(struct timespec*) &packet[8]; // partie data de icmp
	unsigned char *data=&packet[8+sizeof(struct timespec)];
	
	memset(packet, 0, MAXPACKET);
	
	icmpPacket->icmp_type = ICMP_ECHO;
	icmpPacket->icmp_code = 0;
	icmpPacket->icmp_cksum = 0;
	icmpPacket->icmp_seq = nbrSend;
	icmpPacket->icmp_id = pid;
	
	for(i=(unsigned int) sizeof(struct timespec); i<sizeData; i++){
		*data++=i;
	}
	clock_gettime(CLOCK_REALTIME, timee);
	icmpPacket->icmp_cksum=checksum((unsigned short*) icmpPacket, 8+sizeData);
	nbs=sendto(sockfd, packet, 8+sizeData, 0, (struct sockaddr*) &destination, sizeof(struct sockaddr));
	
	if(nbs<0 || (unsigned int) nbs< 8+sizeData){
		if(nbs<0)
			perror("sendto :");
		fprintf(stderr, "ping : sendto %s %d chars, achieve %d\n", hostname, 8+sizeData, nbs);
		fflush(stderr);
	}
	nbrSend++;
}


void lirePacketICMP(unsigned char* buf, unsigned int size, struct sockaddr_in* doctorWho){
	struct ip* ip;
	struct icmp* icmpPacket;
	struct timespec tnow;
	struct timespec* tbefore;
	struct timespec diff;
	unsigned int ipheaderlen;
	unsigned int timems;
	char* trunc;
	
	clock_gettime(CLOCK_REALTIME, &tnow);
	ip=(struct ip*) buf;
	ipheaderlen=ip->ip_hl<<2; // passage de bits en octets (*32/8)==> *4 ==> <<2 EZ
	if(size < ipheaderlen + ICMP_MINLEN){
		printf("Vodoo magic happened, the size of the packet must be %d a minima\n", ipheaderlen + ICMP_MINLEN);
		return;
	}
	size-=ipheaderlen;
	icmpPacket=(struct icmp*) (buf+ipheaderlen);
	if(icmpPacket->icmp_type!=ICMP_ECHOREPLY){
		fprintf(stderr, "%d bytes from %s : icmp_type=%d, icmp_code=%d\n", size, inet_ntoa(doctorWho->sin_addr), icmpPacket->icmp_type, icmpPacket->icmp_code);
		return;
	}
	if(icmpPacket->icmp_id!=pid){ // pas notre paquet
		return;
	}
	tbefore=(struct timespec*) &icmpPacket->icmp_data[0];
	diff=time_diff(tbefore, &tnow);
	timems=diff.tv_sec*1000+(diff.tv_nsec/1000000);
	timeOverall+=timems;
	if(timems>timeMax)
		timeMax=timems;
	if(timems<timeMin)
		timeMin=timems;
	if(size<sizeData)
		trunc="(truncated)";
	else
		trunc="";
	fprintf(stdout, "%u bytes from %s: icmp_seq=%d, time %dms %s\n", size-8, inet_ntoa(doctorWho->sin_addr), icmpPacket->icmp_seq, timems, trunc); 
	nbrReceive++;
}
