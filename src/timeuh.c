/**
 * \file timeuh.c (source code file)
 * \author SCHIMCHOWITSCH PLANTE Raphaël François Guillaume, SCHMITT Maxime Joël
 * \brief Code source pour la fonction de calcul du temps
 */

#include "timeuh.h"

struct timespec time_diff(struct timespec* begin, struct timespec* end){
    struct timespec tp;
    tp.tv_sec = end->tv_sec - begin->tv_sec;
    tp.tv_nsec = end->tv_nsec - begin->tv_nsec;
    if(tp.tv_nsec<0){
        tp.tv_sec -= 1;
        tp.tv_nsec += 1000000000;
    }
    return tp;
}
