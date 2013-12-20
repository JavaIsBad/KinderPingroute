/**
 * \file const.h (header filer)
 * \author SCHIMCHOWITSCH PLANTE Raphaël François Guillaume, SCHMITT Maxime Joël
 * \brief definit les constantes utilisées dans les fonctions
 */

#ifndef __CONST_H
#define __CONST_H


#define MAXJUMP 30  /**<Le nombre de saut max pour traceroute */
#define MAXTRY 4 /**<Le nombre max d'essays avant de passer au saut suivant */
#define MAXPACKET 4096 /**<La taille maximale */
#define ICMP_OPTION 1 /**<Option ICMP */
#define TCP_OPTION 2 /**<Option TCP */
#define UDP_OPTION 4 /**<Option UDP */
#define SIZE_OPTION 8 /**<Option de taille des paquets a envoyer */
#define TIME_OPTION 16 /**<Option de temps entre chaque paquets envoye */
#define PORT_OPTION 32 /**<Option de sPort a utiliser */


#endif // __CONST_H
