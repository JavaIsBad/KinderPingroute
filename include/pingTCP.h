/**
 * \file pingTCP.h (header file)
 * \author SCHMITT Maxime Joël, SCHIMCHOWITSCH PLANTE Raphaël François Guillaume, 
 * \brief header pour le fichier pingTCP.c
 */

#ifndef __PINGTCP_H
#define __PINGTCP_H

#include <netinet/in.h>

/**
 * \fn void pingerTCP(void)
 * \brief Fonction qui envoie une sonde TCP
 */
void pingerTCP(void);

/**
 * \fn void lirePacketTCP(unsigned char* buf, unsigned int size, struct sockaddr_in* doctorWho)
 * \brief Fonction recevant les données d'un paquet TCP
 * \param buf le buffer
 * \param size la taille
 * \param doctorWho la machine qui nous a envoye le paquet
 */
void lirePacketTCP(unsigned char* buf, unsigned int size, struct sockaddr_in* doctorWho);

#endif
