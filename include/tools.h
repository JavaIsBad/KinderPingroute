#ifndef __TOOLS_H
#define __TOOLS_H

#include "const.h"

//~ typedef struct {
	//~ int sockfd;
	//~ union{
		//~ struct sockaddr_in v4;
		//~ struct sockaddr_in6 v6;
	//~ } u_me;
	//~ union{
		//~ struct sockaddr_in v4;
		//~ struct sockaddr_in6 v6;
	//~ } u_other;
//~ } connect_info;
//~ 
//~ int connecticut(int af, int type/*Sock_raw/dgram/stream */, const char* src, connect_info* ci, int port);

void sigIntAction(int signum);

unsigned short checksum(unsigned short* icmp, int totalLength);

void * pingou (void * time);

unsigned short checksum_tcp(unsigned long ip_source_tampon, unsigned long ip_destination_tampon, unsigned char buf[MAXPACKET], int totalLength);

#endif  // __TOOLS_H
