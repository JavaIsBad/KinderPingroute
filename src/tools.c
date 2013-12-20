#include "tools.h"
#include "const.h"
#include <signal.h>
#include <pthread.h>
#include <netinet/ip.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

struct pseudo_entete
    {
    u_int32_t ip_source; // Adresse ip source
    u_int32_t ip_destination; // Adresse ip destination
    char mbz; // Champs à 0
    char type; // Type de protocole
    u_int16_t length; // htons( Taille de l'entete Pseudo + Entete TCP ou UDP + Data )
 };


u_int16_t checksum(u_int16_t* icmp, int totalLength){
	 u_int32_t checksum=0;
    // Complément à 1 de la somme des complément à 1 sur 16 bits
    while(totalLength>1){
		checksum=checksum+*icmp++;
        totalLength=totalLength-sizeof(u_int16_t);
    }

    if(totalLength>0)
        checksum=checksum+*(unsigned char*)icmp;

    checksum=(checksum>>16)+(checksum&0xffff);
    checksum=checksum+(checksum>>16);

    return (u_int16_t)(~checksum); // complement a 1
}


u_int16_t checksum_tcp_udp(char proto, u_int32_t ip_source_tampon, u_int32_t ip_destination_tampon, unsigned char *buf, int totalLength){
    struct pseudo_entete pseudo_tcp;
    unsigned char tampon[MAXPACKET+sizeof(struct pseudo_entete)];
    u_int16_t check;

    // ********************************************************
    // Le calcul du Checksum TCP (Idem à UDP)
    // ********************************************************
    // Le calcul passe par une pseudo entete TCP + l'entete TCP + les Data
    pseudo_tcp.ip_source=ip_source_tampon;
    pseudo_tcp.ip_destination=ip_destination_tampon;
    pseudo_tcp.mbz=0;
    pseudo_tcp.type=proto; //IPPROTO_TCP ou IPPROTO_UDP
    pseudo_tcp.length=htons((unsigned short)(totalLength));
    memcpy(tampon, &pseudo_tcp, sizeof(pseudo_tcp));
    memcpy(tampon+sizeof(pseudo_tcp), buf, totalLength);
    check=checksum((unsigned short*)tampon, sizeof(struct pseudo_entete)+totalLength);
    return(check);
}

int appartient(u_int32_t ip, u_int32_t *tab){
	int i;
	int trouve=0;
	for(i=0; i<MAXJUMP && !trouve; i++){
		trouve=trouve || tab[i]==ip;
	}
	return trouve;
}

void afficheEtoile(int i){
	printf("%d * * * \n", i);
}
