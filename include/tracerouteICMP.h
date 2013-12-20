/**
 * \file tracerouteICMP.h (header file)
 * \author SCHMITT Maxime Joël, SCHIMCHOWITSCH PLANTE Raphaël François Guillaume, 
 * \brief header pour le fichier tracerouteICMP.c
 */


#ifndef __TRACEROUTE_ICMP_H
#define __TRACEROUTE_ICMP_H

#include <netinet/in.h>

/**
 * \brief analyse le packet retourné par le reseau
 * \param buf le buffer contenant le packet
 * \param size la taille du packet
 * \param doctorWho celui qui a envoyé le packet
 */

void tracertICMP(unsigned char* buf, unsigned int size, struct sockaddr_in* doctorWho);

/**
 * \brief ecrit et lance le packet sur le reseau
 */
void pingerICMP(void);

#endif // __TRACEROUTE_ICMP_H
