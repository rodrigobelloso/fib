#if __STDC_VERSION__ < 199901L
#error "C99 Minimum Required"
#endif

#include <errno.h>
#include <gmp.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

void display_help(const char *program_name) {
  printf("\nUsage: %s <limit> [options]\n", program_name);
  printf("\n");
  printf("Calculate Fibonacci numbers.\n");
  printf("\n");
  printf("Options:\n");
  printf("  -h, --help    Display this help message and exit.\n");
  printf("  -t, --time    Show calculation time.\n");
  printf("  -r, --raw     Output only the number without prefix.\n");
  printf("  -o, --output  Save the result to the specified file.\n");
  printf("  %s 10         Calculate the 10th Fibonacci number.\n", program_name);
  printf("  %s 100 -t     Calculate and show the time taken.\n", program_name);
  printf("  %s 50 -r      Calculate and save raw result to a file.\n", program_name);
  printf("\n");
}

int main(int argc, const char *const argv[argc + 1]) {
  if (argc < 2) {
    fprintf(stderr, "Usage: %s <limit> [-h] [-t] [-r] [-o filename]\n", argv[0]);
    fprintf(stderr, "Try '%s --help' for more information.\n", argv[0]);
    return EXIT_FAILURE;
  }

  int show_time = 0;
  int raw_output = 0;
  long limit = -1;
  char const *output_file = NULL;

  for (int i = 1; i < argc; i++) {
    if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0) {
      display_help(argv[0]);
      return EXIT_SUCCESS;
    } else if (strcmp(argv[i], "-t") == 0 || strcmp(argv[i], "--time") == 0) {
      show_time = 1;
    } else if (strcmp(argv[i], "-r") == 0 || strcmp(argv[i], "--raw") == 0) {
      raw_output = 1;
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
      fprintf(stderr, "Usage: %s <limit> [-h] [-t] [-r] [-o filename]\n", argv[0]);
      fprintf(stderr, "Try '%s --help' for more information.\n", argv[0]);
      return EXIT_FAILURE;
    }
  }

  if (limit == -1) {
    fprintf(stderr, "Error: Missing limit value\n");
    fprintf(stderr, "Try '%s --help' for more information.\n", argv[0]);
    return EXIT_FAILURE;
  }

  mpz_t a, b, c;
  mpz_init_set_ui(a, 1);
  mpz_init_set_ui(b, 0);
  mpz_init(c);

  clock_t start_time = (clock_t)0;
  if (show_time) {
    start_time = clock();
    if (start_time == (clock_t)-1) {
      fprintf(stderr, "Error start_time clock()\n");
      return EXIT_FAILURE;
    }
  }

  long i = 0;
  while (i < limit) {
    mpz_add(c, a, b);
    mpz_set(a, b);
    mpz_set(b, c);
    ++i;
  }

  clock_t end_time = (clock_t)0;
  if (show_time) {
    end_time = clock();
    if (end_time == (clock_t)-1) {
      fprintf(stderr, "Error end_time clock()\n");
      return EXIT_FAILURE;
    }
  }

  FILE *output = stdout;
  if (output_file != NULL) {
    output = fopen(output_file, "w");
    if (output == NULL) {
      perror("Error opening output file");
      mpz_clear(a);
      mpz_clear(b);
      mpz_clear(c);
      return EXIT_FAILURE;
    }
  }

  if (!raw_output) {
    if (fprintf(output, "Fibonacci Number %ld: ", i) < 0) {
      if (output != stdout) {
        fclose(output);
      }
      mpz_clear(a);
      mpz_clear(b);
      mpz_clear(c);
      return EXIT_FAILURE;
    }
  }

  char *result_str = mpz_get_str(NULL, 10, b);
  if (result_str == NULL) {
    if (output != stdout) {
      fclose(output);
    }
    mpz_clear(a);
    mpz_clear(b);
    mpz_clear(c);
    return EXIT_FAILURE;
  }

  if (fprintf(output, "%s\n", result_str) < 0) {
    free(result_str);
    if (output != stdout) {
      fclose(output);
    }
    mpz_clear(a);
    mpz_clear(b);
    mpz_clear(c);
    return EXIT_FAILURE;
  }

  free(result_str);

  if (show_time) {
    const double time_taken =
        ((double)(end_time - start_time)) / (double)CLOCKS_PER_SEC;
    if (fprintf(output, "Calculation Time: %lf seconds\n", time_taken) < 0) {
      if (output != stdout) {
        fclose(output);
      }
      mpz_clear(a);
      mpz_clear(b);
      mpz_clear(c);
      return EXIT_FAILURE;
    }
  }

  if (output != stdout) {
    if (fclose(output) != 0) {
      perror("Error closing output file");
      mpz_clear(a);
      mpz_clear(b);
      mpz_clear(c);
      return EXIT_FAILURE;
    }
  } else {
    if (fflush(stdout) == EOF) {
      mpz_clear(a);
      mpz_clear(b);
      mpz_clear(c);
      return EXIT_FAILURE;
    }
  }

  mpz_clear(a);
  mpz_clear(b);
  mpz_clear(c);
  return EXIT_SUCCESS;
}
