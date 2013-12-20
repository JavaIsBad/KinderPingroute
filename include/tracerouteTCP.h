#ifndef __TRACEROUTE_TCP_H
#define __TRACEROUTE_TCP_H

#include <netinet/in.h>

void tracertTCP(unsigned char*, unsigned int, struct sockaddr_in*);

void pingerTCP(void);


#endif // __TRACEROUTE_UDP_H
