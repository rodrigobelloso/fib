#include "fib.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>

#define MAX_INPUT_SIZE 100

static long get_long_input(const char *prompt, long min_value, long max_value) {
  char input[MAX_INPUT_SIZE];
  long value;
  char *endptr;
  int valid = 0;

  do {
    printf("%s (between %ld and %ld): ", prompt, min_value, max_value);

    if (!fgets(input, MAX_INPUT_SIZE, stdin)) {
      printf("Error reading input. Please try again.\n");
      continue;
    }

    size_t len = strlen(input);
    if (len > 0 && input[len - 1] == '\n')
      input[len - 1] = '\0';

    if (input[0] == '\0') {
      printf("Input cannot be empty. Please try again.\n");
      continue;
    }

    errno = 0;
    value = strtol(input, &endptr, 10);

    if (errno == ERANGE) {
      printf("Number out of range. Please try again.\n");
      continue;
    }

    if (endptr == input || *endptr != '\0') {
      printf("Invalid input. Please enter an integer.\n");
      continue;
    }

    if (value < min_value || value > max_value) {
      printf("Number must be between %ld and %ld. Please try again.\n", min_value, max_value);
      continue;
    }

    valid = 1;
  } while (!valid);

  return value;
}

static char *get_string_option(const char *prompt, const char *valid_options[], int num_options) {
  char input[MAX_INPUT_SIZE];
  int valid = 0;
  int option_index = -1;

  do {
    printf("%s (", prompt);
    for (int i = 0; i < num_options; i++) {
      printf("%s", valid_options[i]);
      if (i < num_options - 1)
        printf("/");
    }
    printf("): ");

    if (!fgets(input, MAX_INPUT_SIZE, stdin)) {
      printf("Error reading input. Please try again.\n");
      continue;
    }

    size_t len = strlen(input);
    if (len > 0 && input[len - 1] == '\n')
      input[len - 1] = '\0';

    if (input[0] == '\0') {
      printf("Input cannot be empty. Please try again.\n");
      continue;
    }

    for (int i = 0; i < num_options; i++) {
      if (strcmp(input, valid_options[i]) == 0) {
        option_index = i;
        valid = 1;
        break;
      }
    }

    if (!valid) {
      printf("Invalid option. Please try again.\n");
    }
  } while (!valid);

  return strdup(valid_options[option_index]);
}

static int get_yes_no(const char *prompt, int default_value) {
  char input[MAX_INPUT_SIZE];

  printf("%s (y/n) [%s]: ", prompt, default_value ? "y" : "n");

  if (!fgets(input, MAX_INPUT_SIZE, stdin)) {
    return default_value;
  }

  size_t len = strlen(input);
  if (len > 0 && input[len - 1] == '\n')
    input[len - 1] = '\0';

  if (input[0] == '\0') {
    return default_value;
  }

  for (int i = 0; input[i]; i++) {
    input[i] = tolower((unsigned char) input[i]);
  }

  if (strcmp(input, "y") == 0 || strcmp(input, "yes") == 0) {
    return 1;
  } else {
    return 0;
  }
}

static char *get_filename(const char *prompt, int allow_empty) {
  char input[MAX_INPUT_SIZE];
  int valid = 0;

  do {
    printf("%s%s: ", prompt, allow_empty ? " (press Enter to skip)" : "");

    if (!fgets(input, MAX_INPUT_SIZE, stdin)) {
      printf("Error reading input. Please try again.\n");
      continue;
    }

    size_t len = strlen(input);
    if (len > 0 && input[len - 1] == '\n')
      input[len - 1] = '\0';

    if (input[0] == '\0') {
      if (allow_empty) {
        return NULL;
      } else {
        printf("Input cannot be empty. Please try again.\n");
        continue;
      }
    }

    valid = 1;
  } while (!valid);

  return strdup(input);
}

void run_user_interface(int *argc, char ***argv) {
  printf("\n===== fib =====\n\n");

  long fib_number = get_long_input("Enter the Fibonacci number to calculate", 0, 1000000);

  const char *algorithm_options[] = {"iter", "recur", "matrix"};
  printf("Algorithm: 'iter' is the default\n");
  char *algorithm = strdup("iter");
  printf("Selected algorithm: %s\n", algorithm);

  if (get_yes_no("Do you want to change the algorithm?", 0)) {
    free(algorithm);
    algorithm = get_string_option("Choose algorithm", algorithm_options, 3);
  }

  const char *format_options[] = {"dec", "hex", "bin"};
  printf("Format: 'dec' is the default\n");
  char *format = strdup("dec");
  printf("Selected format: %s\n", format);

  if (get_yes_no("Do you want to change the output format?", 0)) {
    free(format);
    format = get_string_option("Choose output format", format_options, 3);
  }

  int show_time = get_yes_no("Do you want to show calculation time?", 0);

  int raw_output = get_yes_no("Do you want to show only the raw number?", 0);

  int verbose = get_yes_no("Do you want to show detailed calculation information?", 0);

  char *output_file = get_filename("Enter the output file name", 1);

  int new_argc = 2;
  if (strcmp(algorithm, "iter") != 0)
    new_argc += 2;
  if (strcmp(format, "dec") != 0)
    new_argc += 2;
  if (show_time)
    new_argc += 1;
  if (raw_output)
    new_argc += 1;
  if (verbose)
    new_argc += 1;
  if (output_file)
    new_argc += 2;

  char **new_argv = (char **) malloc(new_argc * sizeof(char *));
  if (!new_argv) {
    fprintf(stderr, "Error: Could not allocate memory for arguments\n");
    exit(EXIT_FAILURE);
  }

  new_argv[0] = strdup((*argv)[0]);

  char number_str[32];
  sprintf(number_str, "%ld", fib_number);
  new_argv[1] = strdup(number_str);

  int arg_index = 2;

  if (strcmp(algorithm, "iter") != 0) {
    new_argv[arg_index++] = strdup("-a");
    new_argv[arg_index++] = strdup(algorithm);
  }

  if (strcmp(format, "dec") != 0) {
    new_argv[arg_index++] = strdup("-f");
    new_argv[arg_index++] = strdup(format);
  }

  if (show_time)
    new_argv[arg_index++] = strdup("-t");
  if (raw_output)
    new_argv[arg_index++] = strdup("-r");
  if (verbose)
    new_argv[arg_index++] = strdup("-v");

  if (output_file) {
    new_argv[arg_index++] = strdup("-o");
    new_argv[arg_index++] = strdup(output_file);
    free(output_file);
  }

  printf("\n===== Configuration Summary =====\n");
  printf("Fibonacci Number: %ld\n", fib_number);
  printf("Algorithm: %s\n", algorithm);
  printf("Format: %s\n", format);
  printf("Show time: %s\n", show_time ? "Yes" : "No");
  printf("Raw output: %s\n", raw_output ? "Yes" : "No");
  printf("Detailed information: %s\n", verbose ? "Yes" : "No");
  printf("Output file: %s\n", output_file ? output_file : "No (standard output)");
  printf("\nProcessing...\n\n");

  free(algorithm);
  free(format);

  *argc = new_argc;
  *argv = new_argv;
}

void free_generated_args(int argc, char **argv) {
  for (int i = 0; i < argc; i++) {
    free(argv[i]);
  }
  free(argv);
}
