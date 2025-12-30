#ifndef FIB_H
#define FIB_H

#include <gmp.h>
#include <time.h>

typedef enum { ITERATIVE, RECURSIVE, MATRIX } Algorithm;
typedef enum { DECIMAL, HEXADECIMAL, BINARY } OutputFormat;

#define MAX_HISTORY_ENTRIES 100

typedef struct {
  long fib_number;
  Algorithm algorithm;
  OutputFormat format;
  double calc_time;
  time_t timestamp;
  char result_preview[65];  // First 64 chars of result + null terminator
} HistoryEntry;

void calculate_fibonacci_iterative(mpz_t result, long n, int verbose);
void calculate_fibonacci_recursive(mpz_t result, long n, void *unused, int verbose);
void calculate_fibonacci_matrix(mpz_t result, long n, int verbose);

void matrix_multiply(mpz_t a11, mpz_t a12, mpz_t a21, mpz_t a22, mpz_t b11, mpz_t b12, mpz_t b21,
                     mpz_t b22, mpz_t c11, mpz_t c12, mpz_t c21, mpz_t c22);

void matrix_power(mpz_t a11, mpz_t a12, mpz_t a21, mpz_t a22, long n, mpz_t result11,
                  mpz_t result12, mpz_t result21, mpz_t result22, int verbose);

void display_help(const char *program_name);
char *get_formatted_result(mpz_t result, OutputFormat format, int verbose);
const char *get_format_prefix(OutputFormat format);

void run_user_interface(int *argc, char ***argv);
void free_generated_args(int argc, char **argv);

// History management functions
int load_history(HistoryEntry **history, int *count);
int save_history(const HistoryEntry *history, int count);
int add_to_history(long fib_number, Algorithm algorithm, OutputFormat format, double calc_time,
                   const char *result_str);
void display_history(void);
const char *algorithm_to_string(Algorithm algo);
const char *format_to_string(OutputFormat fmt);

#endif
