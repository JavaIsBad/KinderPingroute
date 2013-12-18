#ifndef __PINGBIS_H
#define __PINGBIS_H

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

void pingerICMP(void);

void * pingou (void * time);

int checksum(unsigned short* icmp, int totalLength);

void sigIntAction(int signum);

void lirePacketICMP(unsigned char* buf, unsigned int size, struct sockaddr_in* doctorWho);

#endif // __PING_H
