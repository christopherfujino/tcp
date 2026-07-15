#include <stdio.h>  // fprintf(), stderr
#include <stdlib.h> // malloc()
#include <string.h> // memcpy()

#include "connections.h"

Connections connections_create(void) {
  int *data = (int *)malloc(INITIAL_CONNECTIONS_CAP);
  return (Connections){.len = 0, .cap = INITIAL_CONNECTIONS_CAP, .data = data};
}

void connections_add(Connections *connections, int next) {
  if (connections->len == connections->cap) {
    connections->cap *= 2;
    connections->data = realloc(connections->data, connections->cap);
    if (connections->data == NULL) {
      // TODO return?
      fprintf(stderr, "realloc failure!\n");
      exit(1);
    }
  }

  connections->data[connections->len] = next;
  connections->len += 1;
}

void connections_remove(Connections *connections, int index) {
  if (index >= connections->len) {
    fprintf(stderr, "Invalid call to connections_remove()\n");
    exit(1);
  }

  size_t copy_len = connections->len - index - 1;
  memmove(connections->data + index, connections->data + index + 1, copy_len);
  connections->len -= 1;
}
