CC = clang
CFLAGS = -g -O0 -Wall -Werror -Wextra -Wpedantic -I./include/

.PHONY: all run

all: build/server build/client

run: build/server build/client
	./client &
	./server

build/server: build/server.o build/tcp.o build/message.o build/connections.o
	$(CC) $(CFLAGS) $^ -o $@

build/client: build/client.o build/tcp.o build/message.o
	$(CC) $(CFLAGS) $^ -o $@

build/connections.o: src/connections.c include/connections.h
	$(CC) $(CFLAGS) -c $< -o $@

build/server.o: bin/server.c include/connections.h include/message.h include/tcp.h
	$(CC) $(CFLAGS) -c $< -o $@

build/client.o: bin/client.c include/message.h include/tcp.h
	$(CC) $(CFLAGS) -c $< -o $@

build/message.o: src/message.c include/message.h
	$(CC) $(CFLAGS) -c $< -o $@

build/tcp.o: src/tcp.c include/tcp.h
	$(CC) $(CFLAGS) -c $< -o $@
