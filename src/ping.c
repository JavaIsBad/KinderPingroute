#include

char* hostname;
int pid;
int sockfd;
long unsigned int timeMin=ULONG_MAX;
long unsigned int timeMax=0;
long unsigned int timeOverall=0;
long unsigned int nbrReceive=0;
long unsigned int nbrSend=0;
long unsigned int limitePing=0;
unsigned int sizeData;
struct sockaddr_in destination;
pthread_t threadPinger;
unsigned char buffer[MAXPACKET];
char nameDest[INET6_ADDRSTRLEN];
