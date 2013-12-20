/**
 * \file ping.h (header file)
 * \author SCHIMCHOWITSCH PLANTE Raphaël François Guillaume, SCHMITT Maxime Joël
 * \brief header pour le fichier ping.c
 */

#ifndef __PING_H
#define __PING_H


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
