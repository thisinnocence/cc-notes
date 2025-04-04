CC := gcc
CFLAGS := -Wall -Wextra -g -Werror -std=c99

SRCS := $(wildcard *.c*)
OBJS := $(SRCS:.c=.o)
TARGET := a.out

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) $^ -o $@

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)

