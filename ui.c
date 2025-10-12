#define _POSIX_C_SOURCE 200809L
#define _DEFAULT_SOURCE
#define _BSD_SOURCE

#include "fib.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>

#define MAX_INPUT_SIZE 100

static char *my_strdup(const char *str) {
  size_t len = strlen(str) + 1;
  char *new_str = malloc(len);
  if (new_str) {
    memcpy(new_str, str, len);
  }
  return new_str;
}

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

  return my_strdup(valid_options[option_index]);
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

  return my_strdup(input);
}

/**
 * Runs the interactive user interface for the Fibonacci calculator.
 *
 * This function provides a comprehensive interactive interface that guides users
 * through all available options for calculating Fibonacci numbers. It collects
 * user preferences and constructs command-line arguments accordingly.
 *
 * argc Pointer to the argument count, which will be updated with the new count
 * argv Pointer to the argument vector, which will be replaced with newly generated arguments
 *
 * The function performs the following steps:
 * 1. Prompts the user for the Fibonacci number to calculate (0-1000000)
 * 2. Allows selection of calculation algorithm (iter, recur, or matrix)
 * 3. Allows selection of output format (dec, hex, or bin)
 * 4. Configures timing options (show time, time-only mode)
 * 5. Configures output options (raw output, verbose mode)
 * 6. Optionally specifies an output file
 * 7. Constructs a new argv array based on user choices
 * 8. Updates argc and argv pointers with the new values
 */
void run_user_interface(int *argc, char ***argv) {
  printf("\n===== fib =====\n\n");

  // Step 1: Get the Fibonacci number to calculate
  long fib_number = get_long_input("Enter the Fibonacci number to calculate", 0, 1000000);

  // Step 2: Configure the calculation algorithm
  // Available algorithms: iterative (default), recursive, and matrix-based
  const char *algorithm_options[] = {"iter", "recur", "matrix"};
  printf("Algorithm: 'iter' is the default\n");
  char *algorithm = my_strdup("iter");
  printf("Selected algorithm: %s\n", algorithm);

  if (get_yes_no("Do you want to change the algorithm?", 0)) {
    free(algorithm);
    algorithm = get_string_option("Choose algorithm", algorithm_options, 3);
  }

  // Step 3: Configure the output format
  // Available formats: decimal (default), hexadecimal, and binary
  const char *format_options[] = {"dec", "hex", "bin"};
  printf("Format: 'dec' is the default\n");
  char *format = my_strdup("dec");
  printf("Selected format: %s\n", format);

  if (get_yes_no("Do you want to change the output format?", 0)) {
    free(format);
    format = get_string_option("Choose output format", format_options, 3);
  }

  // Step 4: Configure timing options
  // Users can choose to display calculation time, or only show time without the result
  int show_time = get_yes_no("Do you want to show calculation time?", 0);

  int time_only = 0;
  if (show_time) {
    time_only = get_yes_no("Do you want to show ONLY the time (skip result output)?", 0);
  }

  // Step 5: Configure output display options
  // Raw output shows only the number, verbose shows detailed calculation info
  int raw_output = get_yes_no("Do you want to show only the raw number?", 0);

  int verbose = get_yes_no("Do you want to show detailed calculation information?", 0);

  // Step 6: Configure optional output file
  // If specified, results will be written to a file instead of stdout
  // Note: The path will be validated by validate_output_path() in main()
  // to prevent tainted path vulnerabilities (CWE-73)
  char *output_file = get_filename("Enter the output file name", 1);

  // Step 7: Calculate the number of arguments needed
  // Start with 2 (program name + fibonacci number)
  int new_argc = 2;
  if (strcmp(algorithm, "iter") != 0)
    new_argc += 2;  // -a <algorithm>
  if (strcmp(format, "dec") != 0)
    new_argc += 2;  // -f <format>
  if (show_time)
    new_argc += 1;  // -t flag
  if (time_only)
    new_argc += 1;  // -T flag
  if (raw_output)
    new_argc += 1;  // -r flag
  if (verbose)
    new_argc += 1;  // -v flag
  if (output_file)
    new_argc += 2;  // -o <filename>

  // Step 8: Allocate memory for the new argument array
  char **new_argv = (char **) malloc(new_argc * sizeof(char *));
  if (!new_argv) {
    fprintf(stderr, "Error: Could not allocate memory for arguments\n");
    exit(EXIT_FAILURE);
  }

  // Step 9: Build the new argument array
  // First argument is always the program name
  new_argv[0] = my_strdup((*argv)[0]);

  // Second argument is the Fibonacci number
  char number_str[32];
  sprintf(number_str, "%ld", fib_number);
  new_argv[1] = my_strdup(number_str);

  int arg_index = 2;

  // Add algorithm option if not using default
  if (strcmp(algorithm, "iter") != 0) {
    new_argv[arg_index++] = my_strdup("-a");
    new_argv[arg_index++] = my_strdup(algorithm);
  }

  // Add format option if not using default
  if (strcmp(format, "dec") != 0) {
    new_argv[arg_index++] = my_strdup("-f");
    new_argv[arg_index++] = my_strdup(format);
  }

  // Add timing flags if requested
  if (show_time)
    new_argv[arg_index++] = my_strdup("-t");
  if (time_only)
    new_argv[arg_index++] = my_strdup("-T");

  // Add output display flags if requested
  if (raw_output)
    new_argv[arg_index++] = my_strdup("-r");
  if (verbose)
    new_argv[arg_index++] = my_strdup("-v");

  // Add output file option if specified
  // Note: User input from get_filename() will be validated by
  // validate_output_path() when processed by main() to prevent
  // tainted path vulnerabilities (CWE-73)
  if (output_file) {
    new_argv[arg_index++] = my_strdup("-o");
    new_argv[arg_index++] = my_strdup(output_file);
    free(output_file);
  }

  // Step 10: Display configuration summary to the user
  printf("\n===== Configuration Summary =====\n");
  printf("Fibonacci Number: %ld\n", fib_number);
  printf("Algorithm: %s\n", algorithm);
  printf("Format: %s\n", format);
  printf("Show time: %s\n", show_time ? "Yes" : "No");
  printf("Time only (no result): %s\n", time_only ? "Yes" : "No");
  printf("Raw output: %s\n", raw_output ? "Yes" : "No");
  printf("Detailed information: %s\n", verbose ? "Yes" : "No");
  printf("Output file: %s\n", output_file ? output_file : "No (standard output)");
  printf("\nProcessing...\n\n");

  // Clean up temporary strings that are no longer needed
  free(algorithm);
  free(format);

  // Step 11: Update the caller's argc and argv with the new values
  // This allows the main program to process the constructed arguments
  *argc = new_argc;
  *argv = new_argv;
}

void free_generated_args(int argc, char **argv) {
  for (int i = 0; i < argc; i++) {
    free(argv[i]);
  }
  free(argv);
}
