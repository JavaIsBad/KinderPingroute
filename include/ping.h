/**
 * \file ping.h (header file)
 * \author SCHIMCHOWITSCH PLANTE Raphaël François Guillaume, SCHMITT Maxime Joël
 * \brief header pour le fichier ping.c
 */

#ifndef __PING_H
#define __PING_H
#include <string.h>
#include <ctype.h>

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
 * \fn void sigIntAction(int signum)
 * \brief sigaction du sigint
 * \param signum le numero de signal
 */
void sigIntAction(int signum);

/**
 * \fn void * pingou (void * time)
 * \brief Thread qui envoie les paquet a intervalle regulier
 * \param time le temps
 */
void * pingou (void * time);

 
#endif // __PING_H
