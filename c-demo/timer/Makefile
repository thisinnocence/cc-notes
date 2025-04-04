# Targets
TARGETS = timer.out

# Include directories
INC_DIRS = ../util
INC_FLAGS = $(addprefix -I, $(INC_DIRS))

# compile lags： -MMD -MP is used for generate .d file
CFLAGS = $(INC_FLAGS) -MMD -MP -Wall -Wextra -g

# link flags:
LDFLAGS = -lrt -lpthread

# Source, object, and dependency files
SRCS = $(wildcard *.c)
OBJS = $(SRCS:.c=.o)
DEPS = $(SRCS:.c=.d)

# Phony targets
.PHONY: all clean

# Default target
all: $(TARGETS)

# Include dependency files
-include $(DEPS)

# when Makefile change, re-compile all
$(OBJS): Makefile

# 下面这条规则是Makefile的隐式规则，默认就有，可以省略
# %.o: %.c
#	$(CC) $(CFLAGS) -c $< -o $@

# Linking
timer.out: $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

clean:
	rm -f $(OBJS) $(TARGETS) $(DEPS)
