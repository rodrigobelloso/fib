#ifndef UI_THEME_H
#define UI_THEME_H

#include "ui_types.h"

extern ThemeType current_theme;

void init_colors(ThemeType theme);
const char *get_theme_name(ThemeType theme);
void cycle_theme(void);

#endif  // UI_THEME_H
