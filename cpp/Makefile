.PHONY: run

CC = clang++
CFLAGS = -g -std=c++20 -Wall -Werror -Wextra -Wpedantic

run:
	./test.sloth

server: server.o tcp.o message.o
	$(CC) $(CFLAGS) $^ -o $@

client: client.o tcp.o message.o
	$(CC) $(CFLAGS) $^ -o $@

server_entrypoint.o: server_entrypoint.cc
	$(CC) $(CFLAGS) -c $< -o $@

server.o: server.cc tcp.h
	$(CC) $(CFLAGS) -c $< -o $@

client.o: client.cc tcp.h
	$(CC) $(CFLAGS) -c $< -o $@

message.o: message.cc message.h
	$(CC) $(CFLAGS) -c $< -o $@

tcp.o: tcp.cc tcp.h
	$(CC) $(CFLAGS) -c $< -o $@
