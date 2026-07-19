# := means simple assign, evaluate only once
CC := clang
CFLAGS := -g -O0 -Wall -Werror -Wextra -Wpedantic -I$(PWD)/include/

SRCS := $(wildcard src/*.c)
DEPDIR := .deps
DEPFILES := $(SRCS:src/%.c=$(DEPDIR)/%.d)

# -MMD = generate dep file as a side-effect of compilation sans system headers
# -MT  = set the target name in generated dep file
# -MP  = adds a target for each prereq in the list, to avoid errors when
#        deleting files (?)
# -MF  = write the depfile
# $*   = The stem of the target filename. A stem is typically a filename
#        without its suffix. Its use outside of pattern rules is discouraged.
DEPFLAGS = -MT $@ -MMD -MP -MF $(DEPDIR)/$*.d

.PHONY: all clean

all: build/server build/client build/chat-server

# Declare so make won't file when they don't exist; will be generated when
# objects get compiled.
$(DEPFILES):

include $(wildcard $(DEPFILES))

# OUTPUT_OPTION implicit rule
build/%.o: src/%.c
	$(CC) $(DEPFLAGS) $(CFLAGS) -c $< $(OUTPUT_OPTION)

build/%.o: bin/%.c
	$(CC) $(DEPFLAGS) $(CFLAGS) -c $< $(OUTPUT_OPTION)

build/server: build/server.o build/tcp.o build/message.o build/connections.o
	$(CC) $(CFLAGS) $^ -o $@

build/client: build/client.o build/tcp.o build/message.o
	$(CC) $(CFLAGS) $^ -o $@

build/chat-server: build/chat-server.o build/connections.o
	$(CC) $(CFLAGS) $^ -o $@

clean:
	rm -f build/*.o build/server build/client
