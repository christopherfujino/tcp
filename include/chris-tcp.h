#ifndef __CHRIS_TCP_H
#define __CHRIS_TCP_H

// Public header

#include <arpa/inet.h> // sockaddr_in
#include <stdint.h>    // uint32_t

typedef enum Result {
  ResultOk,
  ResultError,
  ResultEOF,
} Result;

extern const int PORT;
extern const char *ADDRESS;

void init_address(struct sockaddr_in *addr);

// Connections
typedef struct Connections {
  int len;
  int cap;
  struct pollfd *data;
} Connections;

Connections connections_create(void);
void connections_free(Connections *connections);

void connections_add(Connections *connections, int next);
void connections_remove(Connections *connections, int index);
void connections_debug(Connections *connections);

// Message
typedef struct Message {
  uint32_t size; // up to 4gb
  void *data;
} Message;

Message message_of_pointer(void *ptr, uint32_t size);
void free_message(Message msg);
void send_message(int fd, Message msg);
Result receive_message(int fd, Message *message);

#endif // __CHRIS_TCP_H
