CC = clang
CFLAGS = -g -O0 -Wall -Werror -Wextra -Wpedantic -I$(PWD)/include/

.PHONY: all _all run

all: all_but_compile_commands compile_commands.json

all_but_compile_commands: build/server build/client build/chat-server

run: build/server build/client
	./client &
	./server

build/server: build/server.o build/tcp.o build/message.o build/connections.o
	$(CC) $(CFLAGS) $^ -o $@

build/client: build/client.o build/tcp.o build/message.o
	$(CC) $(CFLAGS) $^ -o $@

build/chat-server: build/chat-server.o build/connections.o
	$(CC) $(CFLAGS) $^ -o $@

build/chat-server.o: bin/chat-server.c include/chris-tcp.h
	$(CC) $(CFLAGS) -c $< -o $@

include/chris-tcp.h: include/connections.h
	touch $@

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

compile_commands.json: Makefile
	make clean
	bear -- make all_but_compile_commands

clean:
	rm -f build/*.o build/server build/client
