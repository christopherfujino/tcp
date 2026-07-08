#include <errno.h>      // errno
#include <stdint.h>     // uint32_t
#include <stdio.h>      // fprintf(), stderr
#include <stdlib.h>     // malloc()
#include <string.h>     // memcpy(), strerror()
#include <sys/socket.h> // send()

#include "message.h"

Message message_of_pointer(void *ptr, uint32_t size) {
  void *buffer = malloc(size);
  memcpy(buffer, ptr, size);
  return (Message){
      .size = size,
      .data = buffer,
  };
}

void free_message(Message msg) { free(msg.data); }

static const int SEND_FLAGS = 0x0;
static char *SENTINEL = "flooboo";
static const int SENTINEL_SIZE = 7;

static void _send(int fd, size_t bytes_to_send, uint8_t *send_ptr) {
  while (bytes_to_send > 0) {
    ssize_t n = send(fd, send_ptr, bytes_to_send, SEND_FLAGS);
    if (n == -1) {
      // TODO return error to caller to handle cleanup
      fprintf(stderr, "Failed to send message\n");
      exit(1);
    }
    bytes_to_send -= n;
    send_ptr += n;
  }
}

void send_message(int fd, Message msg) {
  uint8_t size_bytes[4] = {0};

  {
    uint32_t _size = msg.size;

    for (int i = 3; i >= 0; i--) {
      size_bytes[i] = _size & 0xFF;
      _size = _size >> 8;
    }
  }

  // Send header
  _send(fd, 4, size_bytes);

  // Send body
  _send(fd, msg.size, msg.data);

  // Send footer
  _send(fd, SENTINEL_SIZE, (uint8_t *)SENTINEL);
}

static const int RECEIVE_FLAGS = 0x0;
static uint8_t *_receive(int fd, size_t size) {
  uint8_t *buffer = malloc(size);
  uint8_t *buffer_ptr = buffer;
  size_t bytes_to_read = size;

  while (bytes_to_read > 0) {
    ssize_t n = recv(fd, buffer_ptr, size, RECEIVE_FLAGS);
    if (n == -1) {
      // TODO: pass error back
      fprintf(stderr, "Failed to receive from socket: %s\n", strerror(errno));
      exit(1);
    }
    bytes_to_read -= n;
    buffer_ptr += n;
  }

  return buffer;
}

Message receive_message(int fd) {
  // Receive header
  uint32_t size = 0;
  {
    uint8_t *_size_bytes = _receive(fd, 4);

    for (int i = 0; i < 4; i++) {
      size = size << 8;
      size = _size_bytes[i];
    }

    free(_size_bytes);
  }

  // Receive body
  uint8_t *data = _receive(fd, size);

  // Receive footer
  {
    char *footer = (char *)_receive(fd, SENTINEL_SIZE);

    if (strncmp(footer, SENTINEL, SENTINEL_SIZE) != 0) {
      fprintf(stderr, "Error! Did not receive expected footer\n");
      exit(1);
    }

    free(footer);
  }

  return (Message){
      .size = size,
      .data = data,
  };
}
