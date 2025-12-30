#include "fib.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void display_help(const char *program_name) {
  printf("\nUsage: %s <limit> [options]\n", program_name);
  printf("\n");
  printf("Calculate Fibonacci numbers.\n");
  printf("\n");
  printf("Options:\n");
  printf("  -h, --help    Display this help message and exit.\n");
  printf("  -t, --time    Show calculation time.\n");
  printf("  -T, --time-only\n");
  printf("                Show only calculation time (no result output).\n");
  printf("                Useful for stress testing and benchmarking.\n");
  printf("  -r, --raw     Output only the number without prefix.\n");
  printf("  -v, --verbose Show detailed information during calculation.\n");
  printf("  -o, --output  Save the result to the specified file.\n");
  printf("  -f, --format <format>\n");
  printf("                Set output number format. Available options:\n");
  printf("                  dec   - Decimal (default)\n");
  printf("                  hex   - Hexadecimal\n");
  printf("                  bin   - Binary\n");
  printf("  -a, --algorithm <method>\n");
  printf("                Set calculation algorithm. Available options:\n");
  printf("                  iter   - Iterative\n");
  printf("                  recur  - Recursive with memoization\n");
  printf("                  matrix - Matrix exponentiation (default)\n");
  printf("\n");
  printf("Examples:\n");
  printf("  %s 100                 Calculate using default algorithm\n", program_name);
  printf("  %s 50 -a matrix        Calculate using matrix exponentiation\n", program_name);
  printf("  %s 30 -f hex           Display result in hexadecimal\n", program_name);
  printf("  %s 20 -f bin -r        Display raw binary result\n", program_name);
  printf("  %s 30 -a recur -t      Calculate recursively and show time\n", program_name);
  printf("  %s 1000000 -T          Stress test - show only time\n", program_name);
  printf("\n");
}

char *get_formatted_result(mpz_t result, OutputFormat format, int verbose) {
  char *result_str = NULL;

  switch (format) {
    case DECIMAL:
      if (verbose) {
        fprintf(stderr, "Converting result to decimal format\n");
      }
      result_str = mpz_get_str(NULL, 10, result);
      break;

    case HEXADECIMAL:
      if (verbose) {
        fprintf(stderr, "Converting result to hexadecimal format\n");
      }
      result_str = mpz_get_str(NULL, 16, result);
      break;

    case BINARY:
      if (verbose) {
        fprintf(stderr, "Converting result to binary format\n");
      }
      result_str = mpz_get_str(NULL, 2, result);
      break;

    default:
      if (verbose) {
        fprintf(stderr, "Unknown format, defaulting to decimal\n");
      }
      result_str = mpz_get_str(NULL, 10, result);
  }

  return result_str;
}

const char *get_format_prefix(OutputFormat format) {
  switch (format) {
    case HEXADECIMAL:
      return "0x";
    case BINARY:
      return "0b";
    default:
      return "";
  }
}
