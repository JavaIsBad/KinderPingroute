/**
 * \file tracetaroute.h header pour les fonctions du traceroute
 * \author SCHMITT Maxime et son fidèle acolyte SCHIMCHOWITSCH Raphaël
 */

#ifndef __TRACETAROUTE_H
#define __TRACETAROUTE_H

#include <netinet/udp.h>
#include <netinet/ip.h>

struct SONDE_UDP {
	unsigned char TTL;	 //TTL de la sonde
	unsigned char SEQ; //Numero de sequence de la sonde
	struct ip ip;
	/*
	 *  struct udphdr : les informations sur le paquet en train de circuler
	 * u_short uh_sport   ou  source => port source 
	 * u_short uh_dport   ou  dest => port destination  
	 * u_short uh_ulen   ou  len => taille de l'UDP
	 * u_short uh_sum   ou  check => Check sum
	 */
	struct udphdr udp; 
	struct timespec tv;	/* time packet left */
};

struct EnvoieEtRecup { //va de pair avec la fonction __init
	int envoieSocketUDP; //Le socket qu'on utilise pour l'envoie
	int recoitICMPSocket; //ce que l'on recoit
};
/* UDP First :
 * http://www.codeproject.com/Articles/18807/Traceroute-Using-RAW-Socket-UDP
 * http://www-r2.u-strasbg.fr/~pansiot/traceroute/traceroute.c
 */
 void __init(struct EnvoieEtRecup *EetR);
 void envoieMessageUDP(unsigned char SEQ, unsigned char TTL);
 void RecevoirMessageUDP();

#endif //__TRACETAROUTE_H
