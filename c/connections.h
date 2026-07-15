#ifndef __CONNECTIONS_H
#define __CONNECTIONS_H

#define INITIAL_CONNECTIONS_CAP 8

typedef struct Connections {
  int len;
  int cap;
  int *data;
} Connections;

Connections connections_create(void);

void connections_add(Connections *connections, int next);
void connections_remove(Connections *connections, int index);

#endif // __CONNECTIONS_H
