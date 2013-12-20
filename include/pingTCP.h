#ifndef __PINGTCP_H
#define __PINGTCP_H

#include <netinet/in.h>

void pingerTCP(void);

void lirePacketTCP(unsigned char* buf, unsigned int size, struct sockaddr_in* doctorWho);

#endif
