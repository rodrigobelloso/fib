#include "ui_draw.h"
#include "ui_theme.h"
#include "fib.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

void draw_header(WINDOW *win) {
  int max_y, max_x;
  getmaxyx(win, max_y, max_x);
  (void) max_y;  // unused

#ifdef VERSION
  char title[64];
  snprintf(title, sizeof(title), "FIB %s (%s)", VERSION, BUILD_ID);
#else
  const char *title = "FIB";
#endif
  int title_len = strlen(title);
  int padding = (max_x - 4 - title_len) / 2;

  wattron(win, A_BOLD | COLOR_PAIR(COLOR_PAIR_HEADER));

  // Title centered
  mvwprintw(win, 1, 2 + padding, "%s", title);

  wattroff(win, A_BOLD | COLOR_PAIR(COLOR_PAIR_HEADER));

  // Show current theme
  wattron(win, COLOR_PAIR(COLOR_PAIR_ACCENT));
  mvwprintw(win, 1, max_x - 20, "Theme: %s", get_theme_name(current_theme));
  wattroff(win, COLOR_PAIR(COLOR_PAIR_ACCENT));
}

void draw_footer(WINDOW *win, int max_y) {
  wattron(win, COLOR_PAIR(COLOR_PAIR_DIM));
  mvwprintw(win, max_y - 2, 2,
            "Navigation: UP/DOWN or TAB | Edit: ENTER | Toggle: SPACE | Calculate: F");
  mvwprintw(win, max_y - 1, 2, "Quit: Q or ESC | History: H | Performance: P | Theme: T");
  wattroff(win, COLOR_PAIR(COLOR_PAIR_DIM));
}

void draw_history_footer(WINDOW *win, int max_y) {
  wattron(win, COLOR_PAIR(COLOR_PAIR_DIM));
  mvwprintw(win, max_y - 2, 2, "Navigation: UP/DOWN | Delete: D");
  mvwprintw(win, max_y - 1, 2, "Back to Main: H or ESC | Quit: Q | Theme: T");
  wattroff(win, COLOR_PAIR(COLOR_PAIR_DIM));
}

void draw_performance_footer(WINDOW *win, int max_y) {
  wattron(win, COLOR_PAIR(COLOR_PAIR_DIM));
  mvwprintw(win, max_y - 2, 2, "Navigation: UP/DOWN to scroll");
  mvwprintw(win, max_y - 1, 2, "Back to Main: P or ESC | Quit: Q | Theme: T");
  wattroff(win, COLOR_PAIR(COLOR_PAIR_DIM));
}

void draw_performance_graph(WINDOW *win, int scroll_offset) {
  int max_y, max_x;
  getmaxyx(win, max_y, max_x);

  wclear(win);
  box(win, 0, 0);

  draw_header(win);

  wattron(win, A_BOLD | COLOR_PAIR(COLOR_PAIR_RESULT));
  mvwprintw(win, 3, 4, "Performance Graph - Calculation Time vs Fibonacci Number");
  wattroff(win, A_BOLD | COLOR_PAIR(COLOR_PAIR_RESULT));

  HistoryEntry *history = NULL;
  int count = 0;

  if (load_history(&history, &count) != 0 || count == 0) {
    mvwprintw(win, 5, 4, "No calculation history available.");
    draw_performance_footer(win, max_y);
    wrefresh(win);
    free(history);
    return;
  }

  // Find min and max values for scaling
  double max_time = 0.0;
  long max_number = 0;
  for (int i = 0; i < count; i++) {
    if (history[i].calc_time > max_time) {
      max_time = history[i].calc_time;
    }
    if (history[i].fib_number > max_number) {
      max_number = history[i].fib_number;
    }
  }

  // Graph dimensions
  int graph_height = max_y - 13;
  int graph_width = max_x - 20;
  int graph_x = 12;
  int graph_y = 6;

  if (graph_height < 5 || graph_width < 20) {
    mvwprintw(win, 5, 4, "Terminal too small to display graph.");
    draw_performance_footer(win, max_y);
    wrefresh(win);
    free(history);
    return;
  }

  // Draw axes
  wattron(win, COLOR_PAIR(COLOR_PAIR_ACCENT));
  // Y-axis
  for (int i = 0; i < graph_height; i++) {
    mvwprintw(win, graph_y + i, graph_x - 1, "|");
  }
  // X-axis
  for (int i = 0; i < graph_width; i++) {
    mvwprintw(win, graph_y + graph_height, graph_x + i, "-");
  }
  mvwprintw(win, graph_y + graph_height, graph_x - 1, "+");
  wattroff(win, COLOR_PAIR(COLOR_PAIR_ACCENT));

  // Y-axis label
  wattron(win, A_BOLD | COLOR_PAIR(COLOR_PAIR_HEADER));
  mvwprintw(win, graph_y - 1, 4, "Time (s)");
  wattroff(win, A_BOLD | COLOR_PAIR(COLOR_PAIR_HEADER));

  // Y-axis scale
  wattron(win, COLOR_PAIR(COLOR_PAIR_DIM));
  for (int i = 0; i <= 4; i++) {
    double value = max_time * (4 - i) / 4.0;
    int y_pos = graph_y + (graph_height - 1) * i / 4;
    mvwprintw(win, y_pos, 2, "%.3f", value);
  }
  wattroff(win, COLOR_PAIR(COLOR_PAIR_DIM));

  // X-axis label
  wattron(win, A_BOLD | COLOR_PAIR(COLOR_PAIR_HEADER));
  mvwprintw(win, graph_y + graph_height + 1, graph_x + graph_width / 2 - 4, "Fib(n)");
  wattroff(win, A_BOLD | COLOR_PAIR(COLOR_PAIR_HEADER));

  // Plot points
  for (int i = 0; i < count; i++) {
    // Scale x and y to fit graph
    int x_pos = (int) ((double) history[i].fib_number / max_number * (graph_width - 1));
    int y_pos = graph_height - 1 - (int) (history[i].calc_time / max_time * (graph_height - 1));

    if (x_pos >= 0 && x_pos < graph_width && y_pos >= 0 && y_pos < graph_height) {
      // Choose character and color based on algorithm
      char marker;
      int color_attr;
      switch (history[i].algorithm) {
        case MATRIX:
          marker = 'M';
          color_attr = COLOR_PAIR(COLOR_PAIR_SUCCESS) | A_BOLD;
          break;
        case ITERATIVE:
          marker = 'I';
          color_attr = COLOR_PAIR(COLOR_PAIR_GRAPH);
          break;
        case RECURSIVE:
          marker = 'R';
          color_attr = COLOR_PAIR(COLOR_PAIR_ERROR);
          break;
        default:
          marker = '*';
          color_attr = COLOR_PAIR(COLOR_PAIR_DEFAULT);
      }

      wattron(win, color_attr);
      mvwprintw(win, graph_y + y_pos, graph_x + x_pos, "%c", marker);
      wattroff(win, color_attr);
    }
  }

  // Legend
  int legend_y = max_y - 6;
  wattron(win, A_BOLD | COLOR_PAIR(COLOR_PAIR_HEADER));
  mvwprintw(win, legend_y, 4, "Legend:");
  wattroff(win, A_BOLD | COLOR_PAIR(COLOR_PAIR_HEADER));

  wattron(win, COLOR_PAIR(COLOR_PAIR_SUCCESS) | A_BOLD);
  mvwprintw(win, legend_y + 1, 6, "M");
  wattroff(win, COLOR_PAIR(COLOR_PAIR_SUCCESS) | A_BOLD);
  mvwprintw(win, legend_y + 1, 8, "- Matrix algorithm");

  wattron(win, COLOR_PAIR(COLOR_PAIR_GRAPH));
  mvwprintw(win, legend_y + 1, 32, "I");
  wattroff(win, COLOR_PAIR(COLOR_PAIR_GRAPH));
  mvwprintw(win, legend_y + 1, 34, "- Iterative algorithm");

  wattron(win, COLOR_PAIR(COLOR_PAIR_ERROR));
  mvwprintw(win, legend_y + 1, 60, "R");
  wattroff(win, COLOR_PAIR(COLOR_PAIR_ERROR));
  mvwprintw(win, legend_y + 1, 62, "- Recursive algorithm");

  // Statistics
  wattron(win, COLOR_PAIR(COLOR_PAIR_DIM));
  mvwprintw(win, legend_y - 2, 4, "Total calculations: %d | Max time: %.6fs | Max Fib(n): %ld",
            count, max_time, max_number);
  wattroff(win, COLOR_PAIR(COLOR_PAIR_DIM));

  draw_performance_footer(win, max_y);
  wrefresh(win);

  free(history);
}

void draw_history_view(WINDOW *win, int selected_index, int scroll_offset) {
  int max_y, max_x;
  getmaxyx(win, max_y, max_x);

  wclear(win);
  box(win, 0, 0);

  draw_header(win);

  wattron(win, A_BOLD | COLOR_PAIR(COLOR_PAIR_RESULT));
  mvwprintw(win, 3, 4, "Calculation History");
  wattroff(win, A_BOLD | COLOR_PAIR(COLOR_PAIR_RESULT));

  HistoryEntry *history = NULL;
  int count = 0;

  if (load_history(&history, &count) != 0 || count == 0) {
    mvwprintw(win, 5, 4, "No calculation history available.");
    draw_history_footer(win, max_y);
    wrefresh(win);
    free(history);
    return;
  }

  int y = 5;
  int max_items = max_y - 9;  // Space for header and footer
  int end_index = scroll_offset + max_items;
  if (end_index > count) {
    end_index = count;
  }

  for (int i = scroll_offset; i < end_index && y < max_y - 4; i++) {
    int is_selected = (i == selected_index);

    if (is_selected) {
      wattron(win, A_REVERSE);
    }

    // Format timestamp
    char time_str[32];
    struct tm tm_info;
    localtime_r(&history[i].timestamp, &tm_info);
    strftime(time_str, sizeof(time_str), "%m/%d %H:%M", &tm_info);

    // Display entry
    mvwprintw(win, y, 4, "%s | F(%ld) | %s | %s | %.4fs", time_str, history[i].fib_number,
              algorithm_to_string(history[i].algorithm), format_to_string(history[i].format),
              history[i].calc_time);

    // Display result preview on next line if selected
    if (is_selected && history[i].result_preview[0] != '\0') {
      mvwprintw(win, y + 1, 6, "Result: %s%s", history[i].result_preview,
                strlen(history[i].result_preview) == 64 ? "..." : "");
      y++;
    }

    if (is_selected) {
      wattroff(win, A_REVERSE);
    }

    y++;
  }

  // Show scroll indicators
  if (scroll_offset > 0) {
    wattron(win, A_DIM);
    mvwprintw(win, 4, max_x - 10, "^ More ^");
    wattroff(win, A_DIM);
  }

  if (end_index < count) {
    wattron(win, A_DIM);
    mvwprintw(win, max_y - 3, max_x - 10, "v More v");
    wattroff(win, A_DIM);
  }

  // Show count
  wattron(win, A_DIM);
  mvwprintw(win, max_y - 3, 4, "Total: %d entries", count);
  wattroff(win, A_DIM);

  draw_history_footer(win, max_y);
  wrefresh(win);

  free(history);
}

void draw_field(WINDOW *win, int y, int x, const char *label, const char *value, int is_selected,
                int max_width) {
  if (is_selected) {
    wattron(win, A_BOLD | COLOR_PAIR(COLOR_PAIR_ACCENT));
    mvwprintw(win, y, x, "> ");
  } else {
    mvwprintw(win, y, x, "  ");
  }

  wprintw(win, "%-25s: ", label);

  if (is_selected) {
    wattron(win, COLOR_PAIR(COLOR_PAIR_SELECTED));
  }

  wprintw(win, "%-*s", max_width, value);

  if (is_selected) {
    wattroff(win, COLOR_PAIR(COLOR_PAIR_SELECTED));
    wattroff(win, A_BOLD | COLOR_PAIR(COLOR_PAIR_ACCENT));
  }
}

void draw_toggle_field(WINDOW *win, int y, int x, const char *label, int value, int is_selected) {
  if (is_selected) {
    wattron(win, A_BOLD | COLOR_PAIR(COLOR_PAIR_ACCENT));
    mvwprintw(win, y, x, "> ");
  } else {
    mvwprintw(win, y, x, "  ");
  }

  wprintw(win, "%-25s: ", label);

  if (is_selected) {
    wattron(win, COLOR_PAIR(COLOR_PAIR_SELECTED));
  } else if (value) {
    wattron(win, COLOR_PAIR(COLOR_PAIR_SUCCESS));
  }

  wprintw(win, "[%c] %s", value ? 'X' : ' ', value ? "Yes" : "No");

  if (is_selected) {
    wattroff(win, COLOR_PAIR(COLOR_PAIR_SELECTED));
    wattroff(win, A_BOLD | COLOR_PAIR(COLOR_PAIR_ACCENT));
  } else if (value) {
    wattroff(win, COLOR_PAIR(COLOR_PAIR_SUCCESS));
  }
}

void draw_ui(WINDOW *win, UIConfig *config, int selected_field) {
  int max_y, max_x;
  getmaxyx(win, max_y, max_x);

  wclear(win);
  box(win, 0, 0);

  draw_header(win);

  // Draw result if available
  if (config->has_result) {
    int result_y = 3;
    wattron(win, A_BOLD | COLOR_PAIR(COLOR_PAIR_RESULT));
    mvwprintw(win, result_y, 4, "Result:");
    wattroff(win, A_BOLD | COLOR_PAIR(COLOR_PAIR_RESULT));

    // Display calculation time if requested
    if (config->show_time) {
      wattron(win, COLOR_PAIR(COLOR_PAIR_DIM));
      mvwprintw(win, result_y, 12, "(%.6f seconds)", config->calc_time);
      wattroff(win, COLOR_PAIR(COLOR_PAIR_DIM));
    }

    // Display the result (may wrap for large numbers)
    if (!config->time_only && config->result_string) {
      int result_display_y = result_y + 1;
      int max_result_width = max_x - 8;

      // Word wrap the result for very large numbers
      const char *ptr = config->result_string;
      int line = 0;
      while (*ptr && line < 5) {
        int chars_to_print = max_result_width;
        int remaining = strlen(ptr);
        if (remaining < chars_to_print) {
          chars_to_print = remaining;
        }

        mvwprintw(win, result_display_y + line, 6, "%.*s", chars_to_print, ptr);
        ptr += chars_to_print;
        line++;
      }

      if (*ptr) {
        mvwprintw(win, result_display_y + line, 6, "... (truncated)");
      }
    }

    // Separator line
    wattron(win, A_DIM);
    mvwhline(win, result_y + 8, 2, ACS_HLINE, max_x - 4);
    wattroff(win, A_DIM);
  }

  int y = config->has_result ? 13 : 4;
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

  y++;
  const char *file_display = config->has_output_file ? config->output_file : "(none)";
  draw_field(win, y++, 4, "Output File", file_display, selected_field == FIELD_OUTPUT_FILE, 30);

  y += 2;

  if (selected_field == FIELD_CONFIRM) {
    wattron(win, A_BOLD | COLOR_PAIR(COLOR_PAIR_BUTTON));
    mvwprintw(win, y, max_x / 2 - 8, "  [ CALCULATE ]  ");
    wattroff(win, A_BOLD | COLOR_PAIR(COLOR_PAIR_BUTTON));
  } else {
    wattron(win, A_BOLD | COLOR_PAIR(COLOR_PAIR_ACCENT));
    mvwprintw(win, y, max_x / 2 - 8, "  [ CALCULATE ]  ");
    wattroff(win, A_BOLD | COLOR_PAIR(COLOR_PAIR_ACCENT));
  }

  draw_footer(win, max_y);

  wrefresh(win);
}
