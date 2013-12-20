/**
 * \file tracerouteTCP.h (header file)
 * \author SCHMITT Maxime Joël, SCHIMCHOWITSCH PLANTE Raphaël François Guillaume, 
 * \brief header pour le fichier tracerouteTCP.c
 */


#ifndef __TRACEROUTE_TCP_H
#define __TRACEROUTE_TCP_H

#include <netinet/in.h>
/**
 * \brief analyse le packet retourné par le reseau
 * \param buf le buffer contenant le packet
 * \param size la taille du packet
 * \param doctorWho celui qui a envoyé le packet
 */

void tracertTCP(unsigned char* buf, unsigned int size, struct sockaddr_in* doctorWho);
/**
 * \brief ecrit et lance le packet sur le reseau
 */

void pingerTCP(void);


#endif // __TRACEROUTE_UDP_H
