/**
 * \file tracerouteUDP.h (header file)
 * \author SCHMITT Maxime Joël, SCHIMCHOWITSCH PLANTE Raphaël François Guillaume, 
 * \brief header pour le fichier tracerouteUDP.c
 */


#ifndef __TRACEROUTE_UDP_H
#define __TRACEROUTE_UDP_H

#include <netinet/in.h>
/**
 * \brief analyse le packet retourné par le reseau
 * \param buf le buffer contenant le packet
 * \param size la taille du packet
 * \param doctorWho celui qui a envoyé le packet
 */

void tracertUDP(unsigned char* buf, unsigned int size, struct sockaddr_in* doctorWho);
/**
 * \brief ecrit et lance le packet sur le reseau
 */

void pingerUDP(void);


#endif // __TRACEROUTE_UDP_H
