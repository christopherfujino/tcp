.PHONY: run

run: server
	./server

server: server.c
	clang $< -o $@
