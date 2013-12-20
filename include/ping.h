#ifndef __PING_H
#define __PING_H
#include <string.h>
#include <ctype.h>
static inline int estEntier(char* c){ //1 si c'est pas un entier, 0 si c'est un entier
unsigned int i;
for (i=0;i<strlen(c);i++)
	{
		if (!isdigit(c[i])){
			return 1;
		}
	}
return 0;
} 
#endif // __PING_H
