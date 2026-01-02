#ifndef UI_INPUT_H
#define UI_INPUT_H

#include "ui_types.h"
#include <ncurses.h>

int edit_number(WINDOW *win, long *value, long min_val, long max_val);
int edit_string(WINDOW *win, char *buffer, int max_len, const char *prompt);

#endif  // UI_INPUT_H
