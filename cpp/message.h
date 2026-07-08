#ifndef _MESSAGE_H
#define _MESSAGE_H

#include <optional> // std::optional
#include <stdint.h> // uint32_t

struct Message {
  uint32_t size; // up to 4gb
  void *data;
};

Message message_of_pointer(void *ptr, uint32_t size);

void free_message(Message msg);

void send_message(int fd, Message msg);

std::optional<Message> receive_message(int fd);

#endif // #ifndef _MESSAGE_H
