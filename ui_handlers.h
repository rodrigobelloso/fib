#ifndef UI_HANDLERS_H
#define UI_HANDLERS_H

#include "ui_types.h"
#include <ncurses.h>

void cycle_algorithm(char *algorithm);
void cycle_format(char *format);
void calculate_result(UIConfig *config);
void handle_history_up(int *history_selected, int *history_scroll);
void handle_history_down(int *history_selected, int *history_scroll, int max_y);
void handle_history_delete(int *history_selected, int *history_scroll);
void handle_space_key(UIConfig *config, int selected_field);
void handle_enter_key(WINDOW *main_win, UIConfig *config, int selected_field);

#endif  // UI_HANDLERS_H
