#ifndef __TRACEROUTE_UDP_H
#define __TRACEROUTE_UDP_H

#include <netinet/in.h>

void tracertUDP(unsigned char*, unsigned int, struct sockaddr_in*);

void pingerUDP(void);


#endif // __TRACEROUTE_UDP_H
