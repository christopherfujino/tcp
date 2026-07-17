#include <poll.h>   // struct pollfd
#include <stdio.h>  // fprintf(), stderr
#include <stdlib.h> // malloc()
#include <string.h> // memcpy()

#include "connections.h"

Connections connections_create(void) {
  struct pollfd *data =
      (struct pollfd *)malloc(INITIAL_CONNECTIONS_CAP * sizeof(struct pollfd));
  memset(data, 0x0, sizeof(struct pollfd) * INITIAL_CONNECTIONS_CAP);
  return (Connections){.len = 0, .cap = INITIAL_CONNECTIONS_CAP, .data = data};
}

void connections_free(Connections *connections) {
  free(connections->data);
}

void connections_add(Connections *connections, int next) {
  if (connections->len == connections->cap) {
    connections->cap *= 2;
    printf("[DEBUG] realloc; cap = %d; len = %d\n", connections->cap, connections->len);
    connections->data = realloc(connections->data, connections->cap * sizeof(struct pollfd));
    if (connections->data == NULL) {
      // TODO return?
      fprintf(stderr, "realloc failure!\n");
      exit(1);
    }
  }
  connections_debug(connections);

  printf("[DEBUG] adding a connection with FD %d at index %d\n", next, connections->len);
  connections->data[connections->len] = (struct pollfd){
      .fd = next,
      .events = __CONNECTIONS_H_POLL_EVENTS_MASK,
      .revents = 0,
  };
  connections->len += 1;
}

void connections_remove(Connections *connections, int index) {
  if (index >= connections->len) {
    fprintf(stderr, "Invalid call to connections_remove()\n");
    exit(1);
  }

  size_t copy_len = sizeof(*connections->data) * (connections->len - index - 1);
  memmove(connections->data + index, connections->data + index + 1, copy_len);
  connections->len -= 1;
}

void connections_debug(Connections *connections) {
  printf("connections[");
  for (int i = 0; i < connections->len; i++) {
    if (i > 0) {
      printf(", ");
    }
    printf("%d:%d", i, connections->data[i].fd);
  }
  printf("]\n");
}
