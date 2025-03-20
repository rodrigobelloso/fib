[![GitHub Actions tests](https://github.com/rodrigobelloso/fib/actions/workflows/build.yml/badge.svg)](https://github.com/rodrigobelloso/fib/actions/workflows/build.yml) [![GitHub license](https://img.shields.io/badge/license-MIT-blue.svg)](./LICENSE)

# fib

High-precision Fibonacci number calculator implemented in C using the GMP (GNU Multiple Precision Arithmetic) library.

## Description

This program calculates Fibonacci numbers up to a specified limit using arbitrary-precision arithmetic, allowing the calculation of extremely large numbers without loss of precision.

## Requirements

- C compiler supporting C99 or higher
- GMP library (GNU Multiple Precision Arithmetic)

## Installation

1. Install GMP if not already installed:

```sh
# Debian based distros
apt-get install libgmp-dev

# macOS
brew install gmp
```

## Compiling

```sh
# Debian based distros
gcc -o fib fib.c -lgmp
clang -o fib fib.c -lgmp

# macOS
gcc fib.c -o fib -I/opt/homebrew/include -L/opt/homebrew/lib -lgmp
clang fib.c -o fib -I/opt/homebrew/include -L/opt/homebrew/lib -lgmp
```

## Usage

```sh
# Display help information
./fib -h
# or
./fib --help

# Calculate Fibonacci number
./fib <number>

# Calculate Fibonacci number and show its calculation time
./fib <number> -t
# or
./fib <number> --time

# Calculate Fibonacci number without the "Fibonacci Number X:" prefix
./fib <number> -r
# or
./fib <number> --raw

# Save the result to a file
./fib <number> -o filename
# or
./fib <number> --output filename

# Combining options
./fib <number> -t -r -o result.txt
```

## Command-line Options

- `<number>`: The sequence position of the Fibonacci number to calculate
- `-h, --help`: Display help message and exit
- `-t, --time`: Show calculation time
- `-r, --raw`: Output only the number without "Fibonacci Number X:" prefix
- `-o, --output <filename>`: Save the result to the specified file

## Examples

1. Calculate the 50th Fibonacci number:

   ```
   ./fib 50
   Fibonacci Number 50: 12586269025
   ```

2. Calculate and show time taken:

   ```
   ./fib 100 -t
   Fibonacci Number 100: 354224848179261915075
   Calculation Time: 0.000123 seconds
   ```

3. Generate raw output and save to file:
   ```
   ./fib 75 -r -o result.txt
   ```
   Contents of result.txt:
   ```
   2111485077978050
   ```

## License

fib is under the [GNU GENERAL PUBLIC LICENSE 3](./LICENSE).
