#ifndef __TOOLS_H
#define __TOOLS_H

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

#endif  // __TOOLS_H
