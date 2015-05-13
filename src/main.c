#include "globals.h"
#include "main_window.h"
#include "config.h"

int main() {
  //setlocale(LC_TIME, "es_ES");
  comm_init();
  config_init();
  main_init();
  main_window_push();
  app_event_loop();
  config_deinit();

}
