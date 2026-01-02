#include "ui_theme.h"
#include <ncurses.h>

ThemeType current_theme = THEME_DEFAULT;

void init_colors(ThemeType theme) {
  if (!has_colors()) {
    return;
  }

  start_color();
  use_default_colors();

  switch (theme) {
    case THEME_DEFAULT:
      init_pair(COLOR_PAIR_HEADER, COLOR_CYAN, -1);
      init_pair(COLOR_PAIR_SELECTED, COLOR_BLACK, COLOR_WHITE);
      init_pair(COLOR_PAIR_RESULT, COLOR_GREEN, -1);
      init_pair(COLOR_PAIR_BUTTON, COLOR_YELLOW, -1);
      init_pair(COLOR_PAIR_DIM, COLOR_WHITE, -1);
      init_pair(COLOR_PAIR_ERROR, COLOR_RED, -1);
      init_pair(COLOR_PAIR_SUCCESS, COLOR_GREEN, -1);
      init_pair(COLOR_PAIR_GRAPH, COLOR_MAGENTA, -1);
      init_pair(COLOR_PAIR_ACCENT, COLOR_BLUE, -1);
      break;

    case THEME_DARK:
      init_pair(COLOR_PAIR_HEADER, COLOR_WHITE, COLOR_BLACK);
      init_pair(COLOR_PAIR_SELECTED, COLOR_BLACK, COLOR_CYAN);
      init_pair(COLOR_PAIR_RESULT, COLOR_CYAN, -1);
      init_pair(COLOR_PAIR_BUTTON, COLOR_BLACK, COLOR_WHITE);
      init_pair(COLOR_PAIR_DIM, COLOR_BLACK, -1);
      init_pair(COLOR_PAIR_ERROR, COLOR_RED, -1);
      init_pair(COLOR_PAIR_SUCCESS, COLOR_GREEN, -1);
      init_pair(COLOR_PAIR_GRAPH, COLOR_BLUE, -1);
      init_pair(COLOR_PAIR_ACCENT, COLOR_MAGENTA, -1);
      break;

    case THEME_MATRIX:
      init_pair(COLOR_PAIR_HEADER, COLOR_GREEN, COLOR_BLACK);
      init_pair(COLOR_PAIR_SELECTED, COLOR_BLACK, COLOR_GREEN);
      init_pair(COLOR_PAIR_RESULT, COLOR_GREEN, -1);
      init_pair(COLOR_PAIR_BUTTON, COLOR_BLACK, COLOR_GREEN);
      init_pair(COLOR_PAIR_DIM, COLOR_GREEN, -1);
      init_pair(COLOR_PAIR_ERROR, COLOR_GREEN, -1);
      init_pair(COLOR_PAIR_SUCCESS, COLOR_GREEN, -1);
      init_pair(COLOR_PAIR_GRAPH, COLOR_GREEN, -1);
      init_pair(COLOR_PAIR_ACCENT, COLOR_GREEN, -1);
      break;

    case THEME_OCEAN:
      init_pair(COLOR_PAIR_HEADER, COLOR_CYAN, COLOR_BLUE);
      init_pair(COLOR_PAIR_SELECTED, COLOR_WHITE, COLOR_BLUE);
      init_pair(COLOR_PAIR_RESULT, COLOR_CYAN, -1);
      init_pair(COLOR_PAIR_BUTTON, COLOR_WHITE, COLOR_BLUE);
      init_pair(COLOR_PAIR_DIM, COLOR_BLUE, -1);
      init_pair(COLOR_PAIR_ERROR, COLOR_MAGENTA, -1);
      init_pair(COLOR_PAIR_SUCCESS, COLOR_CYAN, -1);
      init_pair(COLOR_PAIR_GRAPH, COLOR_CYAN, -1);
      init_pair(COLOR_PAIR_ACCENT, COLOR_BLUE, -1);
      break;

    case THEME_SUNSET:
      init_pair(COLOR_PAIR_HEADER, COLOR_YELLOW, COLOR_RED);
      init_pair(COLOR_PAIR_SELECTED, COLOR_BLACK, COLOR_YELLOW);
      init_pair(COLOR_PAIR_RESULT, COLOR_YELLOW, -1);
      init_pair(COLOR_PAIR_BUTTON, COLOR_BLACK, COLOR_YELLOW);
      init_pair(COLOR_PAIR_DIM, COLOR_RED, -1);
      init_pair(COLOR_PAIR_ERROR, COLOR_RED, -1);
      init_pair(COLOR_PAIR_SUCCESS, COLOR_YELLOW, -1);
      init_pair(COLOR_PAIR_GRAPH, COLOR_MAGENTA, -1);
      init_pair(COLOR_PAIR_ACCENT, COLOR_RED, -1);
      break;

    case THEME_CYBERPUNK:
      init_pair(COLOR_PAIR_HEADER, COLOR_MAGENTA, COLOR_BLACK);
      init_pair(COLOR_PAIR_SELECTED, COLOR_BLACK, COLOR_MAGENTA);
      init_pair(COLOR_PAIR_RESULT, COLOR_CYAN, -1);
      init_pair(COLOR_PAIR_BUTTON, COLOR_BLACK, COLOR_CYAN);
      init_pair(COLOR_PAIR_DIM, COLOR_MAGENTA, -1);
      init_pair(COLOR_PAIR_ERROR, COLOR_RED, -1);
      init_pair(COLOR_PAIR_SUCCESS, COLOR_CYAN, -1);
      init_pair(COLOR_PAIR_GRAPH, COLOR_YELLOW, -1);
      init_pair(COLOR_PAIR_ACCENT, COLOR_MAGENTA, -1);
      break;

    default:
      init_pair(COLOR_PAIR_HEADER, COLOR_CYAN, -1);
      init_pair(COLOR_PAIR_SELECTED, COLOR_BLACK, COLOR_WHITE);
      init_pair(COLOR_PAIR_RESULT, COLOR_GREEN, -1);
      init_pair(COLOR_PAIR_BUTTON, COLOR_YELLOW, -1);
      init_pair(COLOR_PAIR_DIM, COLOR_WHITE, -1);
      init_pair(COLOR_PAIR_ERROR, COLOR_RED, -1);
      init_pair(COLOR_PAIR_SUCCESS, COLOR_GREEN, -1);
      init_pair(COLOR_PAIR_GRAPH, COLOR_MAGENTA, -1);
      init_pair(COLOR_PAIR_ACCENT, COLOR_BLUE, -1);
      break;
  }
}

const char *get_theme_name(ThemeType theme) {
  switch (theme) {
    case THEME_DEFAULT:
      return "Default";
    case THEME_DARK:
      return "Dark";
    case THEME_MATRIX:
      return "Matrix";
    case THEME_OCEAN:
      return "Ocean";
    case THEME_SUNSET:
      return "Sunset";
    case THEME_CYBERPUNK:
      return "Cyberpunk";
    default:
      return "Unknown";
  }
}

void cycle_theme(void) {
  current_theme = (current_theme + 1) % THEME_COUNT;
  init_colors(current_theme);
}
