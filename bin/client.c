#include <arpa/inet.h>  // sockaddr_in
#include <errno.h>      // errno
#include <stdio.h>      // fprintf(), stderr
#include <stdlib.h>     // exit()
#include <string.h>     // strerror()
#include <sys/socket.h> // socket()
#include <unistd.h>     // close(), getpid()

#include "chris-tcp.h"

static int _sock_fd = -1;

static void _error_exit(void) {
  if (_sock_fd > -1) {
    close(_sock_fd);
  }
  exit(1);
}

static void _connect_with_backoff(struct sockaddr_in *sa) {
  static const int MAX_ATTEMPTS = 8;
  // 5ms; last attempt will be 20.5 seconds
  useconds_t microseconds = 5000;

  // start at first attempt
  int i = 1;
  while (1) {
    if (connect(_sock_fd, (struct sockaddr *)sa, sizeof(*sa))) {
      fprintf(stderr, "Attempt %d failed to connect to socket: %s\n", i,
              strerror(errno));
    } else {
      printf("Successfully connected to server\n");
      return;
    }

    if (i >= MAX_ATTEMPTS) {
      fprintf(stderr, "Timed out, exiting %d\n", getpid());
      _error_exit();
    }

    if (microseconds >= 500000) {
      printf("Waiting for %.1f secs...\n", (float)(microseconds) / 1000000);
    } else {
      printf("Waiting for %d ms...\n", microseconds / 1000);
    }

    i += 1;
    usleep(microseconds);
    microseconds *= 4;
  }
}

int main(void) {
  _sock_fd = socket(AF_INET, SOCK_STREAM, 0);
  if (_sock_fd == -1) {
    fprintf(stderr, "Failed to open a TCP socket\n");
    _error_exit();
  }
  struct sockaddr_in server_address;
  init_address(&server_address);

  _connect_with_backoff(&server_address);

  {
    Message msg = message_of_pointer("yolo", 5); // include NULL byte!
    send_message(_sock_fd, msg);
    free_message(msg);
  }

  printf("Client exiting...\n");
  close(_sock_fd);
  return 0;
}
