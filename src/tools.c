#include "tools.h"
#include "const.h"
#include <signal.h>
#include <pthread.h>
#include <netinet/ip.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//~ int connecticut(int af, int type/*Sock_raw/dgram/stream */, const char* src, connect_info* ci, int port){
	//~ int protocol;
	//~ switch(af){
		//~ case AF_INET :
			//~ switch(type){
				//~ case SOCK_RAW :
					//~ protocol=IPPROTO_ICMP; //icmp
					//~ break;
				//~ case SOCK_DGRAM :
					//~ protocol=IPPROTO_UDP; //udp
					//~ break;
				//~ case SOCK_STREAM :
					//~ protocol=IPPROTO_TCP; //tcp
					//~ break;
				//~ default:
					//~ protocol=-1;
					//~ break;
			//~ }
			//~ ci->sockfd=socket(af, type, protocol);
			//~ ci->u_other.v4.sin_family = af;
			//~ ci->u_other.v4.sin_port = port;
			//~ inet_pton(af, src, &ci->u_other.v4.sin_addr); 
			//~ break;
		//~ case AF_INET6 :
		//~ switch(type){
				//~ case SOCK_RAW :
					//~ protocol=IPPROTO_ICMPV6; //icmp
					//~ break;
				//~ case SOCK_DGRAM :
					//~ protocol=IPPROTO_UDP; //udp
					//~ break;
				//~ case SOCK_STREAM :
					//~ protocol=IPPROTO_TCP; //tcp
					//~ break;
				//~ default:
					//~ protocol=-1;
					//~ break;
			//~ }
			//~ ci->sockfd=socket(af, type, protocol);
			//~ ci->u_other.v6.sin6_family = af;
			//~ ci->u_other.v6.sin6_port = port;
			//~ inet_pton(af, src, &ci->u_other.v6.sin6_addr); 
			//~ break;
		//~ default:
			//~ break;
	//~ }
	//~ return 0;
//~ }

struct pseudo_entete
    {
    u_int32_t ip_source; // Adresse ip source
    u_int32_t ip_destination; // Adresse ip destination
    char mbz; // Champs à 0
    char type; // Type de protocole
    u_int16_t length; // htons( Taille de l'entete Pseudo + Entete TCP ou UDP + Data )
 };

extern char* hostname;
extern int pid;
extern long unsigned int timeMin;
extern long unsigned int timeMax;
extern long unsigned int timeOverall;
extern long unsigned int nbrReceive;
extern long unsigned int nbrSend;
extern long unsigned int limitePing;
extern pthread_t threadPinger;
extern unsigned char buffer[MAXPACKET];
extern char nameDest[INET6_ADDRSTRLEN];
extern void (*pinger)(void);


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

u_int16_t checksum(u_int16_t* icmp, int totalLength){
	 u_int32_t checksum=0;
    // Complément à 1 de la somme des complément à 1 sur 16 bits
    while(totalLength>1){
		checksum=checksum+*icmp++;
        totalLength=totalLength-sizeof(u_int16_t);
    }

    if(totalLength>0)
        checksum=checksum+*(unsigned char*)icmp;

    checksum=(checksum>>16)+(checksum&0xffff);
    checksum=checksum+(checksum>>16);

    return (u_int16_t)(~checksum); // complement a 1
}

void sigIntAction(int signum){
	pthread_cancel(threadPinger);
	pthread_join(threadPinger, NULL);
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

u_int16_t checksum_tcp_udp(char proto, u_int32_t ip_source_tampon, u_int32_t ip_destination_tampon, unsigned char *buf, int totalLength){
    struct pseudo_entete pseudo_tcp;
    unsigned char tampon[MAXPACKET+sizeof(struct pseudo_entete)];
    u_int16_t check;

    // ********************************************************
    // Le calcul du Checksum TCP (Idem à UDP)
    // ********************************************************
    // Le calcul passe par une pseudo entete TCP + l'entete TCP + les Data
    pseudo_tcp.ip_source=ip_source_tampon;
    pseudo_tcp.ip_destination=ip_destination_tampon;
    pseudo_tcp.mbz=0;
    pseudo_tcp.type=proto; //IPPROTO_TCP ou IPPROTO_UDP
    pseudo_tcp.length=htons((unsigned short)(totalLength));
    memcpy(tampon, &pseudo_tcp, sizeof(pseudo_tcp));
    memcpy(tampon+sizeof(pseudo_tcp), buf, totalLength);
    check=checksum((unsigned short*)tampon, sizeof(struct pseudo_entete)+totalLength);
    return(check);
}
