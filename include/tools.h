#ifndef __TOOLS_H
#define __TOOLS_H

#include "const.h"
#include <sys/types.h>

void afficheEtoile(int i);

int appartient(u_int32_t ip, u_int32_t *tab);

void sigIntAction(int signum);

u_int16_t checksum(u_int16_t * icmp, int totalLength);

void * pingou (void * time);

u_int16_t checksum_tcp_udp(char proto, u_int32_t ip_source_tampon, u_int32_t ip_destination_tampon, unsigned char buf[MAXPACKET], int totalLength);

#endif  // __TOOLS_H
