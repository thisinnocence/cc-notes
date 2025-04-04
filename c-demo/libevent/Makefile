CC = gcc

CFLAGS = -Wall -Wextra -g -I../../util
LDFLAGS = -levent

TARGETS = event.out

all: $(TARGETS)

SRCS = libevent.c
OBJS = $(SRCS:.c=.o)
DEPS = $(SRCS:.c=.d)

-include $(DEPS)

%.o: %.c %.d
	$(CC) $(CFLAGS) -c $< -o $@

%.d: %.c
	@set -e; \
	$(CC) $(CFLAGS) -MM $< | \
	sed 's,\($*\)\.o[ :]*,\1.o $@ : ,g' > $@

$(TARGETS): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

clean:
	rm -f $(OBJS) $(TARGETS) $(DEPS)

