#include "tools.h"
#include "const.h"
#include "pingTCP.h"
#include "timeuh.h"
#include <netinet/tcp.h>
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
extern unsigned char* buffer;
extern char nameDest[INET6_ADDRSTRLEN];

static struct timespec tbefore;



void pingerTCP(void){
	unsigned char packet[MAXPACKET];
	struct tcphdr *head=(struct tcphdr*) &packet;
	unsigned int i;
	unsigned char *data=&packet[20];
	int nbs;
	memset(head, 0, sizeof(struct tcphdr));
	head->source=LocalPort;
	head->dest=DistantPort;
	head->seq=nbrSend;
	head->ack_seq=0;
	head->doff=5; // 5*32bits (5 bytes)
	head->syn= 1;
	head->window= TCP_MAXWIN;
	
	for(i=0; i<sizeData; i++)
		*data++=i;
	head->check=checksum((unsigned short*) packet, 20+sizeData);
	nbs=sendto(sockfd, packet, 20+sizeData, 0, (struct sockaddr*) &destination, sizeof(struct sockaddr));
	if(nbs<0 || (unsigned int) nbs< 8+sizeData){
		if(nbs<0)
			perror("sendto :");
		fprintf(stderr, "ping : sendto %s %d chars, achieve %d\n", hostname, 8+sizeData, nbs);
		fflush(stderr);
	}
	clock_gettime(CLOCK_REALTIME, &tbefore);
	nbrSend++;
}

void lirePacketTCP(unsigned char* buf, unsigned int size, struct sockaddr_in* doctorWho){
	struct ip* ip;
	struct tcphdr *tcphead;
	struct timespec tnow;
	struct timespec diff;
	unsigned int ipheaderlen;
	unsigned int tcpheaderlen;
	unsigned int timems;
	
	clock_gettime(CLOCK_REALTIME, &tnow);
	ip=(struct ip*) buf;
	ipheaderlen=ip->ip_hl<<2; // passage de bits en octets (*32/8)==> *4 ==> <<2 EZ
	if(size < ipheaderlen){
		printf("Vodoo magic happened, the size of the packet must be %d a minima\n", ipheaderlen);
		return;
	}
	size-=ipheaderlen;
	tcphead=(struct tcphdr*) (buf+ipheaderlen);
	tcpheaderlen=tcphead->doff << 2;
	if(size < tcpheaderlen){
		printf("Vodoo magic happened, the size of the packet must be %d a minima\n", tcpheaderlen+ipheaderlen);
		return;
	}
	size-=tcpheaderlen;
	if(!tcphead->syn && !tcphead->ack && tcphead->ack_seq!=nbrSend){
		if(!tcphead->syn || !tcphead->ack)
			printf("message from %s wich is not an syn/ack as expected\n", inet_ntoa(doctorWho->sin_addr));
		else
			printf("this is not the message we were expecting : got %lu, get %u\n", nbrSend, tcphead->ack_seq);
		return;
	}
	diff=time_diff(&tbefore, &tnow);
	timems=diff.tv_sec*1000+(diff.tv_nsec/1000000);
	timeOverall+=timems;
	if(timems>timeMax)
		timeMax=timems;
	if(timems<timeMin)
		timeMin=timems;
	fprintf(stdout, "message received from %s: tcp_seq=%d, time %dms\n", inet_ntoa(doctorWho->sin_addr), tcphead->ack_seq, timems); 
	nbrReceive++;
}
