# Check if .config exists
ifeq ($(wildcard .config),)
$(error .config not found. Please run kconfig_parser.py first)
endif

# Load .config
include .config

CFLAGS := -Werror

# Parse .config to generate -D macros
ifeq ($(CONFIG_FEATURE_A), y)
	CFLAGS += -DCONFIG_FEATURE_A
endif
ifeq ($(CONFIG_FEATURE_B), y)
	CFLAGS += -DCONFIG_FEATURE_B
endif

.PHONY: all clean

# Build targets
all: a.out

a.out: main.c
	$(CC) $(CFLAGS) main.c -o $@

clean:
	rm -vf *.out
