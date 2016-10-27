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
    persist_write_int(PERSIST_WIND, 0);
    persist_write_int(PERSIST_SUNSET, 1);
    persist_write_int(PERSIST_SUNRISE, 1);
    persist_write_int(PERSIST_DIGITALTIME, 0);
    persist_write_int(PERSIST_INVERTED, 0);
    persist_write_int(PERSIST_NUMBERS, 1);
    persist_write_int(PERSIST_MINUTES, 0);
    persist_write_int(PERSIST_CELCIUS, 1);
    persist_write_int(PERSIST_POLLTIME, 60);


    persist_write_string(PERSIST_WE_TEMP, "");
    persist_write_string(PERSIST_WE_HUM, "");
    persist_write_string(PERSIST_WE_SUN, "");
    persist_write_string(PERSIST_CONDITIONS_ID, "");
    persist_write_string(PERSIST_WEATHER_API, "");
    persist_write_int(PERSIST_SERVICE_PROVIDER, 0);

  }

  if (persist_read_int(PERSIST_POLLTIME) == 0){
    persist_write_int(PERSIST_POLLTIME,60);
  }

  refresh_config();

  
}

void config_deinit(){
  for(int i = 0; i < NUM_SETTINGS; i++) {
     persist_write_int(i,s_arr[i] );
  }
    persist_write_string(PERSIST_WE_TEMP, temp_text);
    persist_write_string(PERSIST_WE_HUM, hum_text);
    persist_write_string(PERSIST_WE_SUN, sun_text);
    persist_write_string(PERSIST_CONDITIONS_ID, cond_icon_text);
}
void refresh_config(){
  for(int i = 0; i < NUM_SETTINGS; i++) {
    s_arr[i] = persist_read_int(i);
  }

    persist_read_string(PERSIST_WE_TEMP, temp_text, sizeof(temp_text));
    persist_read_string(PERSIST_WE_HUM, hum_text, sizeof(hum_text));
    persist_read_string(PERSIST_WE_SUN, sun_text, sizeof(sun_text));
    persist_read_string(PERSIST_CONDITIONS_ID, cond_icon_text, sizeof(cond_icon_text));

    persist_read_string(PERSIST_WEATHER_API, weatherApi, sizeof(weatherApi));
    weatherProvider = persist_read_int(PERSIST_SERVICE_PROVIDER);
}

int config_get(int key) {
  return s_arr[key];
}

void config_set(int key, int val) {
  s_arr[key] = val;
}