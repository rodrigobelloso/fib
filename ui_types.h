#ifndef UI_TYPES_H
#define UI_TYPES_H

#include <ncurses.h>
#include "fib.h"

#define MAX_INPUT_SIZE 256
#define MAX_NUMBER_LEN 32

typedef struct {
  long fib_number;
  char algorithm[16];
  char format[16];
  int show_time;
  int time_only;
  int raw_output;
  char output_file[MAX_INPUT_SIZE];
  int has_output_file;
  char *result_string;
  int has_result;
  double calc_time;
} UIConfig;

typedef enum {
  FIELD_NUMBER,
  FIELD_ALGORITHM,
  FIELD_FORMAT,
  FIELD_SHOW_TIME,
  FIELD_TIME_ONLY,
  FIELD_RAW_OUTPUT,
  FIELD_OUTPUT_FILE,
  FIELD_CONFIRM,
  FIELD_COUNT
} FieldType;

typedef enum { VIEW_MAIN, VIEW_HISTORY, VIEW_PERFORMANCE } ViewMode;

typedef enum {
  COLOR_PAIR_DEFAULT = 1,
  COLOR_PAIR_HEADER,
  COLOR_PAIR_SELECTED,
  COLOR_PAIR_RESULT,
  COLOR_PAIR_BUTTON,
  COLOR_PAIR_DIM,
  COLOR_PAIR_ERROR,
  COLOR_PAIR_SUCCESS,
  COLOR_PAIR_GRAPH,
  COLOR_PAIR_ACCENT
} ColorPairs;

typedef enum {
  THEME_DEFAULT,
  THEME_DARK,
  THEME_MATRIX,
  THEME_OCEAN,
  THEME_SUNSET,
  THEME_CYBERPUNK,
  THEME_COUNT
} ThemeType;

#endif  // UI_TYPES_H
