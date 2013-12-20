/**
 * \file const.h (header filer)
 * \author SCHIMCHOWITSCH PLANTE Raphaël François Guillaume, SCHMITT Maxime Joël
 * \brief definit les constantes utilisées dans les fonctions
 */

#ifndef __CONST_H
#define __CONST_H

#define MAXPACKET 4096 /**<La taille maximale */
#define ICMP_OPTION 1 /**<Option ICMP */
#define TCP_OPTION 2 /**<Option TCP */
#define UDP_OPTION 4 /**<Option UDP */
#define SIZE_OPTION 8 /**<Taille des paquets a envoyer */
#define TIME_OPTION 16 /**<Temps entre chaque paquets envoye */
#define PORT_OPTION 32 /**<SPort a utiliser */

#endif // __CONST_H
