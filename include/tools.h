#ifndef __TOOLS_H
#define __TOOLS_H

#include "const.h"
#include <sys/types.h>

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

u_int16_t checksum(u_int16_t * icmp, int totalLength);

void * pingou (void * time);

u_int16_t checksum_tcp_udp(char proto, u_int32_t ip_source_tampon, u_int32_t ip_destination_tampon, unsigned char buf[MAXPACKET], int totalLength);

#endif  // __TOOLS_H
