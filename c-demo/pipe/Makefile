# Targets
TARGETS = pipe.out

# Use pkg-config to get the GLib flags
PKG_CFLAGS = $(shell pkg-config --cflags glib-2.0)
PKG_LDFLAGS = $(shell pkg-config --libs glib-2.0)

# Include directories
INC_DIRS = ../util /usr/include/glib-2.0
INC_FLAGS = $(addprefix -I, $(INC_DIRS))
INC_FLAGS = $(addprefix -I, $(INC_DIRS))

# compile lags： -MMD -MP is used for generate .d file
CFLAGS = $(INC_FLAGS) -MMD -MP -Wall -Wextra -g $(PKG_CFLAGS)

# link flags:
LDFLAGS = -lrt -lpthread $(PKG_LDFLAGS)

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

# Linking
$(TARGETS): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

clean:
	rm -f $(OBJS) $(TARGETS) $(DEPS)
