#include <errno.h>      // errno
#include <optional>     // std::optional
#include <stdint.h>     // uint32_t
#include <stdio.h>      // fprintf(), stderr
#include <stdlib.h>     // malloc()
#include <string.h>     // memcpy(), strerror()
#include <sys/socket.h> // send()

#include "message.h"

static const int _SEND_FLAGS = 0x0;
static char __SENTINEL[] = "flooboo";
static char *_SENTINEL = __SENTINEL;
static const int _SENTINEL_SIZE = 7;

static const int _RECEIVE_FLAGS = 0x0;

Message message_of_pointer(void *ptr, uint32_t size) {
  void *buffer = malloc(size);
  memcpy(buffer, ptr, size);
  return Message{
      .size = size,
      .data = buffer,
  };
}

void free_message(Message msg) { free(msg.data); }

static void _send(int fd, size_t bytes_to_send, void *send_ptr) {
  while (bytes_to_send > 0) {
    ssize_t n = send(fd, send_ptr, bytes_to_send, _SEND_FLAGS);
    if (n == -1) {
      // TODO return error to caller to handle cleanup
      fprintf(stderr, "Failed to send message\n");
      exit(1);
    }
    bytes_to_send -= n;
    send_ptr = (uint8_t *)send_ptr + n;
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
  _send(fd, _SENTINEL_SIZE, (uint8_t *)_SENTINEL);
}

static std::optional<uint8_t *> _receive(int fd, size_t size) {
  uint8_t *buffer = (uint8_t *)malloc(size);
  uint8_t *buffer_ptr = buffer;
  size_t bytes_to_read = size;

  while (bytes_to_read > 0) {
    ssize_t n = recv(fd, buffer_ptr, size, _RECEIVE_FLAGS);
    if (n == -1) {
      // TODO: pass error back
      fprintf(stderr, "Failed to receive from socket: %s\n", strerror(errno));
      throw 420; // TODO
    } else if (n == 0) {
      return std::optional<uint8_t *>{};
    }
    bytes_to_read -= n;
    buffer_ptr += n;
  }

  return std::optional(buffer);
}

std::optional<Message> Message::receive(int fd) {
  // Receive header
  uint32_t size = 0;
  {
    auto _size_bytes_opt = _receive(fd, 4);
    if (!_size_bytes_opt.has_value()) {
      return std::optional<Message>{};
    }
    auto _size_bytes = _size_bytes_opt.value();

    for (int i = 0; i < 4; i++) {
      size = size << 8;
      size = _size_bytes[i];
    }

    free(_size_bytes);
  }

  // Receive body
  uint8_t *data = nullptr;
  {
    auto _data_opt = _receive(fd, size);
    data = _data_opt.value();
  }

  // Receive footer
  {
    char *footer = nullptr;
    {
      auto footer_opt = _receive(fd, _SENTINEL_SIZE);
      footer = (char *)footer_opt.value();
    }

    if (strncmp(footer, _SENTINEL, _SENTINEL_SIZE) != 0) {
      fprintf(stderr, "Error! Did not receive expected footer\n");
      exit(1);
    }

    free(footer);
  }

  return std::optional<Message>(Message{
      .size = size,
      .data = data,
  });
}
