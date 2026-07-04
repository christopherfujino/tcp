#include <arpa/inet.h>  // inet_addr(), struct sockaddr_in
#include <errno.h>      // errno
#include <stdio.h>      // fwrite(), fprintf(), printf()
#include <string.h>     // strerror
#include <sys/socket.h> // accept(), bind(), listen(), socklen_t
#include <unistd.h>     // close()

static const int PORT = 8080;
static const int BACKLOG = 16;
static const char *ADDRESS = "127.0.0.1";

static void _pretty_print_u32(unsigned int original_i,
                              unsigned short int port) {
  unsigned char *bytes = (unsigned char *)&original_i;
  printf("%d.%d.%d.%d:%d\n", bytes[0], bytes[1], bytes[2], bytes[3], port);
}

int main() {
  struct sockaddr_in server_address, client_address = {0};

  // man 2 (syscall) socket
  int sock_fd = socket(AF_INET,     // address family internet, that is ipv4
                       SOCK_STREAM, // use TCP
                       0            // protocol,
  );
  if (sock_fd == -1) {
    fprintf(stderr, "Failed to open a TCP socket\n");
    return 1;
  }

  server_address.sin_family = AF_INET;
  // convert host to network byte order
  server_address.sin_port = htons(PORT);
  // convert string to struct in_addr
  server_address.sin_addr.s_addr = inet_addr(ADDRESS);

  // Bind
  if (bind(sock_fd, (struct sockaddr *)(&server_address),
           sizeof(server_address))) {
    fprintf(stderr, "Failed to bind to port %d\n", PORT);
    return 1;
  }

  // Listen
  if (listen(sock_fd, BACKLOG) != 0) {
    fprintf(stderr, "Failed to listen to the socket: %s\n", strerror(errno));
    return 1;
  }

  printf("Server now listening at %s:%d\n", ADDRESS, PORT);

  socklen_t client_address_len = sizeof(client_address);
  // Accept
  int accepted_sock_fd = accept(sock_fd, (struct sockaddr *)(&client_address),
                                &client_address_len);
  if (accepted_sock_fd < 0) {
    fprintf(stderr, "Server accept failed\n");
    return 1;
  }
  printf("A client connected from ");
  _pretty_print_u32(client_address.sin_addr.s_addr, client_address.sin_port);
  printf("\n");

  char buffer[BUFSIZ] = {0};
  while (1) {
    printf("start of loop-a-noop\n");
    size_t n = read(accepted_sock_fd, buffer, sizeof(buffer));
    if (n == -1) {
      fprintf(stderr, "Error reading from client: %s", strerror(errno));
    }
    printf("Read %ld bytes from socket:\n", n);
    fwrite(buffer, 1, n, stdout);
    printf("\n");
  }
  close(sock_fd);
  return 0;
}
