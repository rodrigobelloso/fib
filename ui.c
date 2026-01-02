#define _POSIX_C_SOURCE 200809L
#ifndef _DEFAULT_SOURCE
#define _DEFAULT_SOURCE
#endif
#define _BSD_SOURCE

#include "fib.h"
#include "ui_types.h"
#include "ui_theme.h"
#include "ui_draw.h"
#include "ui_input.h"
#include "ui_handlers.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ncurses.h>

static char *my_strdup(const char *str) {
  size_t len = strlen(str) + 1;
  char *new_str = malloc(len);
  if (new_str) {
    memcpy(new_str, str, len);
  }
  return new_str;
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
  UIConfig config = {.fib_number = '\0',
                     .show_time = 0,
                     .time_only = 0,
                     .raw_output = 0,
                     .has_output_file = 0,
                     .result_string = NULL,
                     .has_result = 0,
                     .calc_time = 0.0};
  strcpy(config.algorithm, "matrix");
  strcpy(config.format, "dec");
  config.output_file[0] = '\0';

  initscr();
  clear();
  refresh();
  cbreak();
  noecho();
  curs_set(0);
  keypad(stdscr, TRUE);

  init_colors(current_theme);

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
  ViewMode current_view = VIEW_MAIN;
  int history_selected = 0;
  int history_scroll = 0;
  int running = 1;

  while (running) {
    if (current_view == VIEW_MAIN) {
      draw_ui(main_win, &config, selected_field);
    } else if (current_view == VIEW_HISTORY) {
      draw_history_view(main_win, history_selected, history_scroll);
    } else if (current_view == VIEW_PERFORMANCE) {
      draw_performance_graph(main_win, 0);
    }

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
        if (current_view == VIEW_HISTORY) {
          free(config.result_string);
          delwin(main_win);
          endwin();
          exit(0);
        } else {
          free(config.result_string);
          delwin(main_win);
          endwin();
          exit(0);
        }
        break;

      case 27:  // ESC
        if (current_view == VIEW_HISTORY) {
          current_view = VIEW_MAIN;
          history_selected = 0;
          history_scroll = 0;
        } else if (current_view == VIEW_PERFORMANCE) {
          current_view = VIEW_MAIN;
        } else {
          free(config.result_string);
          delwin(main_win);
          endwin();
          exit(0);
        }
        break;

      case 'h':
      case 'H':
        if (current_view == VIEW_MAIN) {
          current_view = VIEW_HISTORY;
          history_selected = 0;
          history_scroll = 0;
        } else {
          current_view = VIEW_MAIN;
        }
        break;

      case 'p':
      case 'P':
        if (current_view == VIEW_MAIN) {
          current_view = VIEW_PERFORMANCE;
        } else if (current_view == VIEW_PERFORMANCE) {
          current_view = VIEW_MAIN;
        }
        break;

      case KEY_UP:
      case 'k':
        if (current_view == VIEW_HISTORY) {
          handle_history_up(&history_selected, &history_scroll);
        } else {
          selected_field = (selected_field - 1 + FIELD_COUNT) % FIELD_COUNT;
          if (!config.show_time && selected_field == FIELD_TIME_ONLY) {
            selected_field = (selected_field - 1 + FIELD_COUNT) % FIELD_COUNT;
          }
        }
        break;

      case KEY_DOWN:
      case 'j':
        if (current_view == VIEW_HISTORY) {
          handle_history_down(&history_selected, &history_scroll, max_y);
        } else {
          selected_field = (selected_field + 1) % FIELD_COUNT;
          if (!config.show_time && selected_field == FIELD_TIME_ONLY) {
            selected_field = (selected_field + 1) % FIELD_COUNT;
          }
        }
        break;

      case 9:  // TAB (only in main view)
        if (current_view == VIEW_MAIN) {
          selected_field = (selected_field + 1) % FIELD_COUNT;
          if (!config.show_time && selected_field == FIELD_TIME_ONLY) {
            selected_field = (selected_field + 1) % FIELD_COUNT;
          }
        }
        break;

      case 'd':
      case 'D':
        if (current_view == VIEW_HISTORY) {
          handle_history_delete(&history_selected, &history_scroll);
        }
        break;

      case ' ':  // SPACE - toggle boolean fields or cycle options
        if (current_view == VIEW_MAIN) {
          handle_space_key(&config, selected_field);
        }
        break;

      case '\n':  // Line feed (Enter key - ASCII 10)
      case 13:    // Carriage return
        if (current_view == VIEW_MAIN) {
          handle_enter_key(main_win, &config, selected_field);
        }
        break;

      case 'f':
      case 'F':
        if (current_view == VIEW_MAIN) {
          calculate_result(&config);
        }
        break;

      case 't':
      case 'T':
        cycle_theme();
        break;
    }
  }

  delwin(main_win);
  endwin();

  // Build command-line arguments from config
  int new_argc = 2;  // program name + number
  if (strcmp(config.algorithm, "matrix") != 0)
    new_argc += 2;
  if (strcmp(config.format, "dec") != 0)
    new_argc += 2;
  if (config.show_time)
    new_argc += 1;
  if (config.time_only)
    new_argc += 1;
  if (config.raw_output)
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

  if (strcmp(config.algorithm, "matrix") != 0) {
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
