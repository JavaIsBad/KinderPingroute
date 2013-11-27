#ifndef __TOOLS_H
#define __TOOLS_H

#include <arpa/inet.h>

typedef struct {
	int sockfd;
	union{
		struct sockaddr_in v4;
		struct sockaddr_in6 v6;
	} u_me;
	union{
		struct sockaddr_in v4;
		struct sockaddr_in6 v6;
	} u_other;
} connect_info;

int connecticut(int af, int type/*Sock_raw/dgram/stream */, const char* src, connect_info* ci, int port);

#endif  // __TOOLS_H
