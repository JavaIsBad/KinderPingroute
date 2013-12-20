#ifndef __TRACEROUTE_ICMP_H
#define __TRACEROUTE_ICMP_H

#include <netinet/in.h>

void tracertICMP(unsigned char*, unsigned int, struct sockaddr_in*);

void pingerICMP(void);

#endif // __TRACEROUTE_ICMP_H
