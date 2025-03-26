[![GitHub Actions tests](https://github.com/rodrigobelloso/fib/actions/workflows/build.yml/badge.svg)](https://github.com/rodrigobelloso/fib/actions/workflows/build.yml) [![GitHub license](https://img.shields.io/badge/license-MIT-blue.svg)](./LICENSE)

# fib

A high precision Fibonacci number calculator implemented in C using the GMP (GNU Multiple Precision Arithmetic) library.

## Description:

fib calculates Fibonacci numbers up to a specified limit using arbitrary-precision arithmetic, allowing the calculation of extremely large numbers without loss of precision. It implements three different algorithms for calculation and supports multiple output formats.

## Requirements:

- C compiler supporting C99 or higher
- GMP library (GNU Multiple Precision Arithmetic)
- Make (for easier compilation)

## Installation of Dependencies:

### Automatically:

fib includes a make file to install required dependencies:

```sh
make install-deps
```

### Manually:

Install dependencies manually by executing:

```sh
# On Debian/Ubuntu based systems
apt install libgmp-dev

# On Fedora based distros
dnf install gmp

# On macOS with Homebrew
brew install gmp
```

## Building from source:

### Automatically:

fib uses a `makefile` for easier compilation:

```sh
# Build the project
make

# Clean object files and executables
make clean

# Clean only object files
make cleanobj
```

_The `makefile` will automatically detect your platform and will adjust the path for GMP accordingly._

### Manually:

Build fib manually by executing `gcc` or `clang` in the following manner:

```sh
# Debian/Ubuntu based distros
gcc -o fib fib.c algorithms.c matrix.c utils.c -lgmp

# Fedora based distros
gcc -o fib fib.c algorithms.c matrix.c utils.c -gmp

# macOS systems
gcc fib.c algorithms.c matrix.c utils.c -o fib -I/opt/homebrew/include -L/opt/homebrew/lib -lgmp
```

## Usage:

```sh
# Display help information
./fib -h
# or
./fib --help

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

## Examples:

Calculate the 50th Fibonacci number:

```sh
./fib 50
```

Calculate using matrix exponentiation and show time taken:

```sh
./fib 100 -a matrix -t
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
./fib 1000 -a matrix -f hex -t -v -o res.txt
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
