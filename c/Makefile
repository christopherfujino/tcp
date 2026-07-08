.PHONY: run

CC = clang++
CFLAGS = -g -Wall -Werror -Wextra -Wpedantic

run: server client
	./client &
	./server

server: server.o tcp.o message.o
	$(CC) $(CFLAGS) $^ -o $@

client: client.o tcp.o message.o
	$(CC) $(CFLAGS) $^ -o $@

server.o: server.c tcp.h
	$(CC) $(CFLAGS) -c $< -o $@

client.o: client.c tcp.h
	$(CC) $(CFLAGS) -c $< -o $@

message.o: message.c message.h
	$(CC) $(CFLAGS) -c $< -o $@

tcp.o: tcp.c tcp.h
	$(CC) $(CFLAGS) -c $< -o $@
