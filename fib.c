#if __STDC_VERSION__ < 199901L
#error "C99 Minimum Required"
#endif

#include <errno.h>
#include <gmp.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main(int argc, char *argv[argc + 1]) {
  if (argc != 2) {
    fprintf(stderr, "Usage: %s <limit>\n", argv[0]);
    return EXIT_FAILURE;
  }

  char *end = argv[1];
  errno = 0;
  const long limit = strtol(argv[1], &end, 10);
  if ((argv[1] == end) || *end) {
    fprintf(stderr, "Error Parsing %s\n", argv[1]);
    return EXIT_FAILURE;
  } else if (errno == ERANGE) {
    perror(argv[1]);
    return EXIT_FAILURE;
  }

  mpz_t a, b, c;
  mpz_init_set_ui(a, 1);
  mpz_init_set_ui(b, 0);
  mpz_init(c);

  const clock_t start_time = clock();
  if (start_time == (clock_t)-1) {
    fprintf(stderr, "Error start_time clock()\n");
    return EXIT_FAILURE;
  }

  long i = 0;
  while (i < limit) {
    mpz_add(c, a, b);
    mpz_set(a, b);
    mpz_set(b, c);
    ++i;
  }

  const clock_t end_time = clock();
  if (end_time == (clock_t)-1) {
    fprintf(stderr, "Error end_time clock()\n");
    return EXIT_FAILURE;
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

  const double time_taken =
      ((double)(end_time - start_time)) / (double)CLOCKS_PER_SEC;
  if (printf("Calculation Time: %lf seconds\n", time_taken) < 0) {
    return EXIT_FAILURE;
  }
  if (fflush(stdout) == EOF) {
    return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}
