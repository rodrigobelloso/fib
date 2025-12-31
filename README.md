[![GitHub Actions build and test](https://github.com/rodrigobelloso/fib/actions/workflows/build.yml/badge.svg)](https://github.com/rodrigobelloso/fib/actions/workflows/build.yml) [![GitHub Actions CodeQL](https://github.com/rodrigobelloso/fib/actions/workflows/codeql.yml/badge.svg)](https://github.com/rodrigobelloso/fib/actions/workflows/codeql.yml) [![GitHub License](https://img.shields.io/badge/License-GPL_3.0-yellow.svg)](./LICENSE)

# fib

A high precision Fibonacci number calculator implemented in C using the GMP (GNU Multiple Precision Arithmetic) library.

## Description:

fib calculates Fibonacci numbers up to a specified limit using arbitrary-precision arithmetic, allowing the calculation of extremely large numbers without loss of precision. It implements three different algorithms for calculation and supports multiple output formats.

## Requirements:

- C compiler supporting C99 or higher
- GMP library (GNU Multiple Precision Arithmetic)
- ncurses library (for Terminal User Interface)
- Make (for easier compilation)

## Installation of Dependencies:

### Automatically:

fib includes a Makefile to install required dependencies:

```sh
# Install only build dependencies (GMP, ncurses libraries, compiler tools)
make install-deps

# Install all dependencies including development tools
# (clang-format, cppcheck, valgrind, shellcheck)
make install-debug-deps

# Check which dependencies are installed
make check-deps
```

**Note:** The Makefile automatically detects your package manager (Homebrew, apt-get, dnf, or pacman) and installs the appropriate packages.

### Manually:

Install dependencies manually by executing:

```sh
# On Debian/Ubuntu based systems
sudo apt install libgmp-dev libncurses-dev

# On Fedora/RHEL based systems
sudo dnf install gmp-devel ncurses-devel

# On Arch Linux
sudo pacman -S gmp ncurses

# On macOS with Homebrew
brew install gmp ncurses
```

## Building from source:

### Automatically:

fib uses a comprehensive `Makefile` for building and managing the project. The Makefile automatically detects your platform (macOS/Linux) and adjusts paths for GMP accordingly.

#### Build ID System:

Each build automatically generates a unique 6-character random build ID. This identifier helps track different builds and is displayed when using the `--version` option:

```sh
./fib --version
# Output: ./fib version 0.3.0 (build 4a5ueg)
```

The build ID is:

- Automatically generated during compilation
- A random 6-character alphanumeric string (lowercase letters and numbers)
- Unique for each build
- Not stored in version control (build_id.h is in .gitignore)

#### Basic Build Commands:

```sh
# Build the project (default target)
make
# or
make all

# Get help about all available commands
make help

# Display project information and configuration
make info

# Rebuild the project (clean and then build)
make rebuild
```

#### Build Variants:

The Makefile supports multiple build configurations:

```sh
# Build with debug symbols and no optimization
make debug

# Build optimized release version (-O3, strip symbols)
make release

# Build with profiling support (for gprof)
make profile

# Build with AddressSanitizer (detect memory errors)
make asan

# Build with UndefinedBehaviorSanitizer
make ubsan

# Build with MemorySanitizer
make msan

# Build with ThreadSanitizer
make tsan

# Build with coverage instrumentation
make coverage
```

#### Testing:

```sh
# Run basic tests
make test

# Run comprehensive test suite
make test-full

# Run tests with Valgrind (memory leak detection)
# Note: Requires valgrind to be installed
make test-valgrind

# Run tests with all sanitizers
make test-sanitizers
```

**Test Dependencies:**

- `test` and `test-full` require the `fib` executable to be built
- `test-valgrind` requires valgrind to be installed
- Test scripts automatically check for required dependencies and provide clear error messages if missing

#### Code Quality:

```sh
# Run all linters (C code + shell scripts)
make lint

# Run C code linter (clang-format check)
# Note: Skips gracefully if clang-format is not installed
make lint-c

# Run shell script linter
# Note: Requires shellcheck and clang-format
make lint-shell

# Format source code with clang-format
# Note: Requires clang-format to be installed
make format

# Check if code is properly formatted
# Note: Requires clang-format to be installed
make check-format

# Run static analysis with clang
make analyze

# Run cppcheck static analyzer
# Note: Requires cppcheck to be installed
make cppcheck
```

**Code Quality Dependencies:**

- All code quality targets check for required tools before running
- Missing tools result in clear error messages with guidance
- `lint-c` will skip checks gracefully if clang-format is missing
- Other targets will exit with an error if their required tool is missing

#### Dependency Management:

```sh
# Check if dependencies are installed
make check-deps

# Install only build dependencies (GMP library, compiler tools)
make install-deps

# Install all dependencies including development tools
# (clang-format, cppcheck, valgrind, shellcheck)
make install-debug-deps
```

**Dependency Checking:**

- All Makefile targets that require specific tools now check for their availability
- If a required tool is missing, a clear error message is displayed
- Test scripts (`tests/build.sh` and `tests/lint.sh`) validate dependencies before running
- No more confusing "command not found" errors

**What gets installed:**

- `install-deps`: GMP library, build-essential/base-devel
- `install-debug-deps`: All of the above plus clang-format, cppcheck, valgrind, shellcheck

#### Installation:

```sh
# Install binary to /usr/local/bin (default)
make install

# Install to a custom prefix
make PREFIX=/opt install

# Uninstall the binary
make uninstall
```

#### Cleanup:

```sh
# Remove build artifacts (objects, executables, coverage files)
make clean

# Remove only object files
make cleanobj

# Deep clean (remove all generated files)
make distclean
```

#### Advanced Usage:

You can customize the build by setting variables:

```sh
# Build with custom optimization level
make OPT_LEVEL=-O3

# Build with custom compiler
make CC=clang

# Build with custom prefix
make PREFIX=/opt
```

_The Makefile automatically detects your platform and adjusts compilation flags accordingly. On macOS, it uses Homebrew paths for GMP; on Linux, it uses system paths._

### Manually:

Build fib manually by executing `gcc` or `clang` in the following manner:

```sh
# Debian/Ubuntu based distros
gcc -o fib fib.c algorithms.c matrix.c utils.c -lgmp

# macOS systems
gcc fib.c algorithms.c matrix.c utils.c -o fib -I/opt/homebrew/include -L/opt/homebrew/lib -lgmp
```

## Usage:

```sh
# Display help information
./fib -h
# or
./fib --help

# Display calculation history
./fib -y
# or
./fib --history

# Calculate Fibonacci number using default algorithm (iterative)
./fib <number>

# Choose a specific algorithm for calculation
./fib <number> -a iter     # Iterative (default)
./fib <number> -a recur    # Recursive with memoization
./fib <number> -a matrix   # Matrix exponentiation
# or
./fib <number> --algorithm iter/recur/matrix

# Choose output format
./fib <number> -f dec      # Decimal (default)
./fib <number> -f hex      # Hexadecimal (with 0x prefix)
./fib <number> -f bin      # Binary (with 0b prefix)
# or
./fib <number> --format dec/hex/bin

# Calculate Fibonacci number and show its calculation time
./fib <number> -t
# or
./fib <number> --time

# Show ONLY the calculation time without displaying the result
# Useful for stress testing and benchmarking
./fib <number> -T
# or
./fib <number> --time-only

# Calculate Fibonacci number without the "Fibonacci Number X:" prefix
./fib <number> -r
# or
./fib <number> --raw

# Show detailed progress information during calculation
./fib <number> -v
# or
./fib <number> --verbose

# Save the result to a file
./fib <number> -o filename
# or
./fib <number> --output filename

# Combining options
./fib <number> -a matrix -f hex -t -r -v -o result.txt
```

### Terminal User Interface (TUI)

Run without arguments to launch the interactive TUI:

```sh
./fib
```

TUI Features:

- **Navigation**: Use UP/DOWN arrows, TAB, or k/j keys to navigate fields
- **Edit values**: Press ENTER to edit the selected field
- **Toggle options**: Press SPACE to toggle boolean fields or cycle through options
- **Calculate**: Press F to start the calculation
- **View History**: Press H to view calculation history
- **Quit**: Press Q or ESC to exit

In History View:

- **Navigate**: Use UP/DOWN arrows or k/j keys
- **Delete entry**: Press D to delete the selected history entry
- **Back to Main**: Press H or ESC to return to the main view
- **Quit**: Press Q to exit

The calculation history is automatically saved to `~/.fib_history` and stores up to 100 entries.

## Examples:

Calculate the 50th Fibonacci number:

```sh
./fib 50
```

Calculate using matrix exponentiation and show time taken:

```sh
./fib 100 -a matrix -t
```

View calculation history:

```sh
./fib -y
```

Stress test: Calculate large Fibonacci numbers showing only execution time:

```sh
./fib 1000000 -T
./fib 500000 -T -a matrix
```

Display result in hexadecimal format:

```sh
./fib 20 -f hex
```

Display raw binary output:

```sh
./fib 10 -f bin -r
```

Compare performance between algorithms:

```sh
./fib 30 -a iter -t
./fib 30 -a recur -t
./fib 30 -a matrix -t
```

Benchmark different algorithms with time-only mode:

```sh
echo "=== Performance Comparison ==="
echo -n "Iterative: " && ./fib 100000 -T -a iter
echo -n "Matrix:    " && ./fib 100000 -T -a matrix
```

Generate raw output and save to file:

```sh
./fib 75 -r -o result.txt
```

Calculate with verbose output:

```sh
./fib 100 -v -a matrix
```

Combining multiple options:

```sh
./fib 1000 -a matrix -f hex -t -r -v -o res.txt
```

## Output formats:

fib supports three output formats:

- Decimal: The standard base-10 representation. (default)
- Hexadecimal: Base-16 representation with 0x prefix.
- Binary: Base-2 representation with 0b prefix.

_When using the -r/--raw flag with a non-decimal format, the appropriate prefix is still included._

## Algorithm performance:

- Iterative: O(n) time complexity, O(1) space complexity. Best for general use.
- Recursive with Memoization: O(n) time complexity, O(n) space complexity. Demonstrates dynamic programming.
- Matrix Exponentiation: O(log n) time complexity, O(1) space complexity. Most efficient for very large values of n.

## License:

fib is licensed under the [GPL-3.0](./LICENSE).
