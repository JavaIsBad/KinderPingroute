#include "tools.h"
#include "const.h"
#include <signal.h>
#include <pthread.h>
#include <netinet/ip.h>
#include <stdio.h>
#include <stdlib.h>

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
