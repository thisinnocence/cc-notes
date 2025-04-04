# Targets
TARGETS = sem.out

# Include directories
INC_DIRS = ../util
INC_FLAGS = $(addprefix -I, $(INC_DIRS))

# compile flags： -MMD -MP is used for generate .d file
CFLAGS = $(INC_FLAGS) -MMD -MP -Wall -Wextra -g

# link flags:
LDFLAGS = -lrt -lpthread

# Source, object, and dependency files
SRCS = $(wildcard *.c)
OBJS = $(SRCS:.c=.o)
DEPS = $(SRCS:.c=.d)

# Default target
.PHONY: all
all: $(TARGETS)

# Include dependency files
-include $(DEPS)

# when Makefile change, re-compile all
$(OBJS): Makefile

# Linking
sem.out: $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

.PHONY: clean
clean:
	rm -f $(OBJS) $(TARGETS) $(DEPS)
