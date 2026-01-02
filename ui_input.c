#include "ui_input.h"
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

int edit_number(WINDOW *win, long *value, long min_val, long max_val) {
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
    if (ch == KEY_BACKSPACE || ch == 127 || ch == 8 || ch == KEY_DC) {
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

int edit_string(WINDOW *win, char *buffer, int max_len, const char *prompt) {
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
    if (ch == KEY_BACKSPACE || ch == 127 || ch == 8 || ch == KEY_DC) {
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
    size_t copy_len = strlen(temp);
    if (copy_len >= (size_t) max_len) {
      copy_len = (size_t) (max_len - 1);
    }
    memcpy(buffer, temp, copy_len);
    buffer[copy_len] = '\0';
    return 1;
  }

  return 0;
}
