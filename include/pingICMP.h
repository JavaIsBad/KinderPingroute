#ifndef __PINGBIS_H
#define __PINGBIS_H

#include <netinet/in.h>

void pingerICMP(void);

void lirePacketICMP(unsigned char* buf, unsigned int size, struct sockaddr_in* doctorWho);

#endif // __PING_H
