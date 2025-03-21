#include "fib.h"
#include <stdio.h>

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
