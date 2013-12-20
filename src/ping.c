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
#include <ifaddrs.h>


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
struct sockaddr_in moi;
pthread_t threadPinger;
unsigned char buffer[MAXPACKET];
char nameDest[INET6_ADDRSTRLEN];
void (*pinger)(void);
void (*lirePacket)(unsigned char*, unsigned int, struct sockaddr_in*);


int main(int argc, char** argv){
	LocalPort=htons(6789); // au pif
	DistantPort=htons(80); // par defaut
	struct sigaction siga;
	struct timespec timetowait;
	struct sockaddr_in from;
	struct addrinfo wantedAddr;
	struct addrinfo *to=NULL;
	struct addrinfo *parseAddr=NULL;
	struct ifaddrs *myaddrs, *ifa;
	unsigned int option=0;

	if(argc!=2){
		printf("Utilisation : %s -option1 -option2 ... adresse/url\n", argv[0]);
		exit(EXIT_FAILURE);
	}
	
	memset(&wantedAddr, 0, sizeof(struct addrinfo));
	
	wantedAddr.ai_family=AF_INET;
	wantedAddr.ai_socktype=SOCK_RAW;
	//******** PARSER **********
	option|=ICMP_OPTION;
	hostname=argv[1];
	//******** END PARSER ************
	if(!(option & ICMP_OPTION & TCP_OPTION & UDP_OPTION)){ // si aucune indication utiliser ICMP
		option|=ICMP_OPTION;
	}
	if(option & ICMP_OPTION){
		pinger=pingerICMP;
		lirePacket=lirePacketICMP;	
		wantedAddr.ai_protocol=IPPROTO_ICMP;
	}
	if(option & TCP_OPTION){
		pinger=pingerTCP;
		lirePacket=lirePacketTCP;
		wantedAddr.ai_protocol=IPPROTO_TCP;
	}
	if((option & TIME_OPTION) && !(option & TCP_OPTION & UDP_OPTION)){ // pas changer pour tcp et udp !
		timetowait.tv_sec=0; // a changer
		timetowait.tv_nsec=0; // a changer
	}
	else{ // defaut 1 sec
		timetowait.tv_sec=1; 
		timetowait.tv_nsec=0;
	}
	if(option & SIZE_OPTION){ //uniquement en ICMP
		sizeData=0; // a changer
	}
	else{
		sizeData=64; //defaut
	}
	if(option & PORT_OPTION & UDP_OPTION){
		DistantPort=htons(0); // a changer
	}
	
    if(getifaddrs(&myaddrs) != 0){ //recuperation de notre adresse ip
        perror("getifaddrs");
        exit(1);
    }
    for (ifa = myaddrs; ifa != NULL; ifa = ifa->ifa_next){
        if (ifa->ifa_addr == NULL)
            continue;
        if(ifa->ifa_addr->sa_family == AF_INET){
				if(strcmp("lo", ifa->ifa_name)==0)
					continue;
                memcpy(&moi, ifa->ifa_addr, sizeof(struct sockaddr_in)); 
                break; // on a trouvé une adresse usefull
         }
         else
			continue;
    }
    freeifaddrs(myaddrs);
    char buf[64];
    memset(buf,0,64);
    if (!inet_ntop(AF_INET, &moi.sin_addr, buf, sizeof(buf))){
		printf("%s: inet_ntop failed!\n", ifa->ifa_name);
	}
	else{
		printf("Adresse local : %s\n", buf);
	}
	
	pid=getpid();
	siga.sa_handler=sigIntAction;
	sigfillset(&siga.sa_mask);
	siga.sa_flags=0;
	sigaction(SIGINT, &siga, NULL);
		
	if(getaddrinfo(hostname, NULL, &wantedAddr, &to)<0){
		perror("getaddrinfo");
		exit(EXIT_FAILURE);
	}
	
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

	inet_ntop(destination.sin_family, &destination.sin_addr, nameDest, INET6_ADDRSTRLEN);
	pthread_create(&threadPinger, NULL, pingou, &timetowait);
	printf("Start pinging %s (%s) with %u data bytes send\n", hostname, nameDest, sizeData);
	socklen_t doctorWhoLength=sizeof(from);
	for(;;){
		doctorWhoLength=sizeof(from);
		int nbrecv;
		if((nbrecv=recvfrom(sockfd, buffer, MAXPACKET, 0, (struct sockaddr*) &from, &doctorWhoLength))<=0){
			perror("recvfrom :");
			continue;
		}
		lirePacket(buffer, (unsigned int) nbrecv, &from);
	}
	return EXIT_SUCCESS;
}

