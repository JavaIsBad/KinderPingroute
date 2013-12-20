/**
 * \file tools.h (header file)
 * \author SCHMITT Maxime Joël, SCHIMCHOWITSCH PLANTE Raphaël François Guillaume, 
 * \brief header pour le fichier tools.c
 */

#ifndef __TOOLS_H
#define __TOOLS_H

#include "const.h"
#include <sys/types.h>
#include <string.h>
#include <ctype.h>
/**
 * \brief affiche le numero puis 3 étoiles
 * \param i Un numero à afficher
 */
void afficheEtoile(int i);

/**
 * \brief test si ip appartient au tableau tab
 * \param ip  L'element a chercher
 * \param tab  Le tableau dans lequel chercher
 * \return 0 si n'appartient pas et 1 sinon
 */

int appartient(u_int32_t ip, u_int32_t *tab);

/**
 * \fn u_int16_t checksum(u_int16_t * icmp, int totalLength)
 * \brief Fonction servant au Checksum ICMP
 * \param icmp un paquet ICMP
 * \param totalLength la taille totale
 * \return le checksum du paquet ICMP
 */
u_int16_t checksum(u_int16_t * icmp, int totalLength);

/**
 * \fn static inline int estEntier(char* c)
 * \brief regarde si une chaine de caractère represente un entier ou non
 * \param c la chaine de caracteres
 * \return 1 si c n'est pas un entier, 0 sinon
 */
static inline int estEntier(char* c){
unsigned int i;
for (i=0;i<strlen(c);i++)
	{
		if (!isdigit(c[i])){
			return 1;
		}
	}
return 0;
}


/**
 * \fn u_int16_t checksum_tcp_udp(char proto, u_int32_t ip_source_tampon, u_int32_t ip_destination_tampon, unsigned char buf[MAXPACKET], int totalLength);
 * \brief Fonction servant au Checksum TCP et UDP
 * \param proto le protocole
 * \param ip_source_tampon le paquet tampon ip
 * \param ip_destination_tampon la destination du paquet ip
 * \param buf le buffer
 * \param totalLength la taille maxmiale
 * \return le checksum du paquet TCP ou UDP
 */
u_int16_t checksum_tcp_udp(char proto, u_int32_t ip_source_tampon, u_int32_t ip_destination_tampon, unsigned char buf[MAXPACKET], int totalLength);

#endif  // __TOOLS_H
