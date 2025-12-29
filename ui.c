#define _POSIX_C_SOURCE 200809L
#define _DEFAULT_SOURCE
#define _BSD_SOURCE

#include "fib.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include <ncurses.h>

#define MAX_INPUT_SIZE 256
#define MAX_NUMBER_LEN 32

static char *my_strdup(const char *str) {
  size_t len = strlen(str) + 1;
  char *new_str = malloc(len);
  if (new_str) {
    memcpy(new_str, str, len);
  }
  return new_str;
}

typedef struct {
  long fib_number;
  char algorithm[16];
  char format[16];
  int show_time;
  int time_only;
  int raw_output;
  int verbose;
  char output_file[MAX_INPUT_SIZE];
  int has_output_file;
} UIConfig;

typedef enum {
  FIELD_NUMBER,
  FIELD_ALGORITHM,
  FIELD_FORMAT,
  FIELD_SHOW_TIME,
  FIELD_TIME_ONLY,
  FIELD_RAW_OUTPUT,
  FIELD_VERBOSE,
  FIELD_OUTPUT_FILE,
  FIELD_CONFIRM,
  FIELD_COUNT
} FieldType;

static void init_colors(void) {
  if (has_colors()) {
    start_color();
    use_default_colors();
  }
}

static void draw_header(WINDOW *win) {
  int max_y, max_x;
  getmaxyx(win, max_y, max_x);
  (void) max_y;  // unused

  const char *title = "FIB.C";
  int title_len = strlen(title);
  int padding = (max_x - 4 - title_len) / 2;

  wattron(win, A_BOLD);

  // Title centered
  mvwprintw(win, 1, 2 + padding, "%s", title);

  wattroff(win, A_BOLD);
}

static void draw_footer(WINDOW *win, int max_y) {
  wattron(win, A_DIM);
  mvwprintw(win, max_y - 2, 2,
            "Navigation: UP/DOWN or TAB | Edit: ENTER | Toggle: SPACE | Confirm: F");
  mvwprintw(win, max_y - 1, 2, "Quit: Q or ESC");
  wattroff(win, A_DIM);
}

static void draw_field(WINDOW *win, int y, int x, const char *label, const char *value,
                       int is_selected, int max_width) {
  if (is_selected) {
    wattron(win, A_BOLD);
    mvwprintw(win, y, x, "> ");
  } else {
    mvwprintw(win, y, x, "  ");
  }

  wprintw(win, "%-25s: ", label);

  if (is_selected) {
    wattron(win, A_REVERSE);
  }

  wprintw(win, "%-*s", max_width, value);

  if (is_selected) {
    wattroff(win, A_REVERSE);
  }
  wattroff(win, A_BOLD);
}

static void draw_toggle_field(WINDOW *win, int y, int x, const char *label, int value,
                              int is_selected) {
  if (is_selected) {
    wattron(win, A_BOLD);
    mvwprintw(win, y, x, "> ");
  } else {
    mvwprintw(win, y, x, "  ");
  }

  wprintw(win, "%-25s: ", label);

  if (is_selected) {
    wattron(win, A_REVERSE);
  } else if (value) {
    wattron(win, A_BOLD);
  }

  wprintw(win, "[%c] %s", value ? 'X' : ' ', value ? "Yes" : "No");

  if (is_selected) {
    wattroff(win, A_REVERSE);
  } else if (value) {
    wattroff(win, A_BOLD);
  }
  wattroff(win, A_BOLD);
}

static void draw_ui(WINDOW *win, UIConfig *config, int selected_field) {
  int max_y, max_x;
  getmaxyx(win, max_y, max_x);

  wclear(win);
  box(win, 0, 0);

  draw_header(win);

  int y = 4;
  char number_str[MAX_NUMBER_LEN];
  snprintf(number_str, MAX_NUMBER_LEN, "%ld", config->fib_number);
  draw_field(win, y++, 4, "Fibonacci Number", number_str, selected_field == FIELD_NUMBER, 15);

  y++;
  draw_field(win, y++, 4, "Algorithm", config->algorithm, selected_field == FIELD_ALGORITHM, 15);

  draw_field(win, y++, 4, "Output Format", config->format, selected_field == FIELD_FORMAT, 15);

  y++;
  draw_toggle_field(win, y++, 4, "Show Calculation Time", config->show_time,
                    selected_field == FIELD_SHOW_TIME);

  if (config->show_time) {
    draw_toggle_field(win, y++, 4, "  Time Only (no result)", config->time_only,
                      selected_field == FIELD_TIME_ONLY);
  } else {
    y++;
  }

  y++;
  draw_toggle_field(win, y++, 4, "Raw Output", config->raw_output,
                    selected_field == FIELD_RAW_OUTPUT);

  draw_toggle_field(win, y++, 4, "Verbose Mode", config->verbose, selected_field == FIELD_VERBOSE);

  y++;
  const char *file_display = config->has_output_file ? config->output_file : "(none)";
  draw_field(win, y++, 4, "Output File", file_display, selected_field == FIELD_OUTPUT_FILE, 30);

  y += 2;

  if (selected_field == FIELD_CONFIRM) {
    wattron(win, A_BOLD | A_REVERSE);
    mvwprintw(win, y, max_x / 2 - 8, "  [ GENERATE ]  ");
    wattroff(win, A_BOLD | A_REVERSE);
  } else {
    wattron(win, A_BOLD);
    mvwprintw(win, y, max_x / 2 - 8, "  [ GENERATE ]  ");
    wattroff(win, A_BOLD);
  }

  draw_footer(win, max_y);

  wrefresh(win);
}

static int edit_number(WINDOW *win, long *value, long min_val, long max_val) {
  char buffer[MAX_NUMBER_LEN];
  int max_y, max_x;
  getmaxyx(win, max_y, max_x);

  WINDOW *input_win = newwin(5, 50, max_y / 2 - 2, max_x / 2 - 25);
  wclear(input_win);
  box(input_win, 0, 0);

  wattron(input_win, A_BOLD);
  mvwprintw(input_win, 1, 2, "Enter number (%ld - %ld):", min_val, max_val);
  wattroff(input_win, A_BOLD);

  mvwprintw(input_win, 3, 2, "ESC to cancel");

  curs_set(1);
  noecho();

  mvwprintw(input_win, 2, 2, "> ");
  wrefresh(input_win);

  int ch, pos = 0;
  buffer[0] = '\0';

  while ((ch = wgetch(input_win)) != '\n' && ch != 27) {
    if (ch == KEY_BACKSPACE || ch == 127 || ch == 8) {
      if (pos > 0) {
        pos--;
        buffer[pos] = '\0';
        mvwprintw(input_win, 2, 2, ">                                          ");
        mvwprintw(input_win, 2, 2, "> %s", buffer);
        wrefresh(input_win);
      }
    } else if (isdigit(ch) && pos < MAX_NUMBER_LEN - 1) {
      buffer[pos++] = ch;
      buffer[pos] = '\0';
      mvwprintw(input_win, 2, 4 + pos - 1, "%c", buffer[pos - 1]);
      wrefresh(input_win);
    }
  }

  noecho();
  curs_set(0);
  delwin(input_win);
  touchwin(win);
  wrefresh(win);

  if (ch == 27 || buffer[0] == '\0') {
    return 0;
  }

  char *endptr;
  long num = strtol(buffer, &endptr, 10);

  if (*endptr != '\0' || num < min_val || num > max_val) {
    return 0;
  }

  *value = num;
  return 1;
}

static int edit_string(WINDOW *win, char *buffer, int max_len, const char *prompt) {
  int max_y, max_x;
  getmaxyx(win, max_y, max_x);

  WINDOW *input_win = newwin(5, 60, max_y / 2 - 2, max_x / 2 - 30);
  wclear(input_win);
  box(input_win, 0, 0);

  wattron(input_win, A_BOLD);
  mvwprintw(input_win, 1, 2, "%s", prompt);
  wattroff(input_win, A_BOLD);

  mvwprintw(input_win, 3, 2, "ESC to cancel | ENTER when done");

  curs_set(1);
  noecho();

  mvwprintw(input_win, 2, 2, "> ");
  wrefresh(input_win);

  char temp[MAX_INPUT_SIZE] = "";
  int ch, pos = 0;

  while ((ch = wgetch(input_win)) != '\n' && ch != 27) {
    if (ch == KEY_BACKSPACE || ch == 127 || ch == 8) {
      if (pos > 0) {
        pos--;
        temp[pos] = '\0';
        mvwprintw(input_win, 2, 2, ">                                                    ");
        mvwprintw(input_win, 2, 2, "> %s", temp);
        wrefresh(input_win);
      }
    } else if (pos < max_len - 1 && ch >= 32 && ch < 127) {
      temp[pos++] = ch;
      temp[pos] = '\0';
      mvwprintw(input_win, 2, 4 + pos - 1, "%c", temp[pos - 1]);
      wrefresh(input_win);
    }
  }

  noecho();
  curs_set(0);
  delwin(input_win);
  touchwin(win);
  wrefresh(win);

  if (ch == 27) {
    return 0;
  }

  if (temp[0] != '\0') {
    strncpy(buffer, temp, max_len - 1);
    buffer[max_len - 1] = '\0';
    return 1;
  }

  return 0;
}

static void cycle_algorithm(char *algorithm) {
  if (strcmp(algorithm, "iter") == 0) {
    strcpy(algorithm, "recur");
  } else if (strcmp(algorithm, "recur") == 0) {
    strcpy(algorithm, "matrix");
  } else {
    strcpy(algorithm, "iter");
  }
}

static void cycle_format(char *format) {
  if (strcmp(format, "dec") == 0) {
    strcpy(format, "hex");
  } else if (strcmp(format, "hex") == 0) {
    strcpy(format, "bin");
  } else {
    strcpy(format, "dec");
  }
}

/**
 * Runs the interactive Terminal User Interface (TUI) for the Fibonacci calculator.
 *
 * This function provides a modern, ncurses-based interface with:
 * - Real-time visual feedback
 * - Keyboard navigation (arrows, tab, space, enter)
 * - Color-coded display elements
 * - Interactive field editing
 *
 * argc Pointer to the argument count, which will be updated
 * argv Pointer to the argument vector, which will be replaced with newly generated arguments
 */
void run_user_interface(int *argc, char ***argv) {
  UIConfig config = {.fib_number = 10,
                     .show_time = 0,
                     .time_only = 0,
                     .raw_output = 0,
                     .verbose = 0,
                     .has_output_file = 0};
  strcpy(config.algorithm, "iter");
  strcpy(config.format, "dec");
  config.output_file[0] = '\0';

  initscr();
  clear();
  refresh();
  cbreak();
  noecho();
  curs_set(0);
  keypad(stdscr, TRUE);

  init_colors();

  int max_y, max_x;
  getmaxyx(stdscr, max_y, max_x);

  if (max_y < 24 || max_x < 76) {
    endwin();
    fprintf(stderr, "Error: Terminal too small. Need at least 76x24, have %dx%d\n", max_x, max_y);
    exit(EXIT_FAILURE);
  }

  WINDOW *main_win = newwin(max_y - 2, max_x - 4, 1, 2);
  keypad(main_win, TRUE);

  int selected_field = FIELD_NUMBER;
  int running = 1;

  while (running) {
    draw_ui(main_win, &config, selected_field);

    int ch = wgetch(main_win);

    // Handle terminal resize
    if (ch == KEY_RESIZE) {
      getmaxyx(stdscr, max_y, max_x);

      if (max_y < 24 || max_x < 76) {
        // Terminal became too small
        delwin(main_win);
        endwin();
        fprintf(stderr, "Error: Terminal too small. Need at least 76x24, have %dx%d\n", max_x,
                max_y);
        exit(EXIT_FAILURE);
      }

      // Recreate window with new dimensions
      delwin(main_win);
      clear();
      refresh();
      main_win = newwin(max_y - 2, max_x - 4, 1, 2);
      keypad(main_win, TRUE);
      continue;
    }

    switch (ch) {
      case 'q':
      case 'Q':
      case 27:  // ESC
        delwin(main_win);
        endwin();
        exit(0);
        break;

      case KEY_UP:
      case 'k':
        selected_field = (selected_field - 1 + FIELD_COUNT) % FIELD_COUNT;
        if (!config.show_time && selected_field == FIELD_TIME_ONLY) {
          selected_field = (selected_field - 1 + FIELD_COUNT) % FIELD_COUNT;
        }
        break;

      case KEY_DOWN:
      case 'j':
      case 9:  // TAB
        selected_field = (selected_field + 1) % FIELD_COUNT;
        if (!config.show_time && selected_field == FIELD_TIME_ONLY) {
          selected_field = (selected_field + 1) % FIELD_COUNT;
        }
        break;

      case ' ':  // SPACE - toggle boolean fields or cycle options
        switch (selected_field) {
          case FIELD_ALGORITHM:
            cycle_algorithm(config.algorithm);
            break;
          case FIELD_FORMAT:
            cycle_format(config.format);
            break;
          case FIELD_SHOW_TIME:
            config.show_time = !config.show_time;
            if (!config.show_time) {
              config.time_only = 0;
            }
            break;
          case FIELD_TIME_ONLY:
            if (config.show_time) {
              config.time_only = !config.time_only;
            }
            break;
          case FIELD_RAW_OUTPUT:
            config.raw_output = !config.raw_output;
            break;
          case FIELD_VERBOSE:
            config.verbose = !config.verbose;
            break;
          case FIELD_OUTPUT_FILE:
            config.has_output_file = !config.has_output_file;
            if (!config.has_output_file) {
              config.output_file[0] = '\0';
            }
            break;
        }
        break;

      case '\n':
      case KEY_ENTER:
        switch (selected_field) {
          case FIELD_NUMBER:
            edit_number(main_win, &config.fib_number, 0, 1000000);
            break;
          case FIELD_ALGORITHM:
            cycle_algorithm(config.algorithm);
            break;
          case FIELD_FORMAT:
            cycle_format(config.format);
            break;
          case FIELD_OUTPUT_FILE:
            if (config.has_output_file || config.output_file[0] == '\0') {
              if (edit_string(main_win, config.output_file, MAX_INPUT_SIZE,
                              "Enter output filename:")) {
                config.has_output_file = 1;
              }
            } else {
              config.has_output_file = 0;
              config.output_file[0] = '\0';
            }
            break;
          case FIELD_CONFIRM:
            running = 0;
            break;
        }
        break;

      case 'f':
      case 'F':
        running = 0;
        break;
    }
  }

  delwin(main_win);
  endwin();

  // Build command-line arguments from config
  int new_argc = 2;  // program name + number
  if (strcmp(config.algorithm, "iter") != 0)
    new_argc += 2;
  if (strcmp(config.format, "dec") != 0)
    new_argc += 2;
  if (config.show_time)
    new_argc += 1;
  if (config.time_only)
    new_argc += 1;
  if (config.raw_output)
    new_argc += 1;
  if (config.verbose)
    new_argc += 1;
  if (config.has_output_file && config.output_file[0] != '\0')
    new_argc += 2;

  char **new_argv = (char **) malloc(new_argc * sizeof(char *));
  if (!new_argv) {
    fprintf(stderr, "Error: Memory allocation failed\n");
    exit(EXIT_FAILURE);
  }

  new_argv[0] = my_strdup((*argv)[0]);

  char number_str[MAX_NUMBER_LEN];
  snprintf(number_str, MAX_NUMBER_LEN, "%ld", config.fib_number);
  new_argv[1] = my_strdup(number_str);

  int arg_index = 2;

  if (strcmp(config.algorithm, "iter") != 0) {
    new_argv[arg_index++] = my_strdup("-a");
    new_argv[arg_index++] = my_strdup(config.algorithm);
  }

  if (strcmp(config.format, "dec") != 0) {
    new_argv[arg_index++] = my_strdup("-f");
    new_argv[arg_index++] = my_strdup(config.format);
  }

  if (config.show_time) {
    new_argv[arg_index++] = my_strdup("-t");
  }

  if (config.time_only) {
    new_argv[arg_index++] = my_strdup("-T");
  }

  if (config.raw_output) {
    new_argv[arg_index++] = my_strdup("-r");
  }

  if (config.verbose) {
    new_argv[arg_index++] = my_strdup("-v");
  }

  if (config.has_output_file && config.output_file[0] != '\0') {
    new_argv[arg_index++] = my_strdup("-o");
    new_argv[arg_index++] = my_strdup(config.output_file);
  }

  *argc = new_argc;
  *argv = new_argv;
}

void free_generated_args(int argc, char **argv) {
  for (int i = 0; i < argc; i++) {
    free(argv[i]);
  }
  free(argv);
}
