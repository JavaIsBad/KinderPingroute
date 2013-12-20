/**
 * \file pingICMP.h (header file)
 * \author SCHMITT Maxime Joël, SCHIMCHOWITSCH PLANTE Raphaël François Guillaume, 
 * \brief header pour le fichier pingICMP.c
 */

#ifndef __PINGBIS_H
#define __PINGBIS_H

#include <netinet/in.h>

/**
 * \fn void pingerICMP(void)
 * \brief Fonction envoyant une sonde en ICMP
 */
void pingerICMP(void);

/**
 * \fn void lirePacketICMP(unsigned char* buf, unsigned int size, struct sockaddr_in* doctorWho)
 * \brief Fonction recevant les données d'un paquet ICMP
 * \param buf le buffer
 * \param size la taille
 * \param doctorWho la machine qui nous a envoye le paquet
 */
void lirePacketICMP(unsigned char* buf, unsigned int size, struct sockaddr_in* doctorWho);

#endif // __PING_H
