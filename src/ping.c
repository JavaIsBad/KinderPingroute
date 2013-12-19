#include "ping.h"
#include "pingTCP.h"
#include "pingICMP.h"
#include "tools.h"
#include "const.h"
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <signal.h>
#include <string.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>


u_int16_t LocalPort;
u_int16_t DistantPort;
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
void (*pinger)(void);

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
	printf("Start pinging %s (%s) with %u data bytes send\n", hostname, nameDest, sizeData);
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

