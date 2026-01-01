PROJECT_NAME = fib
VERSION = 0.4.0
AUTHOR = Rodrigo Belloso

# Compiler and tools
CC = gcc
AR = ar
INSTALL = install
SHELL = /bin/bash

# Build ID generation
BUILD_ID_HEADER = build_id.h
BUILD_ID_GEN = ./buildID.sh

# Directories
PREFIX ?= /usr/local
BINDIR = $(PREFIX)/bin
MANDIR = $(PREFIX)/share/man/man1
SRCDIR = .
TESTDIR = tests
BUILDDIR = build

# Source files
SRC = fib.c algorithms.c matrix.c utils.c ui.c
OBJ = $(SRC:.c=.o)
DEP = $(SRC:.c=.d)
TARGET = $(PROJECT_NAME)

# Libraries
LIBS = -lgmp -lncurses

# Detect operating system
UNAME_S := $(shell uname -s)
UNAME_M := $(shell uname -m)

# =============================================================================
# Compiler Flags Configuration
# =============================================================================

# Base flags
BASE_CFLAGS = -Wall -Wextra -Werror -pedantic -std=c99
BASE_CFLAGS += -Wformat=2 -Wformat-security -Wno-unused-parameter
BASE_CFLAGS += -Wcast-align -Wstrict-prototypes -Wmissing-prototypes
# Note: -Wconversion and -Wsign-conversion are too strict for GMP library
# Uncomment if needed: BASE_CFLAGS += -Wconversion -Wsign-conversion

# Optimization level (can be overridden: make OPT_LEVEL=-O3)
OPT_LEVEL ?= -O2

# Default CFLAGS
CFLAGS = $(BASE_CFLAGS) $(OPT_LEVEL) -DVERSION=\"$(VERSION)\"

# Platform-specific settings
ifeq ($(UNAME_S),Darwin)
	HOMEBREW_PREFIX := $(shell brew --prefix 2>/dev/null || echo "/opt/homebrew")
	CFLAGS += -I$(HOMEBREW_PREFIX)/include
	LDFLAGS = -L$(HOMEBREW_PREFIX)/lib
	INSTALL_STRIP = -S
endif

ifeq ($(UNAME_S),Linux)
	# Check if pkg-config is available for ncurses
	NCURSES_CFLAGS := $(shell pkg-config --cflags ncurses 2>/dev/null || echo "")
	NCURSES_LIBS := $(shell pkg-config --libs ncurses 2>/dev/null || echo "-lncurses")
	CFLAGS += $(NCURSES_CFLAGS)
	LDFLAGS = $(NCURSES_LIBS)
	INSTALL_STRIP = -s
endif

# =============================================================================
# Build Modes
# =============================================================================

# Debug mode
ifdef DEBUG
	CFLAGS += -g3 -DDEBUG -O0 -fno-omit-frame-pointer
	LDFLAGS += -g3
	OPT_LEVEL = -O0
endif

# Release mode
ifdef RELEASE
	CFLAGS += -DNDEBUG -ffunction-sections -fdata-sections
	LDFLAGS += -Wl,--gc-sections
	OPT_LEVEL = -O3
endif

# Profile mode (for gprof)
ifdef PROFILE
	CFLAGS += -pg -g
	LDFLAGS += -pg
endif

# Sanitizers
ifdef ASAN
	CFLAGS += -fsanitize=address -fno-omit-frame-pointer -g
	LDFLAGS += -fsanitize=address
endif

ifdef UBSAN
	CFLAGS += -fsanitize=undefined -fno-omit-frame-pointer -g
	LDFLAGS += -fsanitize=undefined
endif

ifdef MSAN
	CFLAGS += -fsanitize=memory -fno-omit-frame-pointer -g
	LDFLAGS += -fsanitize=memory
endif

ifdef TSAN
	CFLAGS += -fsanitize=thread -fno-omit-frame-pointer -g
	LDFLAGS += -fsanitize=thread
endif

# Coverage
ifdef COVERAGE
	CFLAGS += --coverage -O0
	LDFLAGS += --coverage
endif

# Static analysis
ifdef ANALYZE
	CC = clang --analyze
endif

# =============================================================================
# Build Targets
# =============================================================================

.DEFAULT_GOAL := all

all: gcc banner $(BUILD_ID_HEADER) $(TARGET)
	@echo "✓ Build complete: $(TARGET) ($(VERSION))"

$(BUILD_ID_HEADER):
	@echo "Generating build ID..."
	@chmod +x $(BUILD_ID_GEN)
	@$(BUILD_ID_GEN) > $(BUILD_ID_HEADER)

gcc:
	@command -v $(CC) >/dev/null 2>&1 || { echo "Error: gcc is required but not installed. Please install gcc."; exit 1; }

banner:
	@echo "════════════════════════════════════════════════════════"
	@echo "  Building $(PROJECT_NAME) v$(VERSION)"
	@echo "  Build ID will be generated during compilation"
	@echo "  Platform: $(UNAME_S) $(UNAME_M)"
	@echo "  Compiler: $(CC)"
	@echo "  Flags: $(CFLAGS)"
	@echo "════════════════════════════════════════════════════════"

$(TARGET): $(OBJ)
	@echo "Linking $(TARGET)..."
	@$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^ $(LIBS)

%.o: %.c
	@echo "Compiling $<..."
	@$(CC) $(CFLAGS) -MMD -MP -c $< -o $@

-include $(DEP)

# =============================================================================
# Build Variants
# =============================================================================

debug:
	@echo "Building in DEBUG mode..."
	@$(MAKE) DEBUG=1 clean all

release:
	@echo "Building in RELEASE mode..."
	@$(MAKE) RELEASE=1 clean all

profile:
	@echo "Building in PROFILE mode..."
	@$(MAKE) PROFILE=1 clean all

asan:
	@echo "Building with AddressSanitizer..."
	@$(MAKE) ASAN=1 clean all

ubsan:
	@echo "Building with UndefinedBehaviorSanitizer..."
	@$(MAKE) UBSAN=1 clean all

msan:
	@echo "Building with MemorySanitizer..."
	@$(MAKE) MSAN=1 clean all

tsan:
	@echo "Building with ThreadSanitizer..."
	@$(MAKE) TSAN=1 clean all

coverage:
	@echo "Building with coverage instrumentation..."
	@$(MAKE) COVERAGE=1 clean all

# =============================================================================
# Installation Targets
# =============================================================================

install: $(TARGET)
	@echo "Installing $(TARGET) to $(BINDIR)..."
	@$(INSTALL) -d $(BINDIR)
	@$(INSTALL) $(INSTALL_STRIP) -m 755 $(TARGET) $(BINDIR)/$(TARGET)
	@echo "✓ Installation complete"
	@echo "  Binary: $(BINDIR)/$(TARGET)"
	@echo "  Run with: $(TARGET) --help"

uninstall:
	@echo "Uninstalling $(TARGET) from $(BINDIR)..."
	@rm -f $(BINDIR)/$(TARGET)
	@echo "✓ Uninstallation complete"

# =============================================================================
# Testing Targets
# =============================================================================

test: all
	@echo "════════════════════════════════════════════════════════"
	@echo "  Running basic tests..."
	@echo "════════════════════════════════════════════════════════"
	@./$(TARGET) 10
	@./$(TARGET) 20 -a matrix -t
	@echo "✓ Basic tests completed"

test-full: all
	@echo "════════════════════════════════════════════════════════"
	@echo "  Running comprehensive test suite..."
	@echo "════════════════════════════════════════════════════════"
	@if [ -f $(TESTDIR)/build.sh ]; then \
		cd $(TESTDIR) && bash build.sh; \
	else \
		echo "Warning: Test script not found at $(TESTDIR)/build.sh"; \
	fi

test-valgrind: all
	@echo "Running tests with Valgrind..."
	@if ! command -v valgrind >/dev/null 2>&1; then \
		echo ""; \
		echo "════════════════════════════════════════════════════════"; \
		echo "  ERROR: valgrind is not installed"; \
		echo "════════════════════════════════════════════════════════"; \
		echo ""; \
		echo "Please install valgrind to run memory leak tests."; \
		echo ""; \
		exit 1; \
	fi
	@valgrind --leak-check=full --show-leak-kinds=all \
		--track-origins=yes --verbose \
		./$(TARGET) 100
	@echo "✓ Valgrind tests completed"

test-sanitizers: asan-test ubsan-test

asan-test:
	@echo "Running tests with AddressSanitizer..."
	@$(MAKE) asan
	@./$(TARGET) 100
	@echo "✓ AddressSanitizer tests passed"

ubsan-test:
	@echo "Running tests with UndefinedBehaviorSanitizer..."
	@$(MAKE) ubsan
	@./$(TARGET) 100
	@echo "✓ UndefinedBehaviorSanitizer tests passed"

# =============================================================================
# Code Quality Targets
# =============================================================================

lint: lint-c lint-shell

lint-c:
	@echo "Checking C code formatting..."
	@if ! command -v clang-format >/dev/null 2>&1; then \
		echo ""; \
		echo "════════════════════════════════════════════════════════"; \
		echo "  WARNING: clang-format is not installed"; \
		echo "════════════════════════════════════════════════════════"; \
		echo ""; \
		echo "Skipping C code formatting checks."; \
		echo "Install clang-format to enable this check."; \
		echo ""; \
		exit 0; \
	fi
	@clang-format --dry-run --Werror $(SRC) fib.h || true

lint-shell:
	@echo "Running shell script linter..."
	@if [ -f $(TESTDIR)/lint.sh ]; then \
		cd $(TESTDIR) && bash lint.sh; \
	else \
		echo "Warning: Lint script not found"; \
	fi

format:
	@echo "Formatting source code..."
	@if ! command -v clang-format >/dev/null 2>&1; then \
		echo ""; \
		echo "════════════════════════════════════════════════════════"; \
		echo "  ERROR: clang-format is not installed"; \
		echo "════════════════════════════════════════════════════════"; \
		echo ""; \
		echo "Please install clang-format to format C source code."; \
		echo ""; \
		exit 1; \
	fi
	@clang-format -i $(SRC) fib.h
	@echo "✓ Code formatted"

check-format:
	@echo "Checking code formatting..."
	@if ! command -v clang-format >/dev/null 2>&1; then \
		echo ""; \
		echo "════════════════════════════════════════════════════════"; \
		echo "  ERROR: clang-format is not installed"; \
		echo "════════════════════════════════════════════════════════"; \
		echo ""; \
		echo "Please install clang-format to check code formatting."; \
		echo ""; \
		exit 1; \
	fi
	@clang-format --dry-run -Werror $(SRC) fib.h
	@echo "✓ Code formatting is correct"

analyze:
	@echo "Running static analysis..."
	@$(MAKE) ANALYZE=1 clean
	@$(MAKE) ANALYZE=1

cppcheck:
	@echo "Running cppcheck..."
	@if ! command -v cppcheck >/dev/null 2>&1; then \
		echo ""; \
		echo "════════════════════════════════════════════════════════"; \
		echo "  ERROR: cppcheck is not installed"; \
		echo "════════════════════════════════════════════════════════"; \
		echo ""; \
		echo "Please install cppcheck to run static analysis."; \
		echo ""; \
		exit 1; \
	fi
	@cppcheck --enable=all --inconclusive --std=c99 $(SRC)

# =============================================================================
# Dependency Management
# =============================================================================

check-deps:
	@echo "Checking dependencies..."
	@echo -n "  GMP library: "
	@echo "#include <gmp.h>" | $(CC) -E - -o /dev/null 2>/dev/null && \
		echo "✓ Found" || echo "✗ Not found"
	@echo -n "  GCC/Clang: "
	@$(CC) --version > /dev/null 2>&1 && echo "✓ Found" || echo "✗ Not found"
	@echo ""
	@echo "Optional development tools:"
	@echo -n "  clang-format: "
	@command -v clang-format >/dev/null 2>&1 && echo "✓ Found" || echo "✗ Not found"
	@echo -n "  cppcheck: "
	@command -v cppcheck >/dev/null 2>&1 && echo "✓ Found" || echo "✗ Not found"
	@echo -n "  valgrind: "
	@command -v valgrind >/dev/null 2>&1 && echo "✓ Found" || echo "✗ Not found"
	@echo -n "  shellcheck: "
	@command -v shellcheck >/dev/null 2>&1 && echo "✓ Found" || echo "✗ Not found"

install-deps:
	@echo "════════════════════════════════════════════════════════"
	@echo "  Installing Build Dependencies"
	@echo "════════════════════════════════════════════════════════"
	@echo ""
ifeq ($(UNAME_S),Darwin)
	@echo "Platform: macOS"
	@if ! command -v brew >/dev/null 2>&1; then \
		echo ""; \
		echo "ERROR: Homebrew is not installed"; \
		echo "Please install Homebrew to continue."; \
		echo ""; \
		exit 1; \
	fi
	@echo "Installing GMP and ncurses libraries..."
	@brew install gmp ncurses
	@echo ""
	@echo "✓ Build dependencies installed"
else ifeq ($(UNAME_S),Linux)
	@echo "Platform: Linux"
	@if command -v apt-get >/dev/null 2>&1; then \
		echo "Installing dependencies..."; \
		sudo apt-get update && \
		sudo apt-get install -y libgmp-dev libncurses-dev build-essential; \
		echo ""; \
		echo "✓ Build dependencies installed"; \
	elif command -v dnf >/dev/null 2>&1; then \
		echo "Installing dependencies..."; \
		sudo dnf install -y gmp-devel ncurses-devel gcc make; \
		echo ""; \
		echo "✓ Build dependencies installed"; \
	elif command -v pacman >/dev/null 2>&1; then \
		echo "Installing dependencies..."; \
		sudo pacman -S --noconfirm gmp ncurses base-devel; \
		echo ""; \
		echo "✓ Build dependencies installed"; \
	else \
		echo ""; \
		echo "ERROR: Could not detect a compatible package manager"; \
		echo "Please install GMP and ncurses libraries and build tools manually."; \
		echo ""; \
		exit 1; \
	fi
else
	@echo "ERROR: Unsupported operating system: $(UNAME_S)"
	@exit 1
endif
	@echo ""
	@echo "════════════════════════════════════════════════════════"

install-debug-deps: install-deps
	@echo "════════════════════════════════════════════════════════"
	@echo "  Installing Debug & Development Tools"
	@echo "════════════════════════════════════════════════════════"
	@echo ""
ifeq ($(UNAME_S),Darwin)
	@echo "Platform: macOS"
	@if ! command -v brew >/dev/null 2>&1; then \
		echo ""; \
		echo "ERROR: Homebrew is not installed"; \
		echo "Please install Homebrew to continue."; \
		echo ""; \
		exit 1; \
	fi
	@echo "Installing development tools..."
	@brew install clang-format cppcheck shellcheck
	@echo ""
	@echo "✓ Debug & development tools installed"
	@echo ""
	@echo "Note: valgrind may not be available or fully supported"
else ifeq ($(UNAME_S),Linux)
	@echo "Platform: Linux"
	@if command -v apt-get >/dev/null 2>&1; then \
		echo "Installing development tools..."; \
		sudo apt-get install -y clang-format cppcheck valgrind shellcheck; \
		echo ""; \
		echo "✓ Debug & development tools installed"; \
	elif command -v dnf >/dev/null 2>&1; then \
		echo "Installing development tools..."; \
		sudo dnf install -y clang-tools-extra cppcheck valgrind ShellCheck; \
		echo ""; \
		echo "✓ Debug & development tools installed"; \
	elif command -v pacman >/dev/null 2>&1; then \
		echo "Installing development tools..."; \
		sudo pacman -S --noconfirm clang cppcheck valgrind shellcheck; \
		echo ""; \
		echo "✓ Debug & development tools installed"; \
	else \
		echo ""; \
		echo "ERROR: Could not detect a compatible package manager"; \
		echo "Please install development tools manually."; \
		echo ""; \
		exit 1; \
	fi
else
	@echo "ERROR: Unsupported operating system: $(UNAME_S)"
	@exit 1
endif
	@echo ""
	@echo "════════════════════════════════════════════════════════"

# =============================================================================
# Cleanup Targets
# =============================================================================

clean:
	@echo "Cleaning build artifacts..."
	@rm -f $(OBJ) $(DEP) $(TARGET)
	@rm -f $(BUILD_ID_HEADER)
	@rm -f *.gcda *.gcno *.gcov
	@rm -f gmon.out
	@rm -rf *.dSYM
	@echo "✓ Clean complete"

cleanobj:
	@echo "Cleaning object files..."
	@rm -f $(OBJ) $(DEP)

distclean: clean
	@echo "Deep cleaning..."
	@rm -rf $(BUILDDIR)
	@rm -f compile_commands.json
	@rm -f .*.swp *~
	@echo "✓ Distribution clean complete"

# =============================================================================
# Utility Targets
# =============================================================================

rebuild: clean all

info:
	@echo "════════════════════════════════════════════════════════"
	@echo "  Project Information"
	@echo "════════════════════════════════════════════════════════"
	@echo "Project: $(PROJECT_NAME) v$(VERSION)"
	@echo "Author: $(AUTHOR)"
	@echo ""
	@echo "Build Configuration:"
	@echo "  CC:        $(CC)"
	@echo "  CFLAGS:    $(CFLAGS)"
	@echo "  LDFLAGS:   $(LDFLAGS)"
	@echo "  LIBS:      $(LIBS)"
	@echo ""
	@echo "Platform:"
	@echo "  OS:        $(UNAME_S)"
	@echo "  Arch:      $(UNAME_M)"
	@echo ""
	@echo "Directories:"
	@echo "  Prefix:    $(PREFIX)"
	@echo "  Binary:    $(BINDIR)"
	@echo ""
	@echo "Source Files:"
	@for src in $(SRC); do echo "  - $$src"; done
	@echo "════════════════════════════════════════════════════════"

help:
	@echo "════════════════════════════════════════════════════════"
	@echo "  $(PROJECT_NAME) Makefile - Available Targets"
	@echo "════════════════════════════════════════════════════════"
	@echo ""
	@echo "Build Targets:"
	@echo "  all            - Build the project (default)"
	@echo "  debug          - Build with debug symbols and no optimization"
	@echo "  release        - Build optimized release version"
	@echo "  profile        - Build with profiling support (gprof)"
	@echo "  asan           - Build with AddressSanitizer"
	@echo "  ubsan          - Build with UndefinedBehaviorSanitizer"
	@echo "  msan           - Build with MemorySanitizer"
	@echo "  tsan           - Build with ThreadSanitizer"
	@echo "  coverage       - Build with coverage instrumentation"
	@echo ""
	@echo "Installation:"
	@echo "  install            - Install binary to $(BINDIR)"
	@echo "  uninstall          - Remove installed binary"
	@echo "  install-deps       - Install build dependencies (GMP, ncurses, compiler)"
	@echo "  install-debug-deps - Install all dependencies including dev tools"
	@echo "  check-deps         - Check if dependencies are installed"
	@echo ""
	@echo "Testing:"
	@echo "  test           - Run basic tests"
	@echo "  test-full      - Run comprehensive test suite"
	@echo "  test-valgrind  - Run tests with Valgrind"
	@echo "  test-sanitizers- Run tests with all sanitizers"
	@echo ""
	@echo "Code Quality:"
	@echo "  lint           - Run all linters"
	@echo "  lint-c         - Run C code linter (clang-tidy)"
	@echo "  lint-shell     - Run shell script linter"
	@echo "  format         - Format source code with clang-format"
	@echo "  check-format   - Check if code is properly formatted"
	@echo "  analyze        - Run static analysis"
	@echo "  cppcheck       - Run cppcheck static analyzer"
	@echo ""
	@echo "Cleanup:"
	@echo "  clean          - Remove build artifacts"
	@echo "  cleanobj       - Remove object files only"
	@echo "  distclean      - Deep clean (remove all generated files)"
	@echo "  rebuild        - Clean and build from scratch"
	@echo ""
	@echo "Information:"
	@echo "  info           - Display project configuration"
	@echo "  help           - Display this help message"
	@echo ""
	@echo "Examples:"
	@echo "  make                    # Build normally"
	@echo "  make debug              # Build with debug info"
	@echo "  make test-full          # Run all tests"
	@echo "  make PREFIX=/opt install # Install to /opt/bin"
	@echo "  make OPT_LEVEL=-O3      # Build with -O3 optimization"
	@echo "════════════════════════════════════════════════════════"

# =============================================================================
# Phony Targets
# =============================================================================

.PHONY: all banner clean cleanobj distclean rebuild \
        debug release profile asan ubsan msan tsan coverage \
        test test-full test-valgrind test-sanitizers asan-test ubsan-test \
        lint lint-c lint-shell format check-format analyze cppcheck \
        install uninstall install-deps install-debug-deps check-deps \
        info help