#if __STDC_VERSION__ < 199901L
#error "C99 Minimum Required"
#endif

#include "fib.h"
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

int main(int argc, char *argv[]) {
  int free_args = 0;

  if (argc < 2) {
    run_user_interface(&argc, &argv);
    free_args = 1;
  }

  int show_time = 0;
  int raw_output = 0;
  int verbose = 0;
  long limit = -1;
  char const *output_file = NULL;
  Algorithm algo = ITERATIVE;
  OutputFormat format = DECIMAL;

  for (int i = 1; i < argc; i++) {
    if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0) {
      display_help(argv[0]);
      if (free_args)
        free_generated_args(argc, argv);
      return EXIT_SUCCESS;
    } else if (strcmp(argv[i], "-t") == 0 || strcmp(argv[i], "--time") == 0) {
      show_time = 1;
    } else if (strcmp(argv[i], "-r") == 0 || strcmp(argv[i], "--raw") == 0) {
      raw_output = 1;
    } else if (strcmp(argv[i], "-v") == 0 || strcmp(argv[i], "--verbose") == 0) {
      verbose = 1;
    } else if (strcmp(argv[i], "-f") == 0 || strcmp(argv[i], "--format") == 0) {
      if (i + 1 < argc) {
        i++;
        if (strcmp(argv[i], "dec") == 0) {
          format = DECIMAL;
        } else if (strcmp(argv[i], "hex") == 0) {
          format = HEXADECIMAL;
        } else if (strcmp(argv[i], "bin") == 0) {
          format = BINARY;
        } else {
          fprintf(stderr, "Error: Unknown format '%s'\n", argv[i]);
          fprintf(stderr, "Valid options: dec, hex, bin\n");
          if (free_args)
            free_generated_args(argc, argv);
          return EXIT_FAILURE;
        }
      } else {
        fprintf(stderr, "Error: Missing format for -f/--format option\n");
        if (free_args)
          free_generated_args(argc, argv);
        return EXIT_FAILURE;
      }
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
          if (free_args)
            free_generated_args(argc, argv);
          return EXIT_FAILURE;
        }
      } else {
        fprintf(stderr, "Error: Missing algorithm for -a/--algorithm option\n");
        if (free_args)
          free_generated_args(argc, argv);
        return EXIT_FAILURE;
      }
    } else if (strcmp(argv[i], "-o") == 0 || strcmp(argv[i], "--output") == 0) {
      if (i + 1 < argc) {
        output_file = argv[++i];
      } else {
        fprintf(stderr, "Error: Missing filename for -o/--output option\n");
        if (free_args)
          free_generated_args(argc, argv);
        return EXIT_FAILURE;
      }
    } else if (limit == -1) {
      if (argv[i][0] == '-' && argv[i][1] != '\0') {
        fprintf(stderr, "Unknown option: %s\n", argv[i]);
        fprintf(stderr, "Try '%s --help' for more information.\n", argv[0]);
        if (free_args)
          free_generated_args(argc, argv);
        return EXIT_FAILURE;
      }

      char *end;
      errno = 0;
      limit = strtol(argv[i], &end, 10);
      if ((argv[i] == end) || *end) {
        fprintf(stderr, "Error Parsing %s\n", argv[i]);
        if (free_args)
          free_generated_args(argc, argv);
        return EXIT_FAILURE;
      } else if (errno == ERANGE) {
        perror(argv[i]);
        if (free_args)
          free_generated_args(argc, argv);
        return EXIT_FAILURE;
      }
    } else {
      fprintf(stderr, "Usage: %s <limit> [-h] [-t] [-r] [-v] [-f format] [-a algo] [-o filename]\n",
              argv[0]);
      fprintf(stderr, "Try '%s --help' for more information.\n", argv[0]);
      if (free_args)
        free_generated_args(argc, argv);
      return EXIT_FAILURE;
    }
  }

  if (limit == -1) {
    fprintf(stderr, "Error: Missing limit value\n");
    fprintf(stderr, "Try '%s --help' for more information.\n", argv[0]);
    if (free_args)
      free_generated_args(argc, argv);
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

    switch (format) {
      case DECIMAL:
        fprintf(stderr, "Output format: Decimal\n");
        break;
      case HEXADECIMAL:
        fprintf(stderr, "Output format: Hexadecimal\n");
        break;
      case BINARY:
        fprintf(stderr, "Output format: Binary\n");
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
    const char *format_name;
    switch (format) {
      case DECIMAL:
        format_name = "decimal";
        break;
      case HEXADECIMAL:
        format_name = "hexadecimal";
        break;
      case BINARY:
        format_name = "binary";
        break;
      default:
        format_name = "decimal";
    }

    if (fprintf(output, "Fibonacci Number %ld (%s): %s", limit, format_name,
                format != DECIMAL ? get_format_prefix(format) : "") < 0) {
      if (output != stdout) {
        fclose(output);
      }
      mpz_clear(result);
      return EXIT_FAILURE;
    }
  } else if (format != DECIMAL) {
    if (fprintf(output, "%s", get_format_prefix(format)) < 0) {
      if (output != stdout) {
        fclose(output);
      }
      mpz_clear(result);
      return EXIT_FAILURE;
    }
  }

  char *result_str = get_formatted_result(result, format, verbose);
  if (result_str == NULL) {
    if (output != stdout) {
      fclose(output);
    }
    mpz_clear(result);
    return EXIT_FAILURE;
  }

  if (verbose) {
    size_t result_len = strlen(result_str);
    fprintf(stderr, "Result has %zu digits in %s format\n", result_len,
            format == DECIMAL ? "decimal" : (format == HEXADECIMAL ? "hexadecimal" : "binary"));
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

  if (free_args) {
    free_generated_args(argc, argv);
  }

  if (verbose) {
    fprintf(stderr, "Program completed successfully\n");
  }

  return EXIT_SUCCESS;
}
