[![GitHub Actions tests](https://github.com/rodrigobelloso/fib/actions/workflows/build.yml/badge.svg)](https://github.com/rodrigobelloso/fib/actions/workflows/build.yml) [![GitHub license](https://img.shields.io/badge/license-MIT-blue.svg)](./LICENSE)

# fib

High-precision Fibonacci number calculator implemented in C using the GMP (GNU Multiple Precision Arithmetic) library.

## Description

This program calculates Fibonacci numbers up to a specified limit using arbitrary-precision arithmetic, allowing the calculation of extremely large numbers without loss of precision. It implements three different algorithms for calculation: iterative, recursive with memoization, and matrix exponentiation.

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
./fib <number> -a matrix -t -r -v -o result.txt
```

## Command-line Options

- `<number>`: The sequence position of the Fibonacci number to calculate
- `-h, --help`: Display help message and exit
- `-a, --algorithm <method>`: Set calculation algorithm. Available options:
  - `iter`: Iterative method (default, most efficient for general use)
  - `recur`: Recursive method with memoization (demonstrates recursion)
  - `matrix`: Matrix exponentiation method (efficient for very large numbers)
- `-t, --time`: Show calculation time
- `-r, --raw`: Output only the number without "Fibonacci Number X:" prefix
- `-v, --verbose`: Show detailed information during calculation
- `-o, --output <filename>`: Save the result to the specified file

## Examples

1. Calculate the 50th Fibonacci number:

   ```
   ./fib 50
   Fibonacci Number 50: 12586269025
   ```

2. Calculate using matrix exponentiation and show time taken:

   ```
   ./fib 100 -a matrix -t
   Fibonacci Number 100: 354224848179261915075
   Calculation Time: 0.000098 seconds
   ```

3. Compare performance between algorithms:

   ```
   ./fib 30 -a iter -t
   ./fib 30 -a recur -t
   ./fib 30 -a matrix -t
   ```

4. Generate raw output and save to file:

   ```
   ./fib 75 -r -o result.txt
   ```

   Contents of result.txt:

   ```
   2111485077978050
   ```

5. Calculate with verbose output:

   ```
   ./fib 100 -v -a matrix
   Initializing Fibonacci calculation for n=100
   Using matrix exponentiation algorithm
   Computing matrix power 50...
   Calculation complete
   Preparing to write result
   Converting result to string
   Result has 21 digits
   Cleaning up memory
   Program completed successfully
   Fibonacci Number 100: 354224848179261915075
   ```

6. Combining multiple options:
   ```
   ./fib 1000 -a matrix -t -v -o big_fib.txt
   ```
   This will calculate the 1000th Fibonacci number using matrix exponentiation,
   with verbose output to stderr, and save the result and timing information to big_fib.txt.

## Algorithm Performance

- **Iterative**: O(n) time complexity, O(1) space complexity. Best for general use.
- **Recursive with Memoization**: O(n) time complexity, O(n) space complexity. Demonstrates dynamic programming.
- **Matrix Exponentiation**: O(log n) time complexity, O(1) space complexity. Most efficient for very large values of n.

## License

fib is under the [GNU GENERAL PUBLIC LICENSE 3](./LICENSE).
