/**
 * \file timeuh.h (header file)
 * \author SCHMITT Maxime Joël, SCHIMCHOWITSCH PLANTE Raphaël François Guillaume, 
 * \brief header pour le fichier timeuh.c
 */

#ifndef __TIMEUH_H
#define __TIMEUH_H

#include <time.h>

/**
 * \fn struct timespec time_diff(struct timespec* begin, struct timespec* end)
 * \brief Renvoie le differentiel de temps entre un debut et une fin
 * \param begin le debut
 * \param end la fin
 * \return struct timespec avec le temps dedans
 */
struct timespec time_diff(struct timespec* begin, struct timespec* end);

#endif // __TIMEUH_H
