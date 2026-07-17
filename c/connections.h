#ifndef __CONNECTIONS_H
#define __CONNECTIONS_H

#include <poll.h> // struct pollfd

#define INITIAL_CONNECTIONS_CAP 8
#define __CONNECTIONS_H_POLL_EVENTS_MASK (POLLIN)

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

#endif // __CONNECTIONS_H
