#include <arpa/inet.h>  // inet_addr(), struct sockaddr_in
#include <errno.h>      // errno
#include <optional>     // std::optional<T>
#include <stdio.h>      // fwrite(), fprintf(), printf()
#include <string.h>     // strerror
#include <sys/socket.h> // accept(), bind(), listen(), socklen_t
#include <unistd.h>     // close()
#include <vector>       // std::vector<T>

#include "message.h"
#include "tcp.h"

static const int BACKLOG = 16;

static void _pretty_print_u32(unsigned int original_i,
                              unsigned short int port) {
  unsigned char *bytes = (unsigned char *)&original_i;
  printf("%d.%d.%d.%d:%d\n", bytes[0], bytes[1], bytes[2], bytes[3], port);
}

class Server {
public:
  Server() {
    // man 2 (syscall) socket
    listen_fd = socket(AF_INET,     // address family internet, that is ipv4
                       SOCK_STREAM, // use TCP
                       0            // protocol,
    );
    if (listen_fd == -1) {
      fprintf(stderr, "Failed to open a TCP socket\n");
      throw "flooboo";
    }

    struct sockaddr_in server_address = {};

    init_address(&server_address);

    // Bind
    if (bind(listen_fd, (struct sockaddr *)(&server_address),
             sizeof(server_address))) {
      fprintf(stderr, "Failed to bind to port %d\n", PORT);
      throw "flooboo";
    }

    // Listen
    if (listen(listen_fd, BACKLOG) != 0) {
      fprintf(stderr, "Failed to listen to the socket: %s\n", strerror(errno));
      throw "flooboo";
    }

    printf("Server now listening at %s:%d\n", ADDRESS, PORT);
  }

  ~Server() {
    if (listen_fd >= 0) {
      close(listen_fd);
    }
  }

  void accept_client() {
    struct sockaddr_in client_address = {};
    socklen_t client_address_len = sizeof(struct sockaddr_in);

    // Accept
    int accept_fd = accept(listen_fd, (struct sockaddr *)(&client_address),
                           &client_address_len);
    if (accept_fd < 0) {
      fprintf(stderr, "Server accept failed\n");
      throw "flooboo";
    }

    client_fds.push_back(accept_fd);

    printf("A client connected from ");
    _pretty_print_u32(client_address.sin_addr.s_addr, client_address.sin_port);
    printf("\n");
  }

private:
  int listen_fd = -1;
  std::vector<int> client_fds = {};
};

int main(void) {
  Server server;

  // TODO we'll need to call this again to accept more connections
  server.accept_client();

  Message msg = server.receive_message();
  printf("Received a message from client:\n\n");
  fwrite(msg.data, 1, msg.size, stdout);
  printf("\n");
  free_message(msg);

  return 0;
}
