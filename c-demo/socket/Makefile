# `bear make` can generate compile_commands.json for clangd

TARGETS = server.out client.out

# Note: 这个顺序不能放在 -include $(DEPS) 之后，会导致server.c更新不触发增量编译
.PHONY: all
all: $(TARGETS)

CFLAGS = -MMD -MP -Wall -Wextra -g -I../util
LDFLAGS = -lrt -lpthread

SRCS = $(wildcard *.c)
OBJS = $(SRCS:.c=.o)
DEPS = $(SRCS:.c=.d)
-include $(DEPS)

# Build server executable
server.out: server.o
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

# Build client executable
client.out: client.o
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

.PHONY: clean
clean:
	rm -f $(OBJS) $(TARGETS) $(DEPS)
