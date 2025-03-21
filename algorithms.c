#include "fib.h"
#include <stdio.h>
#include <stdlib.h>

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
