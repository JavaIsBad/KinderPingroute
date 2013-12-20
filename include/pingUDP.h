/**
 * \file pingTCP.h (header file)
 * \author SCHMITT Maxime Joël, SCHIMCHOWITSCH PLANTE Raphaël François Guillaume, 
 * \brief header pour le fichier pingTCP.c
 */

#ifndef __PING_UDP_H
#define __PING_UDP_H

#include <netinet/in.h>

/**
 * \fn void pingerUDP(void)
 * \brief Fonction qui envoie une sonde UDP
 */
void pingerUDP(void);

/**
 * \fn void lirePacketUDP(unsigned char* buf, unsigned int size, struct sockaddr_in* doctorWho)
 * \brief Fonction recevant les données d'un paquet UDP
 * \param buf le buffer
 * \param size la taille
 * \param doctorWho la machine qui nous a envoye le paquet
 */
void lirePacketUDP(unsigned char* buf, unsigned int size, struct sockaddr_in* doctorWho);

#endif
