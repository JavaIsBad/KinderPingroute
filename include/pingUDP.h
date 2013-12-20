#ifndef __PING_UDP_H
#define __PING_UDP_H

#include <netinet/in.h>

void pingerUDP(void);

void lirePacketUDP(unsigned char* buf, unsigned int size, struct sockaddr_in* doctorWho);

#endif
