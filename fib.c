#if __STDC_VERSION__ < 199901L
#error "C99 Minimum Required"
#endif

#include <errno.h>
#include <gmp.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

typedef enum { ITERATIVE, RECURSIVE, MATRIX } Algorithm;

void display_help(const char *program_name) {
  printf("\nUsage: %s <limit> [options]\n", program_name);
  printf("\n");
  printf("Calculate Fibonacci numbers.\n");
  printf("\n");
  printf("Options:\n");
  printf("  -h, --help    Display this help message and exit.\n");
  printf("  -t, --time    Show calculation time.\n");
  printf("  -r, --raw     Output only the number without prefix.\n");
  printf("  -v, --verbose Show detailed information during calculation.\n");
  printf("  -o, --output  Save the result to the specified file.\n");
  printf("  -a, --algorithm <method>\n");
  printf("                Set calculation algorithm. Available options:\n");
  printf("                  iter   - Iterative (default, most efficient)\n");
  printf("                  recur  - Recursive with memoization\n");
  printf("                  matrix - Matrix exponentiation\n");
  printf("\n");
  printf("Examples:\n");
  printf("  %s 100                 Calculate using default algorithm\n", program_name);
  printf("  %s 50 -a matrix        Calculate using matrix exponentiation\n", program_name);
  printf("  %s 30 -a recur -t      Calculate recursively and show time\n", program_name);
  printf("\n");
}

void calculate_fibonacci_iterative(mpz_t result, long n, int verbose) {
  if (n == 0) {
    mpz_set_ui(result, 0);
    return;
  } else if (n == 1) {
    mpz_set_ui(result, 1);
    return;
  }

  mpz_t a, b, c;
  mpz_init_set_ui(a, 1);
  mpz_init_set_ui(b, 0);
  mpz_init(c);

  long i = 0;
  while (i < n) {
    if (verbose && (i == 0 || (i > 0 && (i % 100 == 0 || i == n - 1)))) {
      fprintf(stderr, "Calculating F(%ld)...\n", i);
    }

    mpz_add(c, a, b);
    mpz_set(a, b);
    mpz_set(b, c);
    ++i;
  }

  mpz_set(result, b);

  mpz_clear(a);
  mpz_clear(b);
  mpz_clear(c);
}

void matrix_multiply(mpz_t a11, mpz_t a12, mpz_t a21, mpz_t a22, mpz_t b11, mpz_t b12, mpz_t b21,
                     mpz_t b22, mpz_t c11, mpz_t c12, mpz_t c21, mpz_t c22) {
  mpz_t temp1, temp2;
  mpz_init(temp1);
  mpz_init(temp2);

  mpz_mul(temp1, a11, b11);
  mpz_mul(temp2, a12, b21);
  mpz_add(c11, temp1, temp2);

  mpz_mul(temp1, a11, b12);
  mpz_mul(temp2, a12, b22);
  mpz_add(c12, temp1, temp2);

  mpz_mul(temp1, a21, b11);
  mpz_mul(temp2, a22, b21);
  mpz_add(c21, temp1, temp2);

  mpz_mul(temp1, a21, b12);
  mpz_mul(temp2, a22, b22);
  mpz_add(c22, temp1, temp2);

  mpz_clear(temp1);
  mpz_clear(temp2);
}

void matrix_power(mpz_t a11, mpz_t a12, mpz_t a21, mpz_t a22, long n, mpz_t result11,
                  mpz_t result12, mpz_t result21, mpz_t result22, int verbose) {
  if (n == 0) {
    mpz_set_ui(result11, 1);
    mpz_set_ui(result12, 0);
    mpz_set_ui(result21, 0);
    mpz_set_ui(result22, 1);
    return;
  }

  if (n == 1) {
    mpz_set(result11, a11);
    mpz_set(result12, a12);
    mpz_set(result21, a21);
    mpz_set(result22, a22);
    return;
  }

  if (verbose && (n % 50 == 0)) {
    fprintf(stderr, "Computing matrix power %ld...\n", n);
  }

  mpz_t temp11, temp12, temp21, temp22;
  mpz_init(temp11);
  mpz_init(temp12);
  mpz_init(temp21);
  mpz_init(temp22);

  if (n % 2 == 0) {
    matrix_power(a11, a12, a21, a22, n / 2, temp11, temp12, temp21, temp22, verbose);
    matrix_multiply(temp11, temp12, temp21, temp22, temp11, temp12, temp21, temp22, result11,
                    result12, result21, result22);
  } else {
    matrix_power(a11, a12, a21, a22, n - 1, temp11, temp12, temp21, temp22, verbose);
    matrix_multiply(a11, a12, a21, a22, temp11, temp12, temp21, temp22, result11, result12,
                    result21, result22);
  }

  mpz_clear(temp11);
  mpz_clear(temp12);
  mpz_clear(temp21);
  mpz_clear(temp22);
}

void calculate_fibonacci_matrix(mpz_t result, long n, int verbose) {
  if (n == 0) {
    mpz_set_ui(result, 0);
    return;
  } else if (n == 1) {
    mpz_set_ui(result, 1);
    return;
  }

  if (verbose) {
    fprintf(stderr, "Using matrix exponentiation algorithm\n");
  }

  mpz_t a11, a12, a21, a22;
  mpz_init_set_ui(a11, 1);
  mpz_init_set_ui(a12, 1);
  mpz_init_set_ui(a21, 1);
  mpz_init_set_ui(a22, 0);

  mpz_t result11, result12, result21, result22;
  mpz_init(result11);
  mpz_init(result12);
  mpz_init(result21);
  mpz_init(result22);

  matrix_power(a11, a12, a21, a22, n - 1, result11, result12, result21, result22, verbose);

  mpz_set(result, result11);

  mpz_clear(a11);
  mpz_clear(a12);
  mpz_clear(a21);
  mpz_clear(a22);
  mpz_clear(result11);
  mpz_clear(result12);
  mpz_clear(result21);
  mpz_clear(result22);
}

void calculate_fibonacci_recursive(mpz_t result, long n, mpz_t *memo, int verbose) {
  if (mpz_sgn(memo[n]) != 0) {
    mpz_set(result, memo[n]);
    return;
  }

  if (n == 0) {
    mpz_set_ui(result, 0);
    mpz_set_ui(memo[0], 0);
    return;
  } else if (n == 1) {
    mpz_set_ui(result, 1);
    mpz_set_ui(memo[1], 1);
    return;
  }

  if (verbose && (n % 100 == 0 || n <= 10)) {
    fprintf(stderr, "Computing F(%ld) recursively...\n", n);
  }

  mpz_t temp1, temp2;
  mpz_init(temp1);
  mpz_init(temp2);

  calculate_fibonacci_recursive(temp1, n - 1, memo, verbose);
  calculate_fibonacci_recursive(temp2, n - 2, memo, verbose);

  mpz_add(result, temp1, temp2);
  mpz_set(memo[n], result);

  mpz_clear(temp1);
  mpz_clear(temp2);
}

int main(int argc, const char *const argv[argc + 1]) {
  if (argc < 2) {
    fprintf(stderr, "Usage: %s <limit> [-h] [-t] [-r] [-v] [-a algo] [-o filename]\n", argv[0]);
    fprintf(stderr, "Try '%s --help' for more information.\n", argv[0]);
    return EXIT_FAILURE;
  }

  int show_time = 0;
  int raw_output = 0;
  int verbose = 0;
  long limit = -1;
  char const *output_file = NULL;
  Algorithm algo = ITERATIVE;

  for (int i = 1; i < argc; i++) {
    if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0) {
      display_help(argv[0]);
      return EXIT_SUCCESS;
    } else if (strcmp(argv[i], "-t") == 0 || strcmp(argv[i], "--time") == 0) {
      show_time = 1;
    } else if (strcmp(argv[i], "-r") == 0 || strcmp(argv[i], "--raw") == 0) {
      raw_output = 1;
    } else if (strcmp(argv[i], "-v") == 0 || strcmp(argv[i], "--verbose") == 0) {
      verbose = 1;
    } else if (strcmp(argv[i], "-a") == 0 || strcmp(argv[i], "--algorithm") == 0) {
      if (i + 1 < argc) {
        i++;
        if (strcmp(argv[i], "iter") == 0) {
          algo = ITERATIVE;
        } else if (strcmp(argv[i], "recur") == 0) {
          algo = RECURSIVE;
        } else if (strcmp(argv[i], "matrix") == 0) {
          algo = MATRIX;
        } else {
          fprintf(stderr, "Error: Unknown algorithm '%s'\n", argv[i]);
          fprintf(stderr, "Valid options: iter, recur, matrix\n");
          return EXIT_FAILURE;
        }
      } else {
        fprintf(stderr, "Error: Missing algorithm for -a/--algorithm option\n");
        return EXIT_FAILURE;
      }
    } else if (strcmp(argv[i], "-o") == 0 || strcmp(argv[i], "--output") == 0) {
      if (i + 1 < argc) {
        output_file = argv[++i];
      } else {
        fprintf(stderr, "Error: Missing filename for -o/--output option\n");
        return EXIT_FAILURE;
      }
    } else if (limit == -1) {
      if (argv[i][0] == '-' && argv[i][1] != '\0') {
        fprintf(stderr, "Unknown option: %s\n", argv[i]);
        fprintf(stderr, "Try '%s --help' for more information.\n", argv[0]);
        return EXIT_FAILURE;
      }

      char *end;
      errno = 0;
      limit = strtol(argv[i], &end, 10);
      if ((argv[i] == end) || *end) {
        fprintf(stderr, "Error Parsing %s\n", argv[i]);
        return EXIT_FAILURE;
      } else if (errno == ERANGE) {
        perror(argv[i]);
        return EXIT_FAILURE;
      }
    } else {
      fprintf(stderr, "Usage: %s <limit> [-h] [-t] [-r] [-v] [-a algo] [-o filename]\n", argv[0]);
      fprintf(stderr, "Try '%s --help' for more information.\n", argv[0]);
      return EXIT_FAILURE;
    }
  }

  if (limit == -1) {
    fprintf(stderr, "Error: Missing limit value\n");
    fprintf(stderr, "Try '%s --help' for more information.\n", argv[0]);
    return EXIT_FAILURE;
  }

  if (verbose) {
    fprintf(stderr, "Initializing Fibonacci calculation for n=%ld\n", limit);
    if (raw_output) {
      fprintf(stderr, "Raw output mode enabled\n");
    }
    if (output_file != NULL) {
      fprintf(stderr, "Output will be saved to: %s\n", output_file);
    }

    switch (algo) {
      case ITERATIVE:
        fprintf(stderr, "Using iterative algorithm\n");
        break;
      case RECURSIVE:
        fprintf(stderr, "Using recursive algorithm with memoization\n");
        break;
      case MATRIX:
        fprintf(stderr, "Using matrix exponentiation algorithm\n");
        break;
    }
  }

  mpz_t result;
  mpz_init(result);

  clock_t start_time = (clock_t) 0;
  if (show_time) {
    start_time = clock();
    if (start_time == (clock_t) -1) {
      fprintf(stderr, "Error start_time clock()\n");
      return EXIT_FAILURE;
    }
    if (verbose) {
      fprintf(stderr, "Started timing calculation\n");
    }
  }

  switch (algo) {
    case ITERATIVE:
      calculate_fibonacci_iterative(result, limit, verbose);
      break;
    case RECURSIVE:
      if (limit > 1000000) {
        fprintf(stderr, "Warning: Recursive method may be inefficient for n > 1000000\n");
      }
      {
        mpz_t *memo = (mpz_t *) malloc((limit + 1) * sizeof(mpz_t));
        if (memo == NULL) {
          fprintf(stderr, "Error: Memory allocation failed\n");
          return EXIT_FAILURE;
        }

        for (long j = 0; j <= limit; j++) {
          mpz_init(memo[j]);
        }

        calculate_fibonacci_recursive(result, limit, memo, verbose);

        for (long j = 0; j <= limit; j++) {
          mpz_clear(memo[j]);
        }
        free(memo);
      }
      break;
    case MATRIX:
      calculate_fibonacci_matrix(result, limit, verbose);
      break;
  }

  if (verbose) {
    fprintf(stderr, "Calculation complete\n");
  }

  clock_t end_time = (clock_t) 0;
  if (show_time) {
    end_time = clock();
    if (end_time == (clock_t) -1) {
      fprintf(stderr, "Error end_time clock()\n");
      mpz_clear(result);
      return EXIT_FAILURE;
    }
    if (verbose) {
      fprintf(stderr, "Finished timing calculation\n");
    }
  }

  FILE *output = stdout;
  if (output_file != NULL) {
    if (verbose) {
      fprintf(stderr, "Opening output file: %s\n", output_file);
    }

    output = fopen(output_file, "w");
    if (output == NULL) {
      perror("Error opening output file");
      mpz_clear(result);
      return EXIT_FAILURE;
    }
  }

  if (verbose) {
    fprintf(stderr, "Preparing to write result\n");
  }

  if (!raw_output) {
    if (fprintf(output, "Fibonacci Number %ld: ", limit) < 0) {
      if (output != stdout) {
        fclose(output);
      }
      mpz_clear(result);
      return EXIT_FAILURE;
    }
  }

  if (verbose) {
    fprintf(stderr, "Converting result to string\n");
  }

  char *result_str = mpz_get_str(NULL, 10, result);
  if (result_str == NULL) {
    if (output != stdout) {
      fclose(output);
    }
    mpz_clear(result);
    return EXIT_FAILURE;
  }

  if (verbose) {
    size_t result_len = strlen(result_str);
    fprintf(stderr, "Result has %zu digits\n", result_len);
  }

  if (fprintf(output, "%s\n", result_str) < 0) {
    free(result_str);
    if (output != stdout) {
      fclose(output);
    }
    mpz_clear(result);
    return EXIT_FAILURE;
  }

  free(result_str);

  if (show_time) {
    const double time_taken = ((double) (end_time - start_time)) / (double) CLOCKS_PER_SEC;
    if (fprintf(output, "Calculation Time: %lf seconds\n", time_taken) < 0) {
      if (output != stdout) {
        fclose(output);
      }
      mpz_clear(result);
      return EXIT_FAILURE;
    }

    if (verbose) {
      fprintf(stderr, "Time taken for calculation: %lf seconds\n", time_taken);
    }
  }

  if (output != stdout) {
    if (verbose) {
      fprintf(stderr, "Closing output file\n");
    }

    if (fclose(output) != 0) {
      perror("Error closing output file");
      mpz_clear(result);
      return EXIT_FAILURE;
    }
  } else {
    if (fflush(stdout) == EOF) {
      mpz_clear(result);
      return EXIT_FAILURE;
    }
  }

  if (verbose) {
    fprintf(stderr, "Cleaning up memory\n");
  }

  mpz_clear(result);

  if (verbose) {
    fprintf(stderr, "Program completed successfully\n");
  }

  return EXIT_SUCCESS;
}
