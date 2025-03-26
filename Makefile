CC = gcc
BASE_CFLAGS = -Wall -Wextra -Werror -pedantic -std=c99 -O2
LIBS = -lgmp

SRC = fib.c algorithms.c matrix.c utils.c
OBJ = $(SRC:.c=.o)
DEP = $(SRC:.c=.d)
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

ifdef DEBUG
	CFLAGS += -g -DDEBUG
	LDFLAGS += -g
endif

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^ $(LIBS)

%.o: %.c
	$(CC) $(CFLAGS) -MMD -MP -c $< -o $@

-include $(DEP)

clean:
	rm -f $(OBJ) $(DEP) $(TARGET)

cleanobj:
	rm -f $(OBJ) $(DEP)

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

debug:
	$(MAKE) DEBUG=1

rebuild: clean all

test: all
	@echo "Running tests..."
	./$(TARGET) 10
	./$(TARGET) 20 -a matrix -t
	@echo "Tests completed"

.PHONY: all clean cleanobj install-deps debug rebuild test