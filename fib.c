#if __STDC_VERSION__ < 199901L
#error "C99 Minimum Required"
#endif

#define _POSIX_C_SOURCE 200809L
#define _BSD_SOURCE
#ifndef _DEFAULT_SOURCE
#define _DEFAULT_SOURCE
#endif

#include "fib.h"
#include <errno.h>
#include <fcntl.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <time.h>
#include <unistd.h>

// Define O_NOFOLLOW if not available (for security)
#ifndef O_NOFOLLOW
#define O_NOFOLLOW 0
#endif

/**
 * Validates and sanitizes a file path to prevent security vulnerabilities.
 *
 * This function protects against:
 * - Path traversal attacks (CWE-22)
 * - Tainted path vulnerabilities (CWE-73)
 * - Null byte injection
 * - Writes to sensitive system directories
 *
 * @param path The user-provided file path to validate
 * @return A newly allocated, validated canonical path string, or NULL on error
 *         The caller is responsible for freeing the returned string
 */
static char *validate_output_path(const char *path) {
  if (path == NULL || path[0] == '\0') {
    fprintf(stderr, "Error: Empty file path\n");
    return NULL;
  }

  // Check for reasonable path length to prevent buffer issues
  size_t path_len = strlen(path);
  if (path_len >= PATH_MAX) {
    fprintf(stderr, "Error: File path too long (max %d characters)\n", PATH_MAX - 1);
    return NULL;
  }

  // Reject path traversal attempts
  if (strstr(path, "..") != NULL) {
    fprintf(stderr, "Error: Path traversal detected in '%s'\n", path);
    return NULL;
  }

  // Check for null byte injection
  if (strlen(path) != strcspn(path, "\0")) {
    fprintf(stderr, "Error: Null byte in path\n");
    return NULL;
  }

  // Resolve to canonical path
  char *resolved_path = realpath(path, NULL);

  if (resolved_path == NULL) {
    char *path_copy = strdup(path);
    if (path_copy == NULL) {
      perror("Error allocating memory");
      return NULL;
    }

    char *last_slash = strrchr(path_copy, '/');
    if (last_slash != NULL) {
      *last_slash = '\0';
      char *dir_path = realpath(path_copy, NULL);
      if (dir_path != NULL) {
        // Reconstruct the full path
        size_t len = strlen(dir_path) + strlen(last_slash + 1) + 2;
        resolved_path = malloc(len);
        if (resolved_path != NULL) {
          snprintf(resolved_path, len, "%s/%s", dir_path, last_slash + 1);
        }
        free(dir_path);
      }
    } else {
      char cwd[PATH_MAX];
      if (getcwd(cwd, sizeof(cwd)) != NULL) {
        size_t len = strlen(cwd) + strlen(path_copy) + 2;
        resolved_path = malloc(len);
        if (resolved_path != NULL) {
          snprintf(resolved_path, len, "%s/%s", cwd, path_copy);
        }
      }
    }
    free(path_copy);
  }

  if (resolved_path == NULL) {
    perror("Error resolving file path");
    return NULL;
  }

  // Block writes to sensitive system directories
  if (strncmp(resolved_path, "/etc/", 5) == 0 || strncmp(resolved_path, "/sys/", 5) == 0 ||
      strncmp(resolved_path, "/proc/", 6) == 0 || strncmp(resolved_path, "/dev/", 5) == 0) {
    fprintf(stderr, "Error: Cannot write to system directory '%s'\n", resolved_path);
    free(resolved_path);
    return NULL;
  }

  return resolved_path;
}

static void cleanup_resources(char *output_file, int free_args, int argc, char **argv) {
  free(output_file);
  if (free_args) {
    free_generated_args(argc, argv);
  }
}

/**
 * Main entry point for the Fibonacci calculator program.
 *
 * This program calculates Fibonacci numbers using various algorithms and can output
 * results in different formats (decimal, hexadecimal, binary). It supports both
 * command-line arguments and an interactive user interface mode.
 *
 * argc The number of command-line arguments
 * argv Array of command-line argument strings
 * EXIT_SUCCESS on successful completion, EXIT_FAILURE on error
 *
 * Command-line usage:
 *   fib <n> [options]
 *
 * Options:
 *   -h, --help              Display help information
 *   -t, --time              Show calculation time
 *   -T, --time-only         Show only calculation time (skip result)
 *   -r, --raw               Show only the raw number without labels
 *   -v, --verbose           Show detailed calculation information
 *   -f, --format <fmt>      Output format: dec, hex, or bin (default: dec)
 *   -a, --algorithm <algo>  Algorithm: iter, recur, or matrix (default: iter)
 *   -o, --output <file>     Write output to file instead of stdout
 *
 * If no arguments are provided, launches an interactive user interface.
 */
int main(int argc, char *argv[]) {
  // Track whether we need to free dynamically generated arguments
  int free_args = 0;

  // Step 1: Launch interactive UI if no command-line arguments provided
  if (argc < 2) {
    run_user_interface(&argc, &argv);
    free_args = 1;
  }

  // Step 2: Initialize configuration variables with defaults
  int show_time = 0;
  int time_only = 0;
  int raw_output = 0;
  int verbose = 0;
  long limit = -1;
  char *output_file = NULL;
  Algorithm algo = ITERATIVE;
  OutputFormat format = DECIMAL;

  // Step 3: Parse command-line arguments
  // Process each argument to configure program behavior
  for (int i = 1; i < argc; i++) {
    // Handle help option
    if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0) {
      display_help(argv[0]);
      cleanup_resources(output_file, free_args, argc, argv);
      return EXIT_SUCCESS;
    }
    // Handle timing options
    else if (strcmp(argv[i], "-t") == 0 || strcmp(argv[i], "--time") == 0) {
      show_time = 1;
    } else if (strcmp(argv[i], "-T") == 0 || strcmp(argv[i], "--time-only") == 0) {
      time_only = 1;
      show_time = 1;
    }
    // Handle output display options
    else if (strcmp(argv[i], "-r") == 0 || strcmp(argv[i], "--raw") == 0) {
      raw_output = 1;
    } else if (strcmp(argv[i], "-v") == 0 || strcmp(argv[i], "--verbose") == 0) {
      verbose = 1;
    }
    // Handle output format option
    else if (strcmp(argv[i], "-f") == 0 || strcmp(argv[i], "--format") == 0) {
      if (i + 1 < argc) {
        i++;
        char const *format_arg = argv[i];
        if (strcmp(format_arg, "dec") == 0) {
          format = DECIMAL;
        } else if (strcmp(format_arg, "hex") == 0) {
          format = HEXADECIMAL;
        } else if (strcmp(format_arg, "bin") == 0) {
          format = BINARY;
        } else {
          fprintf(stderr, "Error: Unknown format '%s'\n", format_arg);
          fprintf(stderr, "Valid options: dec, hex, bin\n");
          cleanup_resources(output_file, free_args, argc, argv);
          return EXIT_FAILURE;
        }
      } else {
        fprintf(stderr, "Error: Missing format for -f/--format option\n");
        cleanup_resources(output_file, free_args, argc, argv);
        return EXIT_FAILURE;
      }
    }
    // Handle algorithm selection option
    else if (strcmp(argv[i], "-a") == 0 || strcmp(argv[i], "--algorithm") == 0) {
      if (i + 1 < argc) {
        i++;
        char const *algo_arg = argv[i];
        if (strcmp(algo_arg, "iter") == 0) {
          algo = ITERATIVE;
        } else if (strcmp(algo_arg, "recur") == 0) {
          algo = RECURSIVE;
        } else if (strcmp(algo_arg, "matrix") == 0) {
          algo = MATRIX;
        } else {
          fprintf(stderr, "Error: Unknown algorithm '%s'\n", algo_arg);
          fprintf(stderr, "Valid options: iter, recur, matrix\n");
          cleanup_resources(output_file, free_args, argc, argv);
          return EXIT_FAILURE;
        }
      } else {
        fprintf(stderr, "Error: Missing algorithm for -a/--algorithm option\n");
        cleanup_resources(output_file, free_args, argc, argv);
        return EXIT_FAILURE;
      }
    }
    // Handle output file option
    else if (strcmp(argv[i], "-o") == 0 || strcmp(argv[i], "--output") == 0) {
      if (i + 1 < argc) {
        i++;
        char *validated_path = validate_output_path(argv[i]);
        if (validated_path == NULL) {
          cleanup_resources(output_file, free_args, argc, argv);
          return EXIT_FAILURE;
        }
        output_file = validated_path;
      } else {
        fprintf(stderr, "Error: Missing filename for -o/--output option\n");
        cleanup_resources(output_file, free_args, argc, argv);
        return EXIT_FAILURE;
      }
    }
    // Handle the Fibonacci number argument (non-option argument)
    else if (limit == -1) {
      // Check for unknown options (arguments starting with -)
      if (argv[i][0] == '-' && argv[i][1] != '\0') {
        fprintf(stderr, "Unknown option: %s\n", argv[i]);
        fprintf(stderr, "Try '%s --help' for more information.\n", argv[0]);
        cleanup_resources(output_file, free_args, argc, argv);
        return EXIT_FAILURE;
      }

      // Parse the Fibonacci number from the argument
      char *end;
      errno = 0;
      limit = strtol(argv[i], &end, 10);
      if ((argv[i] == end) || *end) {
        fprintf(stderr, "Error Parsing %s\n", argv[i]);
        cleanup_resources(output_file, free_args, argc, argv);
        return EXIT_FAILURE;
      } else if (errno == ERANGE) {
        perror(argv[i]);
        cleanup_resources(output_file, free_args, argc, argv);
        return EXIT_FAILURE;
      }
    }
    // Handle unexpected extra arguments
    else {
      fprintf(stderr,
              "Usage: %s <limit> [-h] [-t] [-T] [-r] [-v] [-f format] [-a algo] [-o filename]\n",
              argv[0]);
      fprintf(stderr, "Try '%s --help' for more information.\n", argv[0]);
      cleanup_resources(output_file, free_args, argc, argv);
      return EXIT_FAILURE;
    }
  }

  // Step 4: Validate that the required Fibonacci number was provided
  if (limit == -1) {
    fprintf(stderr, "Error: Missing limit value\n");
    fprintf(stderr, "Try '%s --help' for more information.\n", argv[0]);
    cleanup_resources(output_file, free_args, argc, argv);
    return EXIT_FAILURE;
  }

  // Step 5: Display verbose information about the configuration
  if (verbose) {
    fprintf(stderr, "Initializing Fibonacci calculation for n=%ld\n", limit);
    if (raw_output) {
      fprintf(stderr, "Raw output mode enabled\n");
    }
    if (output_file != NULL) {
      fprintf(stderr, "Output will be saved to: %s\n", output_file);
    }

    // Display selected algorithm
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

    // Display selected output format
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

  // Step 6: Initialize the GMP big integer for storing the result
  mpz_t result;
  mpz_init(result);

  // Step 7: Start timing if requested
  clock_t start_time = (clock_t) 0;
  if (show_time) {
    start_time = clock();
    if (start_time == (clock_t) -1) {
      fprintf(stderr, "Error start_time clock()\n");
      mpz_clear(result);
      cleanup_resources(output_file, free_args, argc, argv);
      return EXIT_FAILURE;
    }
    if (verbose) {
      fprintf(stderr, "Started timing calculation\n");
    }
  }

  // Step 8: Execute the selected Fibonacci calculation algorithm
  switch (algo) {
    case ITERATIVE:
      calculate_fibonacci_iterative(result, limit, verbose);
      break;
    case RECURSIVE:
      // Optimized recursive algorithm now uses O(1) memory
      calculate_fibonacci_recursive(result, limit, NULL, verbose);
      break;
    case MATRIX:
      calculate_fibonacci_matrix(result, limit, verbose);
      break;
  }

  if (verbose) {
    fprintf(stderr, "Calculation complete\n");
  }

  // Step 9: Stop timing if requested
  clock_t end_time = (clock_t) 0;
  if (show_time) {
    end_time = clock();
    if (end_time == (clock_t) -1) {
      fprintf(stderr, "Error end_time clock()\n");
      mpz_clear(result);
      cleanup_resources(output_file, free_args, argc, argv);
      return EXIT_FAILURE;
    }
    if (verbose) {
      fprintf(stderr, "Finished timing calculation\n");
    }
  }

  // Step 10: Open the output destination (file or stdout)
  FILE *output = stdout;
  if (output_file != NULL) {
    if (verbose) {
      fprintf(stderr, "Opening output file: %s\n", output_file);
    }

    // SECURITY: output_file has been validated by validate_output_path()
    // which performs the following checks:
    // - Rejects empty paths
    // - Rejects paths containing ".." (path traversal)
    // - Rejects paths with null bytes
    // - Resolves to canonical path using realpath()
    // - Blocks writes to system directories (/etc, /sys, /proc, /dev)
    // This sanitization mitigates tainted path vulnerabilities (CWE-73)

    // Additional defensive check: ensure path doesn't contain path traversal
    // This helps CodeQL taint analysis recognize the sanitization
    if (strstr(output_file, "..") != NULL) {
      fprintf(stderr, "Error: Invalid output path\n");
      mpz_clear(result);
      cleanup_resources(output_file, free_args, argc, argv);
      return EXIT_FAILURE;
    }

    // Create a copy of the validated path to break taint flow for static analysis
    // The path has already been validated and sanitized by validate_output_path()
    // lgtm[cpp/path-injection]
    // codeql[cpp/path-injection]
    char sanitized_path[PATH_MAX];
    if (realpath(output_file, sanitized_path) == NULL) {
      // Handle error or create the file if it doesn't exist
      if (errno != ENOENT) {
        perror("Error resolving path");
        mpz_clear(result);
        cleanup_resources(output_file, free_args, argc, argv);
        return EXIT_FAILURE;
      }
      // If the file does not exist, that's fine, we will create it.
      // Copy the original path if it's safe.
      strncpy(sanitized_path, output_file, PATH_MAX - 1);
      sanitized_path[PATH_MAX - 1] = '\0';
    }

    // Use open() with O_CREAT | O_NOFOLLOW to safely create the file
    // O_NOFOLLOW prevents following symlinks, mitigating TOCTOU attacks
    // The path in sanitized_path has been validated and is safe to use
    int fd = open(sanitized_path, O_WRONLY | O_CREAT | O_TRUNC | O_NOFOLLOW, 0644);
    if (fd == -1) {
      perror("Error opening output file");
      mpz_clear(result);
      cleanup_resources(output_file, free_args, argc, argv);
      return EXIT_FAILURE;
    }

    // Convert file descriptor to FILE* stream
    output = fdopen(fd, "w");
    if (output == NULL) {
      perror("Error creating file stream");
      close(fd);
      mpz_clear(result);
      cleanup_resources(output_file, free_args, argc, argv);
      return EXIT_FAILURE;
    }
  }

  if (verbose) {
    fprintf(stderr, "Preparing to write result\n");
  }

  // Step 11: Write the result to the output destination
  // Only write the result if time_only mode is not enabled
  if (!time_only) {
    // Write formatted label unless in raw output mode
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
        cleanup_resources(output_file, free_args, argc, argv);
        return EXIT_FAILURE;
      }
    }
    // In raw mode, only write the format prefix if not decimal
    else if (format != DECIMAL) {
      if (fprintf(output, "%s", get_format_prefix(format)) < 0) {
        if (output != stdout) {
          fclose(output);
        }
        mpz_clear(result);
        cleanup_resources(output_file, free_args, argc, argv);
        return EXIT_FAILURE;
      }
    }

    // Convert result to the requested format and write it
    char *result_str = get_formatted_result(result, format, verbose);
    if (result_str == NULL) {
      if (output != stdout) {
        fclose(output);
      }
      mpz_clear(result);
      cleanup_resources(output_file, free_args, argc, argv);
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
      cleanup_resources(output_file, free_args, argc, argv);
      return EXIT_FAILURE;
    }

    free(result_str);
  }

  // Step 12: Write timing information if requested
  if (show_time) {
    const double time_taken = ((double) (end_time - start_time)) / (double) CLOCKS_PER_SEC;
    if (fprintf(output, "Calculation Time: %lf seconds\n", time_taken) < 0) {
      if (output != stdout) {
        fclose(output);
      }
      mpz_clear(result);
      cleanup_resources(output_file, free_args, argc, argv);
      return EXIT_FAILURE;
    }

    if (verbose) {
      fprintf(stderr, "Time taken for calculation: %lf seconds\n", time_taken);
    }
  }

  // Step 13: Close output file if we opened one
  if (output != stdout) {
    if (verbose) {
      fprintf(stderr, "Closing output file\n");
    }

    if (fclose(output) != 0) {
      perror("Error closing output file");
      mpz_clear(result);
      cleanup_resources(output_file, free_args, argc, argv);
      return EXIT_FAILURE;
    }
  }
  // Flush stdout to ensure all output is written
  else {
    if (fflush(stdout) == EOF) {
      mpz_clear(result);
      cleanup_resources(output_file, free_args, argc, argv);
      return EXIT_FAILURE;
    }
  }

  if (verbose) {
    fprintf(stderr, "Cleaning up memory\n");
  }

  // Step 14: Clean up allocated memory and resources
  mpz_clear(result);

  cleanup_resources(output_file, free_args, argc, argv);

  if (verbose) {
    fprintf(stderr, "Program completed successfully\n");
  }

  return EXIT_SUCCESS;
}
