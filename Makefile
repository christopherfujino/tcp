.PHONY: run

CC = clang

run: server client
	./client &
	./server

server: server.o tcp.o
	$(CC) $^ -o $@

client: client.o tcp.o
	$(CC) $^ -o $@

server.o: server.c tcp.h
	$(CC) -c $< -o $@

client.o: client.c tcp.h
	$(CC) -c $< -o $@
