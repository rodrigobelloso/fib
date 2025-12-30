#include "fib.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include <pwd.h>

static char *get_history_file_path(void) {
  const char *home = getenv("HOME");
  if (!home) {
    struct passwd *pw = getpwuid(getuid());
    if (pw) {
      home = pw->pw_dir;
    }
  }

  if (!home) {
    return NULL;
  }

  size_t len = strlen(home) + strlen("/.fib_history") + 1;
  char *path = malloc(len);
  if (path) {
    snprintf(path, len, "%s/.fib_history", home);
  }
  return path;
}

const char *algorithm_to_string(Algorithm algo) {
  switch (algo) {
    case ITERATIVE:
      return "iter";
    case RECURSIVE:
      return "recur";
    case MATRIX:
      return "matrix";
    default:
      return "unknown";
  }
}

const char *format_to_string(OutputFormat fmt) {
  switch (fmt) {
    case DECIMAL:
      return "dec";
    case HEXADECIMAL:
      return "hex";
    case BINARY:
      return "bin";
    default:
      return "unknown";
  }
}

int load_history(HistoryEntry **history, int *count) {
  char *history_path = get_history_file_path();
  if (!history_path) {
    return -1;
  }

  FILE *fp = fopen(history_path, "rb");
  free(history_path);

  if (!fp) {
    *history = NULL;
    *count = 0;
    return 0;  // No history file yet, not an error
  }

  // Read count
  if (fread(count, sizeof(int), 1, fp) != 1) {
    fclose(fp);
    *count = 0;
    return -1;
  }

  if (*count < 0 || *count > MAX_HISTORY_ENTRIES) {
    fclose(fp);
    *count = 0;
    return -1;
  }

  if (*count == 0) {
    fclose(fp);
    *history = NULL;
    return 0;
  }

  *history = malloc(*count * sizeof(HistoryEntry));
  if (!*history) {
    fclose(fp);
    *count = 0;
    return -1;
  }

  size_t read = fread(*history, sizeof(HistoryEntry), *count, fp);
  fclose(fp);

  if ((int) read != *count) {
    free(*history);
    *history = NULL;
    *count = 0;
    return -1;
  }

  return 0;
}

int save_history(const HistoryEntry *history, int count) {
  if (count < 0 || count > MAX_HISTORY_ENTRIES) {
    return -1;
  }

  char *history_path = get_history_file_path();
  if (!history_path) {
    return -1;
  }

  FILE *fp = fopen(history_path, "wb");
  free(history_path);

  if (!fp) {
    return -1;
  }

  if (fwrite(&count, sizeof(int), 1, fp) != 1) {
    fclose(fp);
    return -1;
  }

  if (count > 0) {
    if (fwrite(history, sizeof(HistoryEntry), count, fp) != (size_t) count) {
      fclose(fp);
      return -1;
    }
  }

  fclose(fp);
  return 0;
}

int add_to_history(long fib_number, Algorithm algorithm, OutputFormat format, double calc_time,
                   const char *result_str) {
  HistoryEntry *history = NULL;
  int count = 0;

  int load_result = load_history(&history, &count);
  if (load_result < 0) {
    return -1;
  }

  // Validate count is within reasonable bounds before allocation
  if (count < 0 || count >= MAX_HISTORY_ENTRIES) {
    free(history);
    return -1;
  }

  // Add new entry at the beginning
  HistoryEntry *new_history = malloc((count + 1) * sizeof(HistoryEntry));
  if (!new_history) {
    free(history);
    return -1;
  }

  // Create new entry
  HistoryEntry entry;
  entry.fib_number = fib_number;
  entry.algorithm = algorithm;
  entry.format = format;
  entry.calc_time = calc_time;
  entry.timestamp = time(NULL);

  // Store preview (first 64 chars)
  if (result_str) {
    size_t len = strlen(result_str);
    if (len > 64) {
      len = 64;
    }
    memcpy(entry.result_preview, result_str, len);
    entry.result_preview[len] = '\0';
  } else {
    entry.result_preview[0] = '\0';
  }

  // Insert at the beginning
  new_history[0] = entry;
  if (count > 0) {
    memcpy(&new_history[1], history, count * sizeof(HistoryEntry));
  }

  // Keep only MAX_HISTORY_ENTRIES
  int new_count = count + 1;
  if (new_count > MAX_HISTORY_ENTRIES) {
    new_count = MAX_HISTORY_ENTRIES;
  }

  int result = save_history(new_history, new_count);

  free(history);
  free(new_history);

  return result;
}

void display_history(void) {
  HistoryEntry *history = NULL;
  int count = 0;

  if (load_history(&history, &count) != 0) {
    printf("Error loading history.\n");
    return;
  }

  if (count == 0) {
    printf("No calculation history available.\n");
    return;
  }

  printf("\n=== Fibonacci Calculation History ===\n\n");

  for (int i = 0; i < count; i++) {
    char time_str[64];
    struct tm tm_info;
    localtime_r(&history[i].timestamp, &tm_info);
    strftime(time_str, sizeof(time_str), "%Y-%m-%d %H:%M:%S", &tm_info);

    printf("Entry %d:\n", i + 1);
    printf("  Date:      %s\n", time_str);
    printf("  Number:    F(%ld)\n", history[i].fib_number);
    printf("  Algorithm: %s\n", algorithm_to_string(history[i].algorithm));
    printf("  Format:    %s\n", format_to_string(history[i].format));
    printf("  Time:      %.6f seconds\n", history[i].calc_time);

    if (history[i].result_preview[0] != '\0') {
      printf("  Result:    %s", history[i].result_preview);
      if (strlen(history[i].result_preview) == 64) {
        printf("...");
      }
      printf("\n");
    }

    printf("\n");
  }

  free(history);
}

void display_help(const char *program_name) {
  printf("\nUsage: %s <limit> [options]\n", program_name);
  printf("\n");
  printf("Calculate Fibonacci numbers.\n");
  printf("\n");
  printf("Options:\n");
  printf("  -h, --help    Display this help message and exit.\n");
  printf("  -y, --history Show calculation history and exit.\n");
  printf("  -t, --time    Show calculation time.\n");
  printf("  -T, --time-only\n");
  printf("                Show only calculation time (no result output).\n");
  printf("                Useful for stress testing and benchmarking.\n");
  printf("  -r, --raw     Output only the number without prefix.\n");
  printf("  -v, --verbose Show detailed information during calculation.\n");
  printf("  -o, --output  Save the result to the specified file.\n");
  printf("  -f, --format <format>\n");
  printf("                Set output number format. Available options:\n");
  printf("                  dec   - Decimal (default)\n");
  printf("                  hex   - Hexadecimal\n");
  printf("                  bin   - Binary\n");
  printf("  -a, --algorithm <method>\n");
  printf("                Set calculation algorithm. Available options:\n");
  printf("                  iter   - Iterative\n");
  printf("                  recur  - Recursive with memoization\n");
  printf("                  matrix - Matrix exponentiation (default)\n");
  printf("\n");
  printf("Examples:\n");
  printf("  %s 100                 Calculate using default algorithm\n", program_name);
  printf("  %s -y                  Show calculation history\n", program_name);
  printf("  %s 50 -a matrix        Calculate using matrix exponentiation\n", program_name);
  printf("  %s 30 -f hex           Display result in hexadecimal\n", program_name);
  printf("  %s 20 -f bin -r        Display raw binary result\n", program_name);
  printf("  %s 30 -a recur -t      Calculate recursively and show time\n", program_name);
  printf("  %s 1000000 -T          Stress test - show only time\n", program_name);
  printf("\n");
}

char *get_formatted_result(mpz_t result, OutputFormat format, int verbose) {
  char *result_str = NULL;

  switch (format) {
    case DECIMAL:
      if (verbose) {
        fprintf(stderr, "Converting result to decimal format\n");
      }
      result_str = mpz_get_str(NULL, 10, result);
      break;

    case HEXADECIMAL:
      if (verbose) {
        fprintf(stderr, "Converting result to hexadecimal format\n");
      }
      result_str = mpz_get_str(NULL, 16, result);
      break;

    case BINARY:
      if (verbose) {
        fprintf(stderr, "Converting result to binary format\n");
      }
      result_str = mpz_get_str(NULL, 2, result);
      break;

    default:
      if (verbose) {
        fprintf(stderr, "Unknown format, defaulting to decimal\n");
      }
      result_str = mpz_get_str(NULL, 10, result);
  }

  return result_str;
}

const char *get_format_prefix(OutputFormat format) {
  switch (format) {
    case HEXADECIMAL:
      return "0x";
    case BINARY:
      return "0b";
    default:
      return "";
  }
}
