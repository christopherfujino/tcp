#include <arpa/inet.h> // sockaddr_in
#include <errno.h>     // errno
#include <stdio.h>     // fprintf(), stderr
#include <stdlib.h>
#include <string.h>     // strerror()
#include <sys/socket.h> // socket()
#include <unistd.h>     // close()

#include "message.h"
#include "tcp.h"

static int _sock_fd = -1;

static void _error_exit(void) {
  if (_sock_fd > -1) {
    close(_sock_fd);
  }
  exit(1);
}

static void _connect_with_backoff(struct sockaddr_in *sa) {
  const int MAX_ATTEMPTS = 3;
  int wait_sec = 1;

  for (int i = 0; i < MAX_ATTEMPTS; i++) {
    if (connect(_sock_fd, (struct sockaddr *)sa, sizeof(*sa))) {
      fprintf(stderr, "Failed to connect to socket: %s\n", strerror(errno));
      printf("Waiting for %d seconds...\n", wait_sec);
    } else {
      printf("Successfully connected to server\n");
      return;
    }
    sleep(wait_sec);
    wait_sec *= 2;
  }
  fprintf(stderr, "Failed to connect to socket: %s\n", strerror(errno));
  _error_exit();
}

int main(void) {
  _sock_fd = socket(AF_INET, SOCK_STREAM, 0);
  if (_sock_fd == -1) {
    fprintf(stderr, "Failed to open a TCP socket\n");
    _error_exit();
  }
  struct sockaddr_in server_address = {0};
  init_address(&server_address);

  _connect_with_backoff(&server_address);

  {
    Message msg = message_of_pointer("yolo", 5); // include NULL byte!
    send_message(_sock_fd, msg);
    free_message(msg);
  }

  close(_sock_fd);
  return 0;
}
