#include "config.h"

static int s_arr[NUM_SETTINGS];

void config_init() {
  // Set defaults
  if(!persist_exists(PERSIST_DEFAULTS_SET)) {
    persist_write_bool(PERSIST_DEFAULTS_SET, true);

    persist_write_int(PERSIST_KEY_DATE, 1);
    persist_write_int(PERSIST_KEY_DAY, 1);
    persist_write_int(PERSIST_KEY_BT, 1);
    persist_write_int(PERSIST_KEY_BATTERY, 0);
    persist_write_int(PERSIST_KEY_SECOND_HAND, 1);
    persist_write_int(PERSIST_BACKTYPE,0);
    persist_write_int(PERSIST_MARGIN,6);
    persist_write_int(PERSIST_HAND_LENGTH_SEC,65);
    persist_write_int(PERSIST_HAND_LENGTH_SEC_INVERSE, 15);
    persist_write_int(PERSIST_HAND_LENGTH_MIN, 65);
    persist_write_int(PERSIST_HAND_LENGTH_HOUR, 45);

    persist_write_int(PERSIST_HAND_TYPE, 2);
    persist_write_int(PERSIST_TEMPERATURE, 1);
    persist_write_int(PERSIST_CONDITIONS, 1);
    persist_write_int(PERSIST_HUMIDITY, 1);
    persist_write_int(PERSIST_WIND, 1);
    persist_write_int(PERSIST_SUNSET, 1);
    persist_write_int(PERSIST_SUNRISE, 1);
    persist_write_int(PERSIST_DIGITALTIME, 0);
    persist_write_int(PERSIST_INVERTED, 0);
    persist_write_int(PERSIST_NUMBERS, 1);
    persist_write_int(PERSIST_MINUTES, 0);

  }

  refresh_config();

  
}

void config_deinit(){
  for(int i = 0; i < NUM_SETTINGS; i++) {
     persist_write_int(i,s_arr[i] );
  }
}
void refresh_config(){
  for(int i = 0; i < NUM_SETTINGS; i++) {
    s_arr[i] = persist_read_int(i);
  }
}

int config_get(int key) {
  return s_arr[key];
}

void config_set(int key, int val) {
  s_arr[key] = val;
}