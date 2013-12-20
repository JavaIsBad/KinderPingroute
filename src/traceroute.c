#include "traceroute.h"
#include "tracerouteICMP.h"
#include "tracerouteUDP.h"
#include "tracerouteTCP.h"
#include "tools.h"
#include "timeuh.h"
#include "const.h"
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <signal.h>
#include <string.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <ifaddrs.h>

u_int32_t adresses[MAXJUMP];
u_int16_t LocalPort;
u_int16_t DistantPort;
char* hostname;
int sockfd;
int sockfd_udp_icmp;
struct sockaddr_in destination;
struct sockaddr_in moi;
unsigned char buffer[MAXPACKET];
char nameDest[INET6_ADDRSTRLEN];
void (*pinger)(void);
void (*lirePacket)(unsigned char*, unsigned int, struct sockaddr_in*);
unsigned int sizeData=64;
int ttl=1;


int main(int argc, char** argv){
	LocalPort=htons(6789); // au pif
	DistantPort=htons(80); // par defaut
	struct timespec timetowait;
	struct sockaddr_in from;
	struct addrinfo wantedAddr;
	struct addrinfo *to=NULL;
	struct addrinfo *parseAddr=NULL;
	struct ifaddrs *myaddrs, *ifa;
	unsigned int option=0;
	int socklisten;

	if(argc!=2){
		printf("Utilisation : %s -option1 -option2 ... adresse/url\n", argv[0]);
		exit(EXIT_FAILURE);
	}
	
	memset(&adresses, 0, MAXJUMP*sizeof(*adresses));
	memset(&wantedAddr, 0, sizeof(struct addrinfo));
	
	wantedAddr.ai_family=AF_INET;
	wantedAddr.ai_socktype=SOCK_RAW;
	//******** PARSER **********
	option|=TCP_OPTION;
	hostname=argv[1];
	//******** END PARSER ************
	if(!(option & ICMP_OPTION & TCP_OPTION & UDP_OPTION)){ // si aucune indication utiliser ICMP
		option|=ICMP_OPTION;
	}
	if(option & ICMP_OPTION){
		pinger=pingerICMP;
		lirePacket=tracertICMP;	
		wantedAddr.ai_protocol=IPPROTO_ICMP;
	}
	if(option & TCP_OPTION){
		pinger=pingerTCP;
		lirePacket=tracertTCP;
		wantedAddr.ai_protocol=IPPROTO_TCP;
	}
	if(option & UDP_OPTION){
		pinger=pingerUDP;
		lirePacket=tracertUDP;
		wantedAddr.ai_protocol=IPPROTO_UDP;
		DistantPort=htons(4); // unasigned
	}
	timetowait.tv_sec=0; 
	timetowait.tv_nsec=500000000; // pour tester 2 fois par seconde et après 10 essays on passe au suivant -> 5sec
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
	if(option & UDP_OPTION || option & TCP_OPTION){
		wantedAddr.ai_protocol=IPPROTO_ICMP;
		if(getaddrinfo(hostname, NULL, &wantedAddr, &to)<0){
			perror("getaddrinfo");
			exit(EXIT_FAILURE);
		}
		
		for(parseAddr=to; parseAddr!=NULL; parseAddr=parseAddr->ai_next){
			sockfd_udp_icmp=socket(parseAddr->ai_family, parseAddr->ai_socktype, parseAddr->ai_protocol);
			if(sockfd!=-1){
				break;
			}
		}
		if(parseAddr==NULL){ // aucune addresse trouvée
			printf("%s, unknown host %s\n", argv[0], hostname);
			exit(EXIT_FAILURE);
		}
		socklisten=sockfd_udp_icmp;
	}
	else
		socklisten=sockfd;
	inet_ntop(destination.sin_family, &destination.sin_addr, nameDest, INET6_ADDRSTRLEN);
	printf("Start tracerouting %s (%s)\n", hostname, nameDest);
	socklen_t doctorWhoLength=sizeof(from);
	setsockopt(sockfd, IPPROTO_IP, IP_TTL, (char*)&ttl, sizeof(ttl));
	int maxSocket=sockfd>socklisten ? sockfd+1 : socklisten+1;
	fd_set ecoute;
	int nbtry=0;
	while(ttl<=MAXJUMP){
		FD_ZERO(&ecoute);
		FD_SET(socklisten, &ecoute);
		if(option & TCP_OPTION){
			FD_SET(sockfd, &ecoute);
		}
		doctorWhoLength=sizeof(from);
		int nbrecv;
		pinger();
		int pres=pselect(maxSocket, &ecoute, NULL, NULL, &timetowait, NULL);
		if(pres<0){
			perror("pselect");
			exit(EXIT_FAILURE);
		}
		if(pres==0){//timeout
			if(nbtry==MAXTRY){
				afficheEtoile(ttl);
				ttl++;
				nbtry=0;
				setsockopt(sockfd, IPPROTO_IP, IP_TTL, (char*)&ttl, sizeof(ttl));
				continue;
			}
			nbtry++;
			continue;
		}
		else{
			if(option & TCP_OPTION){
				if(FD_ISSET(sockfd, &ecoute)){ // on a atteint la destination
					if((nbrecv=recvfrom(sockfd, buffer, MAXPACKET, 0, (struct sockaddr*) &from, &doctorWhoLength))<=0)
						perror("recvfrom :");
					if(from.sin_addr.s_addr==destination.sin_addr.s_addr){ // on a atteind la source
						lirePacket(buffer, (unsigned int) nbrecv, &from);
						return EXIT_SUCCESS; // fin
					}
					continue;
				}
			}
			if((nbrecv=recvfrom(socklisten, buffer, MAXPACKET, 0, (struct sockaddr*) &from, &doctorWhoLength))<=0)
				perror("recvfrom :");
			if(from.sin_addr.s_addr==destination.sin_addr.s_addr){ // on a atteind la source
				lirePacket(buffer, (unsigned int) nbrecv, &from);
				return EXIT_SUCCESS; // fin
			}
			if(!appartient(from.sin_addr.s_addr, adresses)){ // on a trouve
				adresses[ttl-1]=from.sin_addr.s_addr;
				lirePacket(buffer, (unsigned int) nbrecv, &from);
				ttl++;
				nbtry=0;
				setsockopt(sockfd, IPPROTO_IP, IP_TTL, (char*)&ttl, sizeof(ttl));
				continue;
			}
			else{ // une adresse qu'on a déjà eu
				if(nbtry==MAXTRY){
					afficheEtoile(ttl);
					ttl++;
					nbtry=0;
					setsockopt(sockfd, IPPROTO_IP, IP_TTL, (char*)&ttl, sizeof(ttl));
					continue;
				}
				nbtry++;
			}
		}
	}
	return EXIT_SUCCESS;
}


