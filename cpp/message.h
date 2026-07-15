#ifndef _MESSAGE_H
#define _MESSAGE_H

#include <optional> // std::optional
#include <stdint.h> // uint32_t

class Message {
public:
  uint32_t size; // up to 4gb
  void *data;

  static std::optional<Message> receive(int fd);
};

Message message_of_pointer(void *ptr, uint32_t size);

void free_message(Message msg);

void send_message(int fd, Message msg);

#endif // #ifndef _MESSAGE_H
