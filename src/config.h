#pragma once

#include "globals.h"

char temp_text[15], hum_text[15], sun_text[20], cond_icon_text[5];


void config_init();
void config_deinit();
void refresh_config();

int config_get(int key);
void config_set(int key, int val);
