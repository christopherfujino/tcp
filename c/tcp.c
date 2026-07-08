#include <arpa/inet.h>

#include "tcp.h"

void init_address(struct sockaddr_in *addr) {
  addr->sin_family = AF_INET;
  // convert host to network byte order
  addr->sin_port = htons(PORT);
  // convert string to struct in_addr
  addr->sin_addr.s_addr = inet_addr(ADDRESS);
}
