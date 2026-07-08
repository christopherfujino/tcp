#ifndef __TCP_H
#define __TCP_H 1

#include <arpa/inet.h> // sockaddr_in

#define PORT 8080
#define ADDRESS "127.0.0.1"

void init_address(struct sockaddr_in *addr);

#endif // #ifndef __TCP_H
