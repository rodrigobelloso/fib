#ifndef UI_DRAW_H
#define UI_DRAW_H

#include "ui_types.h"
#include <ncurses.h>

void draw_header(WINDOW *win);
void draw_footer(WINDOW *win, int max_y);
void draw_history_footer(WINDOW *win, int max_y);
void draw_performance_footer(WINDOW *win, int max_y);
void draw_performance_graph(WINDOW *win, int scroll_offset);
void draw_history_view(WINDOW *win, int selected_index, int scroll_offset);
void draw_field(WINDOW *win, int y, int x, const char *label, const char *value, int is_selected,
                int max_width);
void draw_toggle_field(WINDOW *win, int y, int x, const char *label, int value, int is_selected);
void draw_ui(WINDOW *win, UIConfig *config, int selected_field);

#endif  // UI_DRAW_H
