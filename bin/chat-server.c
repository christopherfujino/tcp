#include "tcp-rpc.h"

int main(void) {
  Connections connections = connections_create();

  connections_free(&connections);
}
