[![GitHub Actions tests](https://github.com/rodrigobelloso/fib/actions/workflows/build.yml/badge.svg)](https://github.com/rodrigobelloso/fib/actions/workflows/build.yml) [![GitHub license](https://img.shields.io/badge/license-MIT-blue.svg)](./LICENSE)

# fib

High-precision Fibonacci number calculator implemented in C using the GMP (GNU Multiple Precision Arithmetic) library.

## Description

This program calculates Fibonacci numbers up to a specified limit using arbitrary-precision arithmetic, allowing the calculation of extremely large numbers without loss of precision. It implements three different algorithms for calculation and supports multiple output formats.

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

## Command-line Options

- `<number>`: The sequence position of the Fibonacci number to calculate
- `-h, --help`: Display help message and exit
- `-a, --algorithm <method>`: Set calculation algorithm. Available options:
  - `iter`: Iterative method (default, most efficient for general use)
  - `recur`: Recursive method with memoization (demonstrates recursion)
  - `matrix`: Matrix exponentiation method (efficient for very large numbers)
- `-f, --format <format>`: Set output number format. Available options:
  - `dec`: Decimal (default)
  - `hex`: Hexadecimal (with 0x prefix)
  - `bin`: Binary (with 0b prefix)
- `-t, --time`: Show calculation time
- `-r, --raw`: Output only the number without "Fibonacci Number X:" prefix
- `-v, --verbose`: Show detailed information during calculation
- `-o, --output <filename>`: Save the result to the specified file

## Examples

1. Calculate the 50th Fibonacci number:

   ```
   ./fib 50
   Fibonacci Number 50 (decimal): 12586269025
   ```

2. Calculate using matrix exponentiation and show time taken:

   ```
   ./fib 100 -a matrix -t
   Fibonacci Number 100 (decimal): 354224848179261915075
   Calculation Time: 0.000098 seconds
   ```

3. Display result in hexadecimal format:

   ```
   ./fib 20 -f hex
   Fibonacci Number 20 (hexadecimal): 0x1a6d
   ```

4. Display raw binary output:

   ```
   ./fib 10 -f bin -r
   0b110111
   ```

5. Compare performance between algorithms:

   ```
   ./fib 30 -a iter -t
   ./fib 30 -a recur -t
   ./fib 30 -a matrix -t
   ```

6. Generate raw output and save to file:

   ```
   ./fib 75 -r -o result.txt
   ```

   Contents of result.txt:

   ```
   2111485077978050
   ```

7. Calculate with verbose output:

   ```
   ./fib 100 -v -a matrix
   Initializing Fibonacci calculation for n=100
   Using matrix exponentiation algorithm
   Output format: Decimal
   Computing matrix power 50...
   Calculation complete
   Preparing to write result
   Converting result to decimal format
   Result has 21 digits in decimal format
   Cleaning up memory
   Program completed successfully
   Fibonacci Number 100 (decimal): 354224848179261915075
   ```

8. Combining multiple options:
   ```
   ./fib 1000 -a matrix -f hex -t -v -o big_fib.txt
   ```
   This will calculate the 1000th Fibonacci number using matrix exponentiation,
   output in hexadecimal format, with verbose output to stderr, and save the result
   and timing information to big_fib.txt.

## Output Formats

The program supports three output formats:

- **Decimal**: The standard base-10 representation (default)
- **Hexadecimal**: Base-16 representation with `0x` prefix
- **Binary**: Base-2 representation with `0b` prefix

When using the `-r/--raw` flag with a non-decimal format, the appropriate prefix is still included.

## Algorithm Performance

- **Iterative**: O(n) time complexity, O(1) space complexity. Best for general use.
- **Recursive with Memoization**: O(n) time complexity, O(n) space complexity. Demonstrates dynamic programming.
- **Matrix Exponentiation**: O(log n) time complexity, O(1) space complexity. Most efficient for very large values of n.

## License

fib is under the [GNU GENERAL PUBLIC LICENSE 3](./LICENSE).
