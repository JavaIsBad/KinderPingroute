/**
 * \file ping.c (source code file)
 * \author SCHIMCHOWITSCH PLANTE Raphaël François Guillaume, SCHMITT Maxime Joël
 * \brief Code source pour la fonction ping
 */

#include "pingICMP.h"
#include "timeuh.h"
#include <time.h>
#include <errno.h>
#include <signal.h>
#include <netinet/ip_icmp.h>
#include <netinet/ip.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <limits.h>

#define MAXPACKET 4096

void (*pinger)(void);

char* hostname;
int pid;
int sockfd;
long unsigned int timeMin=ULONG_MAX;
long unsigned int timeMax=0;
long unsigned int timeOverall=0;
long unsigned int nbrReceive=0;
long unsigned int nbrSend=0;
long unsigned int limitePing=0;
unsigned int sizeData;
struct sockaddr_in destination;
pthread_t threadPinger;
unsigned char buffer[MAXPACKET];
char nameDest[INET6_ADDRSTRLEN];

void * pingou (void * time){
	sigset_t mask;
	sigfillset(&mask);
	pthread_sigmask(SIG_BLOCK, &mask, NULL);
	struct timespec* timer=(struct timespec*) time;
	for(;;){
		if(limitePing!=0 && nbrSend>=limitePing){
			kill(pid, SIGINT);
			return NULL;
		}
		pinger();
		if(nanosleep(timer, NULL)!=0){
			perror("nanosleep :");
			exit(EXIT_FAILURE);
		}
	}
}

void pingerICMP(void){
	unsigned int i;
	int nbs;
	unsigned char packet[MAXPACKET];
	struct icmp *icmpPacket=(struct icmp*) packet;
	struct timespec *time=(struct timespec*) &packet[8]; // partie data de icmp
	unsigned char *data=&packet[8+sizeof(struct timespec)];
	
	icmpPacket->icmp_type = ICMP_ECHO;
	icmpPacket->icmp_code = 0;
	icmpPacket->icmp_cksum = 0;
	icmpPacket->icmp_seq = nbrSend;
	icmpPacket->icmp_id = pid;
	
	for(i=(unsigned int) sizeof(struct timespec); i<sizeData; i++){
		*data++=i;
	}
	clock_gettime(CLOCK_REALTIME, time);
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

unsigned short checksum(unsigned short* icmp, int totalLength){
	 unsigned long checksum=0;
    // Complément à 1 de la somme des complément à 1 sur 16 bits
    while(totalLength>1){
		checksum=checksum+*icmp++;
        totalLength=totalLength-sizeof(unsigned short);
    }

    if(totalLength>0)
        checksum=checksum+*(unsigned char*)icmp;

    checksum=(checksum>>16)+(checksum&0xffff);
    checksum=checksum+(checksum>>16);

    return (unsigned short)(~checksum); // complement a 1
}

void sigIntAction(int signum){
	pthread_cancel(threadPinger);
	fflush(stdout);
	printf("Signal %d received\n", signum);
	printf("************** STATISTICS FOR %s (%s) PINGING **************\n", hostname, nameDest);
	printf("%ld packets sent, ", nbrSend);
	printf("%ld packets received, ", nbrReceive);
	if(nbrSend>0){
		if(nbrReceive>nbrSend)
			printf("vodoo magic happened, we received more packets than we sent!\n");
		else
			printf("%lu%% packet loss\n", (nbrSend-nbrReceive)*100/nbrSend);
	}
	if(nbrReceive>0)
		printf("round-trip time (ms) min/max/avg =>  %lu/%lu/%lu\n", timeMin, timeMax, timeOverall/nbrReceive);
	fflush(stdout);
	exit(EXIT_SUCCESS);
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

int main(int argc, char** argv){
	struct sigaction siga;
	struct timespec timetowait;
	struct sockaddr_in from;
	struct addrinfo wantedAddr;
	struct addrinfo *to;
	struct addrinfo *parseAddr;
	
	if(argc!=2){
		printf("Utilisation : %s -option1 -option2 ... adresse/url\n", argv[0]);
		exit(EXIT_FAILURE);
	}
	
	pid=getpid();
	siga.sa_handler=sigIntAction;
	sigfillset(&siga.sa_mask);
	siga.sa_flags=0;
	sigaction(SIGINT, &siga, NULL);
	
	hostname=argv[1];
	
	memset(&wantedAddr, 0, sizeof(struct addrinfo));
	wantedAddr.ai_family=AF_INET;
	wantedAddr.ai_socktype=SOCK_RAW;
	wantedAddr.ai_protocol=IPPROTO_ICMP;
	getaddrinfo(hostname, NULL, &wantedAddr, &to);
	
	for(parseAddr=to; parseAddr!=NULL; parseAddr=parseAddr->ai_next){
		sockfd=socket(parseAddr->ai_family, parseAddr->ai_socktype, parseAddr->ai_protocol);
		if(sockfd!=-1){
			memcpy(&destination, parseAddr->ai_addr, sizeof(struct sockaddr_in));
			break;
		}
	}
	if(parseAddr==NULL){ // aucune addresse trouvée
		printf("%s, unknown host %s\n", argv[0], hostname);
		exit(EXIT_FAILURE);
	}
	freeaddrinfo(to);
	// faire les actions stdin
	//****************** A CHANGER AVEC LES OPTIONS TOUT CA ***********************
	timetowait.tv_sec=1;
	timetowait.tv_nsec=0;
	pinger=pingerICMP;
	sizeData=64;
	// ******************* JUSQU'ICI ***********************************************
	pthread_create(&threadPinger, NULL, pingou, &timetowait);
	inet_ntop(destination.sin_family, &destination.sin_addr, nameDest, INET6_ADDRSTRLEN);
	printf("Start pinging %s (%s) with %u data bytes send\n", hostname, nameDest, sizeData+8);
	for(;;){
		socklen_t doctorWhoLength=sizeof(from);
		int nbrecv;
		if((nbrecv=recvfrom(sockfd, buffer, MAXPACKET, 0, (struct sockaddr*) &from, &doctorWhoLength))<=0){
			perror("recvfrom :");
			continue;
		}
		lirePacketICMP(buffer, (unsigned int) nbrecv, &from);
	}
	return EXIT_SUCCESS;
}
