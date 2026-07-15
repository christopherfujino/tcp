#include <errno.h>      // errno
#include <stdint.h>     // uint32_t
#include <stdio.h>      // fprintf(), stderr
#include <stdlib.h>     // malloc()
#include <string.h>     // memcpy(), strerror()
#include <sys/socket.h> // send()

#include "message.h"

static const int _SEND_FLAGS = 0x0;
static char *_SENTINEL = "flooboo";
static const int _SENTINEL_SIZE = 7;

static const int _RECEIVE_FLAGS = 0x0;

Message message_of_pointer(void *ptr, uint32_t size) {
  void *buffer = malloc(size);
  memcpy(buffer, ptr, size);
  return (Message){
      .size = size,
      .data = buffer,
  };
}

void free_message(Message msg) { free(msg.data); }

static void _send(int fd, size_t bytes_to_send, uint8_t *send_ptr) {
  while (bytes_to_send > 0) {
    ssize_t n = send(fd, send_ptr, bytes_to_send, _SEND_FLAGS);
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
  _send(fd, _SENTINEL_SIZE, (uint8_t *)_SENTINEL);
}

static Result _receive(int fd, size_t size, uint8_t **pp) {
  uint8_t *buffer = malloc(size);
  uint8_t *buffer_ptr = buffer;
  size_t bytes_to_read = size;

  while (bytes_to_read > 0) {
    ssize_t n = recv(fd, buffer_ptr, size, _RECEIVE_FLAGS);
    if (n == -1) {
      // TODO: pass error back
      fprintf(stderr, "Failed to receive from socket: %s\n", strerror(errno));
      free(buffer);
      return ResultError;
    } else if (n == 0) {
      free(buffer);
      return ResultEOF;
    }
    bytes_to_read -= n;
    buffer_ptr += n;
  }

  *pp = buffer;
  return ResultOk;
}

Result receive_message(int fd, Message *message) {
  Result result;

  // Receive header
  uint32_t size = 0;
  {
    uint8_t *buffer = NULL;
    result = _receive(fd, 4, &buffer);
    switch (result) {
    case ResultOk:
      break;
    case ResultEOF:
    case ResultError:
      return result;
    }

    for (int i = 0; i < 4; i++) {
      size = size << 8;
      size = buffer[i];
    }

    free(buffer);
  }

  // Receive body
  uint8_t *body = NULL;
  result = _receive(fd, size, &body);
  switch (result) {
  case ResultOk:
    break;
  case ResultEOF:
  case ResultError:
    return result;
  }

  // Receive footer
  {
    char *footer = NULL;
    result = _receive(fd, _SENTINEL_SIZE, (uint8_t **)&footer);
    switch (result) {
    case ResultOk:
      break;
    case ResultEOF:
    case ResultError:
      return result;
    }

    if (strncmp(footer, _SENTINEL, _SENTINEL_SIZE) != 0) {
      fprintf(stderr, "Error! Did not receive expected footer\n");
      exit(1);
    }

    free(footer);
  }

  message->size = size;
  message->data = body;

  return ResultOk;
}
