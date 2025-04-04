SRCS = $(wildcard *.c)
TARGETS = $(SRCS:.c=.out)

.PHONY: all
all: $(TARGETS)

CFLAGS  = -MMD -MP -Wall -g

OBJS = $(SRCS:.c=.o)
DEPS = $(SRCS:.c=.d)
-include $(DEPS)

%.out: %.o
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

.PHONY: clean
clean:
	rm -f *.o *.d *.out
