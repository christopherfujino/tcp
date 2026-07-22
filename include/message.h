#ifndef _MESSAGE_H
#define _MESSAGE_H

#include <stdint.h> // uint32_t

typedef struct Message {
  uint32_t size; // up to 4gb
  void *data;
} Message;

typedef enum Result {
  ResultOk,
  ResultError,
  ResultEOF,
} Result;

Message message_of_pointer(void *ptr, uint32_t size);

void free_message(Message msg);

void send_message(int fd, Message msg);

Result receive_message(int fd, Message *message);

#endif // #ifndef _MESSAGE_H
