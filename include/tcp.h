#ifndef __TCP_H
#define __TCP_H 1

#include <arpa/inet.h> // sockaddr_in

extern const int PORT;
extern const char *ADDRESS;

void init_address(struct sockaddr_in *addr);

#endif // #ifndef __TCP_H
