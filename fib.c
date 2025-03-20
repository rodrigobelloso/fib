#if __STDC_VERSION__ < 199901L
#error "C99 Minimum Required"
#endif

#include <errno.h>
#include <gmp.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

int main(int argc, char *argv[argc + 1]) {
  if (argc < 2 || argc > 3) {
    fprintf(stderr, "Usage: %s <limit> [-t]\n", argv[0]);
    return EXIT_FAILURE;
  }

  int show_time = 0;
  int limit_arg_idx = 1;

  if (argc == 3) {
    if (strcmp(argv[1], "-t") == 0) {
      show_time = 1;
      limit_arg_idx = 2;
    } else if (strcmp(argv[2], "-t") == 0) {
      show_time = 1;
      limit_arg_idx = 1;
    } else {
      fprintf(stderr, "Usage: %s <limit> [-t]\n", argv[0]);
      return EXIT_FAILURE;
    }
  }

  char *end = argv[limit_arg_idx];
  errno = 0;
  const long limit = strtol(argv[limit_arg_idx], &end, 10);
  if ((argv[limit_arg_idx] == end) || *end) {
    fprintf(stderr, "Error Parsing %s\n", argv[limit_arg_idx]);
    return EXIT_FAILURE;
  } else if (errno == ERANGE) {
    perror(argv[limit_arg_idx]);
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

  if (printf("Fibonacci Number %ld: ", i) < 0) {
    return EXIT_FAILURE;
  }
  if (gmp_printf("%Zd", b) < 0) {
    return EXIT_FAILURE;
  }
  if (putchar('\n') == EOF) {
    return EXIT_FAILURE;
  }

  mpz_clear(a);
  mpz_clear(b);
  mpz_clear(c);

  if (show_time) {
    const double time_taken =
        ((double)(end_time - start_time)) / (double)CLOCKS_PER_SEC;
    if (printf("Calculation Time: %lf seconds\n", time_taken) < 0) {
      return EXIT_FAILURE;
    }
  }

  if (fflush(stdout) == EOF) {
    return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}
