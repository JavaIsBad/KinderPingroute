/**
 * \file tracetaroute.c code source pour les fonctions du traceroute
 * \author SCHMITT Maxime et son fidèle acolyte SCHIMCHOWITSCH Raphaël
 */

/* Fonctionnement :
envoie un paquet UDP/TCP/ICMP avec un TTL incrémentant
Chaque routeur qui reçoit un paquet décrémente le TTL et continue vers le routeur suivant
On rcoit une erreur du routeur lorsque TTL = 0
=> ça permet de faire la liaison de proche en proche
On essaie de contacter un port invalide pour que la réponse du routeur 
soit du type ICMP Port Unreachable (le port 33434 par défaut)
*/

#include "tracetaroute.h"
#include <stdlib.h>
#include <stdio.h>

#include <sys/types.h>
#include <unistd.h>

#include <time.h>

#include <sys/types.h>
#include <sys/socket.h>

struct EnvoieEtRecup *EeR;

void __init(struct EnvoieEtRecup *EetR){ //initialisation des 2 sockets
	EetR->envoieSocketUDP=socket(AF_INET, SOCK_RAW, IPPROTO_RAW);
	if (EetR->envoieSocketUDP <0){
		perror ("socket :");
		exit(EXIT_FAILURE);
	}
	EetR->recoitICMPSocket=socket(AF_INET,SOCK_RAW, 1 /*ICMP*/);
	if (EetR->recoitICMPSocket <0){
		perror("socket :");
		exit(EXIT_FAILURE);
	}
}

struct SONDE_UDP *der_UDP; //dernier paquet UDP utilisé
unsigned short identite; //Ce sera l'identité du routeur présent : avec htons(identite)
unsigned short portParDefaut = 33434; //le port qu'on essaiera d'atteindre en envoyant les paquets. 
//C'est un port invalide, et la réponse du routeur tracer sera de type ICMP Port Unreachable
int taille; //La taille des données
struct sockaddr aQui;	//La machine a laquelle on essaie d'acceder
char *hote; //le nom de domaine de l'hôte

void envoieMessageUDP(unsigned char SEQ, unsigned char TTL){
int taillePaquetSansEntete = taille - sizeof(struct ip); //taille totale moins l'entete
/*
 On récupère les infos du dernier paquet envoyé
 */
struct SONDE_UDP *udpsonde = der_UDP;
struct ip *ip= &udpsonde->ip;
struct udphdr *udphdr = &udpsonde->udp;

/*
 On met a jour avec les données du routeur actuel
 */
udpsonde->SEQ=SEQ; //Le nouveau numero de séquence et le nouveau TTL sont donnes en paramètres
udpsonde->TTL=TTL;
int r;
r=clock_gettime(CLOCK_REALTIME, &udpsonde->tv);
if (r<0){
	perror("clock_gettime :");
	exit(EXIT_FAILURE);
}

/*
	 *  struct udphdr :
	 * u_short uh_sport   ou  source => port source 
	 * u_short uh_dport   ou  dest => port destination  
	 * u_short uh_ulen   ou  len => taille de l'UDP
	 * u_short uh_sum   ou  check => Check sum
*/
/*
 * Mettre a jour la struct udphdr
 */
//htons convertit un numero de port en big indian en général
udphdr->source = htons(identite); // Pour avoir l'identité de l'émetteur
udphdr->dest = htons(SEQ+portParDefaut);
udphdr->len = htons((unsigned short)taillePaquetSansEntete); // htons prends un unsigned short et pas un int
udphdr->check = 0; //Pas de Checksum

/*
 * Mettre a jour la struct IP
 */
ip->ip_off = 0; //l'offset
ip->ip_p = IPPROTO_UDP; 
ip->ip_len = taille; // La taille est égale a la taille totale du paquet
ip->ip_ttl = TTL; // On change le TTL avec le TTL donné en paramètre

int snd;
snd=sendto(EeR->envoieSocketUDP,(char *)der_UDP, taille, 0, &aQui, sizeof(struct sockaddr)); //on envoie le paquet
if (snd<1){
	fprintf(stderr,"traceroute : ecrit a %s %d chars, ret=%d\n", hote, taille, snd);
	fflush(stderr);
}
}

void main(){
}
