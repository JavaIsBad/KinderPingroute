#include "tools.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>

int connecticut(int af, int type/*Sock_raw/dgram/stream */, const char* src, connect_info* ci, int port){
	int protocol;
	switch(af){
		case AF_INET :
			switch(type){
				case SOCK_RAW :
					protocol=IPPROTO_ICMP; //icmp
					break;
				case SOCK_DGRAM :
					protocol=IPPROTO_UDP; //udp
					break;
				case SOCK_STREAM :
					protocol=IPPROTO_TCP; //tcp
					break;
				default:
					protocol=-1;
					break;
			}
			ci->sockfd=socket(af, type, protocol);
			ci->u_other.v4.sin_family = af;
			ci->u_other.v4.sin_port = port;
			inet_pton(af, src, &ci->u_other.v4.sin_addr); 
			break;
		case AF_INET6 :
		switch(type){
				case SOCK_RAW :
					protocol=IPPROTO_ICMPV6; //icmp
					break;
				case SOCK_DGRAM :
					protocol=IPPROTO_UDP; //udp
					break;
				case SOCK_STREAM :
					protocol=IPPROTO_TCP; //tcp
					break;
				default:
					protocol=-1;
					break;
			}
			ci->sockfd=socket(af, type, protocol);
			ci->u_other.v6.sin6_family = af;
			ci->u_other.v6.sin6_port = port;
			inet_pton(af, src, &ci->u_other.v6.sin6_addr); 
			break;
		default:
			break;
	}
	return 0;
}

void main(){
}
