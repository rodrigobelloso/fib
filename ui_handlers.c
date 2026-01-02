#include "ui_handlers.h"
#include "ui_input.h"
#include "fib.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>

void cycle_algorithm(char *algorithm) {
  if (strcmp(algorithm, "matrix") == 0) {
    strcpy(algorithm, "iter");
  } else if (strcmp(algorithm, "iter") == 0) {
    strcpy(algorithm, "recur");
  } else {
    strcpy(algorithm, "matrix");
  }
}

void cycle_format(char *format) {
  if (strcmp(format, "dec") == 0) {
    strcpy(format, "hex");
  } else if (strcmp(format, "hex") == 0) {
    strcpy(format, "bin");
  } else {
    strcpy(format, "dec");
  }
}

void calculate_result(UIConfig *config) {
  // Free previous result if exists
  if (config->result_string) {
    free(config->result_string);
    config->result_string = NULL;
  }

  mpz_t result;
  mpz_init(result);

  // Time the calculation
  struct timespec start, end;
  clock_gettime(CLOCK_MONOTONIC, &start);

  // Select algorithm
  if (strcmp(config->algorithm, "iter") == 0) {
    calculate_fibonacci_iterative(result, config->fib_number, 0);
  } else if (strcmp(config->algorithm, "recur") == 0) {
    calculate_fibonacci_recursive(result, config->fib_number, NULL, 0);
  } else if (strcmp(config->algorithm, "matrix") == 0) {
    calculate_fibonacci_matrix(result, config->fib_number, 0);
  }

  clock_gettime(CLOCK_MONOTONIC, &end);
  config->calc_time = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;

  // Format the result
  OutputFormat fmt = DECIMAL;
  if (strcmp(config->format, "hex") == 0) {
    fmt = HEXADECIMAL;
  } else if (strcmp(config->format, "bin") == 0) {
    fmt = BINARY;
  }

  char *raw_result = get_formatted_result(result, fmt, 0);

  // Build the final result string based on raw_output setting
  if (config->raw_output) {
    // Raw output: just the number
    config->result_string = raw_result;
  } else {
    // Formatted output: add label and prefix
    const char *format_name;
    switch (fmt) {
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

    const char *prefix = get_format_prefix(fmt);
    size_t label_len = snprintf(NULL, 0, "Fibonacci Number %ld (%s): %s%s", config->fib_number,
                                format_name, prefix, raw_result);
    config->result_string = malloc(label_len + 1);
    if (config->result_string) {
      snprintf(config->result_string, label_len + 1, "Fibonacci Number %ld (%s): %s%s",
               config->fib_number, format_name, prefix, raw_result);
      free(raw_result);
    } else {
      config->result_string = raw_result;  // Fallback if malloc fails
    }
  }

  config->has_result = 1;

  // Add to history
  Algorithm algo = MATRIX;
  if (strcmp(config->algorithm, "iter") == 0) {
    algo = ITERATIVE;
  } else if (strcmp(config->algorithm, "recur") == 0) {
    algo = RECURSIVE;
  }

  add_to_history(config->fib_number, algo, fmt, config->calc_time, raw_result);

  // Write to file if requested
  if (config->has_output_file && config->output_file[0] != '\0') {
    // Create file with restrictive permissions (0600 = rw-------)
    // This prevents world-writable files and potential security vulnerabilities
    int fd = open(config->output_file, O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
    if (fd != -1) {
      FILE *fp = fdopen(fd, "w");
      if (fp) {
        fprintf(fp, "%s\n", config->result_string);
        fclose(fp);  // This also closes the underlying fd
      } else {
        close(fd);
      }
    }
  }

  mpz_clear(result);
}

void handle_history_up(int *history_selected, int *history_scroll) {
  HistoryEntry *history = NULL;
  int count = 0;
  if (load_history(&history, &count) == 0 && count > 0) {
    if (*history_selected > 0) {
      (*history_selected)--;
      if (*history_selected < *history_scroll) {
        *history_scroll = *history_selected;
      }
    }
    free(history);
  }
}

void handle_history_down(int *history_selected, int *history_scroll, int max_y) {
  HistoryEntry *history = NULL;
  int count = 0;
  if (load_history(&history, &count) == 0 && count > 0) {
    if (*history_selected < count - 1) {
      (*history_selected)++;
      int max_items = max_y - 11;
      if (*history_selected >= *history_scroll + max_items) {
        *history_scroll = *history_selected - max_items + 1;
      }
    }
    free(history);
  }
}

void handle_history_delete(int *history_selected, int *history_scroll) {
  HistoryEntry *history = NULL;
  int count = 0;
  if (load_history(&history, &count) == 0 && count > 0) {
    HistoryEntry *new_history = malloc((count - 1) * sizeof(HistoryEntry));
    if (new_history) {
      int new_idx = 0;
      for (int i = 0; i < count; i++) {
        if (i != *history_selected) {
          new_history[new_idx++] = history[i];
        }
      }
      save_history(new_history, count - 1);
      free(new_history);

      if (*history_selected >= count - 1) {
        *history_selected = count - 2;
      }
      if (*history_selected < 0) {
        *history_selected = 0;
      }
      if (*history_selected < *history_scroll) {
        *history_scroll = *history_selected;
      }
    }
    free(history);
  }
}

void handle_space_key(UIConfig *config, int selected_field) {
  switch (selected_field) {
    case FIELD_ALGORITHM:
      cycle_algorithm(config->algorithm);
      break;
    case FIELD_FORMAT:
      cycle_format(config->format);
      break;
    case FIELD_SHOW_TIME:
      config->show_time = !config->show_time;
      if (!config->show_time) {
        config->time_only = 0;
      }
      break;
    case FIELD_TIME_ONLY:
      if (config->show_time) {
        config->time_only = !config->time_only;
      }
      break;
    case FIELD_RAW_OUTPUT:
      config->raw_output = !config->raw_output;
      break;
    case FIELD_OUTPUT_FILE:
      config->has_output_file = !config->has_output_file;
      if (!config->has_output_file) {
        config->output_file[0] = '\0';
      }
      break;
  }
}

void handle_enter_key(WINDOW *main_win, UIConfig *config, int selected_field) {
  switch (selected_field) {
    case FIELD_NUMBER:
      edit_number(main_win, &config->fib_number, 0, 1000000);
      break;
    case FIELD_ALGORITHM:
      cycle_algorithm(config->algorithm);
      break;
    case FIELD_FORMAT:
      cycle_format(config->format);
      break;
    case FIELD_OUTPUT_FILE:
      if (config->has_output_file || config->output_file[0] == '\0') {
        if (edit_string(main_win, config->output_file, MAX_INPUT_SIZE, "Enter output filename:")) {
          config->has_output_file = 1;
        }
      } else {
        config->has_output_file = 0;
        config->output_file[0] = '\0';
      }
      break;
    case FIELD_CONFIRM:
      calculate_result(config);
      break;
  }
}
