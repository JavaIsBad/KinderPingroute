#include "ping.h"
#include "pingTCP.h"
#include "pingICMP.h"
#include "pingUDP.h"
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
#include <string.h>
#include <ctype.h>


u_int16_t LocalPort;
u_int16_t DistantPort;
char* hostname;
int pid;
int sockfd;
int sockfd_udp_icmp;
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
	int socklisten;
	int i;
	int opt=0;
	int cptTIME, cptSIZE, cptPORT;
	if (strcmp(argv[1],"--help")==0){
		fprintf(stderr,"Ceci est l'aide :\n");
		fprintf(stderr,"Les options existantes sont :\n-TCP ou -UDP ou -ICMP, pour choisir de quelle nature sont les paquets envoyés\nL'option ICMP est utilisée par defaut\n");
		fprintf(stderr,"-TIME secondes,nanosecondes Permet de changer le temps d'envoie entre chaques paquets. Temps par défaut utilisé si utilisé avec -UDP ou -TCP");
		fprintf(stderr,"-SIZE taille Permet de changer la taille des paquets envoyés. Utilisable uniquement en ICMP.");
		exit(EXIT_SUCCESS);
	}
	if(argc!=2){
	//	printf("Utilisation : %s -option1 -option2 ... adresse/url\n", argv[0]);
	//	exit(EXIT_FAILURE);
	}
	
	memset(&wantedAddr, 0, sizeof(struct addrinfo));
	
	wantedAddr.ai_family=AF_INET;
	wantedAddr.ai_socktype=SOCK_RAW;
	//******** PARSER **********
	//option|=ICMP_OPTION;
	hostname=argv[argc-1];
	for (i=0;i<argc-1; i++){
		if(strcmp(argv[i], "-TCP")==0){
			if (opt!=0){
				fprintf(stderr,"Veuillez ne choisir qu'une seule option entre TCP, UDP et ICMP.\n");
				fflush(stderr);
				exit(EXIT_FAILURE);
			}
			else{
				opt=1;
				option|=TCP_OPTION;
			}
		}
		else if(strcmp(argv[i], "-ICMP")==0){
			if (opt!=0){
				fprintf(stderr,"Veuillez ne choisir qu'une seule option entre TCP, UDP et ICMP.\n");
				fflush(stderr);
				exit(EXIT_FAILURE);
			}
			else{
				opt=1;
				option|=ICMP_OPTION;
			}
		}
		else if(strcmp(argv[i], "-UDP")==0){
			if (opt!=0){
				fprintf(stderr,"Veuillez ne choisir qu'une seule option entre TCP, UDP et ICMP.\n");
				fflush(stderr);
				exit(EXIT_FAILURE);
			}
			else{
				opt=1;
				option|=UDP_OPTION;
			}
		}
	}
	 for (i=0;i<argc-1;i++){
		  if (strcmp(argv[i],"-TIME")==0){
			 if(option & TCP_OPTION || option & UDP_OPTION)
				 fprintf(stderr,"L'option TIME ne peut être utilisée pour les versions TCP et UDP\nLe temps par défaut est utilisé.\n");
			 else{	
				 option|=TIME_OPTION;
				 cptTIME=i;
			}
		 }
		 if (strcmp(argv[i],"-SIZE")==0){
			 if (option & TCP_OPTION || option & UDP_OPTION)
				fprintf(stderr,"L'option SIZE n'est utilisable qu'en version ICMP. Elle n'est donc pas prise en compte dans le cas présent.\n");
			 else{
				 option|=SIZE_OPTION;
				 cptSIZE=i;
			 }
		 }
		 if (strcmp(argv[i],"-PORT")==0){
			 if (option & ICMP_OPTION || option & TCP_OPTION)
				fprintf(stderr,"L'option PORT n'est utilisable que pour la version UDP. Elle n'est donc pas prise en compte dans le cas présent.\n");
			else{
				option|=PORT_OPTION;
				cptPORT=i;
			}
		}
	 }
				
			
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
	if(option & UDP_OPTION){
		pinger=pingerUDP;
		lirePacket=lirePacketUDP;
		wantedAddr.ai_protocol=IPPROTO_UDP;
		DistantPort=htons(4); // unasigned
	}
	if(option & TIME_OPTION){ // pas changer pour tcp et udp !
		if (cptTIME+1==argc-1 || cptTIME+2==argc-1 || estEntier(argv[cptTIME+1])!=0 || estEntier(argv[cptTIME+2])!=0){
			fprintf(stderr,"Veuillez indiquer les secondes puis les nanosecondes apres l'option -TIME.\n");
			exit(EXIT_FAILURE);
		}
		else{
			timetowait.tv_sec=atoi(argv[cptTIME+1]);
			timetowait.tv_nsec=atoi(argv[cptTIME+2]);
		}
	}
	else{ // defaut 1,5 sec
		timetowait.tv_sec=1; 
		timetowait.tv_nsec=500000000;
	}
	if(option & SIZE_OPTION){ //uniquement en ICMP
		if (cptSIZE+1==argc-1 || estEntier(argv[cptSIZE+1])!=0){
			fprintf(stderr,"Veuillez indiquer la taille des paquets apres l'option -SIZE.\n");
			exit(EXIT_FAILURE);
		}
		sizeData=atoi(argv[cptSIZE+1]);
	}
	else{
		sizeData=64; //defaut
	}
	if(option & PORT_OPTION & UDP_OPTION){
		if (cptPORT+1==argc-1 || estEntier(argv[cptPORT+1])!=0){
			fprintf(stderr,"Veuillez indiquer le port voulu apres l'option -PORT.\n");
			exit(EXIT_FAILURE);
		}
		DistantPort=htons(atoi(argv[cptPORT+1]));
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
	if(option & UDP_OPTION){
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
	pthread_create(&threadPinger, NULL, pingou, &timetowait);
	printf("Start pinging %s (%s) with %u data bytes send\n", hostname, nameDest, sizeData);
	socklen_t doctorWhoLength=sizeof(from);
	for(;;){
		doctorWhoLength=sizeof(from);
		int nbrecv;
		if((nbrecv=recvfrom(socklisten, buffer, MAXPACKET, 0, (struct sockaddr*) &from, &doctorWhoLength))<=0){
			perror("recvfrom :");
			continue;
		}
		lirePacket(buffer, (unsigned int) nbrecv, &from);
	}
	return EXIT_SUCCESS;
}

