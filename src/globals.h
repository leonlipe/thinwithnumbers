#pragma once

#include <pebble.h>

// #define ANTIALIASING

// Layout
#define THICKNESSMARKS 3
#define THICKNESS 3
#define ANIM_DELTA 34
#define HOUR_HAND_TICKNESS  3
#define MINUTE_HAND_TICKNESS 3

#define PERSIST_DEFAULTS_SET 3489

// Persist
#define PERSIST_KEY_DATE        0
#define PERSIST_KEY_DAY         1
#define PERSIST_KEY_BT          2
#define PERSIST_KEY_BATTERY     3
#define PERSIST_KEY_SECOND_HAND 4
#define PERSIST_BACKTYPE		5
#define PERSIST_MARGIN			6
#define PERSIST_HAND_LENGTH_SEC 		7
#define PERSIST_HAND_LENGTH_SEC_INVERSE 8
#define PERSIST_HAND_LENGTH_MIN 9
#define PERSIST_HAND_LENGTH_HOUR 10
#define PERSIST_HAND_TYPE 11
#define PERSIST_TEMPERATURE 12
#define PERSIST_CONDITIONS 13
#define PERSIST_HUMIDITY 14
#define PERSIST_WIND 15
#define PERSIST_SUNSET 16
#define PERSIST_SUNRISE 17
#define PERSIST_DIGITALTIME 18
#define PERSIST_INVERTED 19
#define PERSIST_NUMBERS 20

#define NUM_SETTINGS            21

#define GRECT_FULL_WINDOW GRect(0,0,144,168)

				
typedef struct {
  int days;
  int hours;
  int minutes;
  int seconds;
} Time;
