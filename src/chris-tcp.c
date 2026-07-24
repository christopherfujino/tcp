#include <arpa/inet.h> // htons(), inet_addr(), AF_INET
#include <string.h>    // memset()

#include "chris-tcp.h"

const char *ADDRESS = "127.0.0.1";
const int PORT = 8080;

void init_address(struct sockaddr_in *addr) {
  memset(addr, 0x00, sizeof(struct sockaddr_in));

  addr->sin_family = AF_INET;
  // convert host to network byte order
  addr->sin_port = htons(PORT);
  // convert string to struct in_addr
  addr->sin_addr.s_addr = inet_addr(ADDRESS);
}
