.PHONY: run

CC = clang
CFLAGS = -g -Wall -Werror -Wextra -Wpedantic

run: server client
	./client &
	./server

server: server.o tcp.o message.o connections.o
	$(CC) $(CFLAGS) $^ -o $@

client: client.o tcp.o message.o
	$(CC) $(CFLAGS) $^ -o $@

connections.o: connections.c connections.h
	$(CC) $(CFLAGS) -c $< -o $@

server.o: server.c connections.h message.h tcp.h
	$(CC) $(CFLAGS) -c $< -o $@

client.o: client.c message.h tcp.h
	$(CC) $(CFLAGS) -c $< -o $@

message.o: message.c message.h
	$(CC) $(CFLAGS) -c $< -o $@

tcp.o: tcp.c tcp.h
	$(CC) $(CFLAGS) -c $< -o $@
