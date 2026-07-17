#include <arpa/inet.h>  // inet_addr(), struct sockaddr_in
#include <errno.h>      // errno
#include <poll.h>       // poll(), struct pollfd
#include <stdio.h>      // fwrite(), fprintf(), printf()
#include <stdlib.h>     // exit()
#include <string.h>     // strerror
#include <sys/socket.h> // accept(), bind(), listen(), socklen_t
#include <time.h>       // nanosleep()
#include <unistd.h>     // close()

#include "connections.h"
#include "message.h"
#include "tcp.h"

static const int BACKLOG = 16;

static void _pretty_print_u32(unsigned int original_i,
                              unsigned short int port) {
  unsigned char *bytes = (unsigned char *)&original_i;
  printf("%d.%d.%d.%d:%d\n", bytes[0], bytes[1], bytes[2], bytes[3], port);
}

static void _sleep(void) {
  static const int milliseconds = 250;
  static const int nanoseconds = milliseconds * 1000000;
  struct timespec duration = {
      .tv_sec = 0,
      .tv_nsec = nanoseconds,
  };
  nanosleep(&duration, NULL);
}

static int _accept_connection(int sock_fd, Connections *connections) {
  struct sockaddr_in address;
  socklen_t socklen = sizeof(struct sockaddr_in);

  int fd = accept(sock_fd, (struct sockaddr *)(&address), &socklen);
  if (fd < 0) {
    return 1;
  }
  connections_add(connections, fd);

  printf("A client connected from ");
  _pretty_print_u32(address.sin_addr.s_addr, address.sin_port);
  printf("\n");

  return 0;
}

int main(void) {
  struct sockaddr_in server_address;
  init_address(&server_address);

  Connections connections = connections_create();

  // man 2 (syscall) socket
  int listen_fd = socket(AF_INET,     // address family internet, that is ipv4
                         SOCK_STREAM, // use TCP
                         0            // protocol,
  );
  if (listen_fd == -1) {
    fprintf(stderr, "Failed to open a TCP socket\n");
    return 1;
  }

  // Bind
  if (bind(listen_fd, (struct sockaddr *)(&server_address),
           sizeof(server_address))) {
    fprintf(stderr, "Failed to bind to port %d\n", PORT);
    close(listen_fd);
    return 1;
  }

  // Listen
  if (listen(listen_fd, BACKLOG) != 0) {
    fprintf(stderr, "Failed to listen to the socket: %s\n", strerror(errno));
    close(listen_fd);
    return 1;
  }

  printf("Server now listening at %s:%d\n", ADDRESS, PORT);

  while (1) {
    _sleep();

    if (connections.len == 0) {
      printf("Waiting for an active connection...\n");
      if (_accept_connection(listen_fd, &connections)) {
        fprintf(stderr, "Server accept failed!\n");
        close(listen_fd);
        return 1;
      }
    } else {
      printf("Start of server loop with %d active connections...\n",
             connections.len);
    }

    // TODO also poll the listen_fd
    int ready_fds = poll(connections.data, connections.len, -1);
    if (ready_fds == 0) {
      fprintf(stderr, "UNREACHABLE timeout (%s:%d)\n", __FILE__, __LINE__);
      exit(1);
    } else if (ready_fds < 0) {
      perror("Error polling");
      exit(1);
    }

    for (int i = 0; i < connections.len; i++) {
      struct pollfd fd = connections.data[i];

      if (fd.revents == 0) {
        continue;
      }

      {
        int revents_mask = fd.revents;
        if (revents_mask & POLLIN) {
          Message msg;

          Result result = receive_message(fd.fd, &msg);
          switch (result) {
          case ResultEOF:
            close(fd.fd);
            connections_remove(&connections, i);
            // remember we mutated the list in a loop
            i--;
            break;
          case ResultError:
            exit(1);
          case ResultOk:
            printf("Received a message from client:\n\n");
            fwrite(msg.data, 1, msg.size, stdout);
            printf("\n");

            free_message(msg);
            break;
          }
          revents_mask -= POLLIN;
        }

        if (revents_mask & POLLHUP) {
          close(fd.fd);
          connections_remove(&connections, i);
          revents_mask -= POLLHUP;
          i--;
        }

        if (revents_mask > 0) {
          fprintf(stderr, "TODO: implement further masks: 0x%x\n",
                  revents_mask);
          exit(1);
        }
      }
    }
  }

  close(listen_fd);
  return 0;
}
