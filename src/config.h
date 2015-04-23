#pragma once

#include "globals.h"

void config_init();
void config_deinit();
void refresh_config();

int config_get(int key);
void config_set(int key, int val);
