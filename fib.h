#ifndef FIB_H
#define FIB_H

#include <gmp.h>

typedef enum { ITERATIVE, RECURSIVE, MATRIX } Algorithm;
typedef enum { DECIMAL, HEXADECIMAL, BINARY } OutputFormat;

void calculate_fibonacci_iterative(mpz_t result, long n, int verbose);
void calculate_fibonacci_recursive(mpz_t result, long n, mpz_t *memo, int verbose);
void calculate_fibonacci_matrix(mpz_t result, long n, int verbose);

void matrix_multiply(mpz_t a11,
                     mpz_t a12,
                     mpz_t a21,
                     mpz_t a22,
                     mpz_t b11,
                     mpz_t b12,
                     mpz_t b21,
                     mpz_t b22,
                     mpz_t c11,
                     mpz_t c12,
                     mpz_t c21,
                     mpz_t c22);
void matrix_power(mpz_t a11,
                  mpz_t a12,
                  mpz_t a21,
                  mpz_t a22,
                  long n,
                  mpz_t result11,
                  mpz_t result12,
                  mpz_t result21,
                  mpz_t result22,
                  int verbose);

void display_help(const char *program_name);
char *get_formatted_result(mpz_t result, OutputFormat format, int verbose);
const char *get_format_prefix(OutputFormat format);

#endif
