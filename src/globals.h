#pragma once

#include <pebble.h>

// #define ANTIALIASING

// Layout
#define MARGIN 9
#define THICKNESSMARKS 3
#define THICKNESS 3
#define ANIM_DELTA 34
#define HAND_LENGTH_SEC 70
#define HAND_LENGTH_SEC_INVERSE 22
#define HAND_LENGTH_MIN 65
#define HAND_LENGTH_HOUR 45
#define HOUR_HAND_TICKNESS 3
#define MINUTE_HAND_TICKNESS 3

#define PERSIST_DEFAULTS_SET 3489

// Persist
#define PERSIST_KEY_DATE        0
#define PERSIST_KEY_DAY         1
#define PERSIST_KEY_BT          2
#define PERSIST_KEY_BATTERY     3
#define PERSIST_KEY_SECOND_HAND 4
#define NUM_SETTINGS            5

#define HAND_TYPE            0 // 0 - Flat | 1 - 



#define GRECT_FULL_WINDOW GRect(0,0,144,168)

				
typedef struct {
  int days;
  int hours;
  int minutes;
  int seconds;
} Time;
