CC = gcc
BASE_CFLAGS = -Wall -Wextra -Werror -pedantic -std=c99 -O2
LIBS = -lgmp

SRC = fib.c algorithms.c matrix.c utils.c
OBJ = $(SRC:.c=.o)
TARGET = fib

UNAME_S := $(shell uname -s)

ifeq ($(UNAME_S),Darwin)
    HOMEBREW_PREFIX := $(shell brew --prefix 2>/dev/null || echo "/opt/homebrew")
    CFLAGS = $(BASE_CFLAGS) -I$(HOMEBREW_PREFIX)/include
    LDFLAGS = -L$(HOMEBREW_PREFIX)/lib
endif

ifeq ($(UNAME_S),Linux)
    CFLAGS = $(BASE_CFLAGS)
    LDFLAGS =
endif

ifndef CFLAGS
    CFLAGS = $(BASE_CFLAGS)
endif

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^ $(LIBS)

%.o: %.c fib.h
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ) $(TARGET)

cleanobj:
	rm -f $(OBJ)

install-deps:
ifeq ($(UNAME_S),Darwin)
	@echo "Installing dependencies for macOS..."
	@which brew > /dev/null || { echo "Error: Homebrew is not installed"; exit 1; }
	@brew install gmp
else ifeq ($(UNAME_S),Linux)
	@echo "Installing dependencies for Linux..."
	@which apt-get > /dev/null && { sudo apt-get update && sudo apt-get install -y libgmp-dev; } || \
	{ which dnf > /dev/null && { sudo dnf install -y gmp-devel; } || \
		{ echo "Could not detect a compatible package manager"; exit 1; }; }
else
	@echo "Incompatible operating system"
	@exit 1
endif

.PHONY: all clean cleanobj install-deps
