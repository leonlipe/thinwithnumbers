#include "main_window.h"
/*const GPathInfo MINUTE_HAND_PATH_POINTS = { 5, (GPoint[] ) { 
  { 0, 0 },
  { -5, -50 }, 
  { 0, -70 }, 
  { 5, -50 }, 
  { 0, 0 },
} };
*/
#define KEY_TEMPERATURE 0
#define KEY_CONDITIONS 1
#define KEY_HUMIDITY 2
#define KEY_WIND 3
#define KEY_SUNRISE 4
#define KEY_SUNSET 5
/*
Flat Minutes
*/
 const GPathInfo MINUTE_HAND_PATH_POINTS_FLAT = { 4, (GPoint[] ) { 
  { MINUTE_HAND_TICKNESS*-1, 0 },
  { MINUTE_HAND_TICKNESS, 0 }, 
  { MINUTE_HAND_TICKNESS, -70 }, 
  { MINUTE_HAND_TICKNESS*-1, -70 }, 
} };

const GPathInfo HOUR_HAND_PATH_POINTS_FLAT = { 4, (GPoint[] ) { 
  { HOUR_HAND_TICKNESS*-1, 0 },
  { HOUR_HAND_TICKNESS, 0 }, 
  { HOUR_HAND_TICKNESS, -50 }, 
  { HOUR_HAND_TICKNESS*-1, -50 }, 
} };




const GPathInfo MINUTE_HAND_PATH_POINTS = { 5, (GPoint[] ) { 
  { -3, 0 },
  { 3, 0 }, 
  { 5, -50 }, 
  { 0, -70 }, 
  { -5, -50 }, 
} };

const GPathInfo MINUTE_HAND_PATH_INNER_POINTS = { 5, (GPoint[] ) { 
  { -1, -8 },
  { 1, -8 }, 
  { 3, -50 }, 
  { 0, -60 }, 

  { -3, -50 }, 
} };

const GPathInfo HOUR_HAND_PATH_POINTS = { 5, (GPoint[] ) { 
  { -3, 0 },
  { 3, 0 }, 
  { 4, -30 }, 
  { 0, -50 }, 
  { -4, -30 }, 
} };

const GPathInfo HOUR_HAND_PATH_INNER_POINTS = { 5, (GPoint[] ) { 
  { -1, -3 },
  { 1, -3 }, 
  { 3, -30 }, 
  { 0, -40 }, 

  { -3, -30 }, 
} };

static Window *s_main_window;
static TextLayer *s_weekday_layer, *s_day_in_month_layer, *s_month_layer, *s_weather_layer;
static Layer *s_canvas_layer, *s_bg_layer, *s_battery_layer;

static BitmapLayer *s_bitmapbackground_layer;
static GBitmap *s_background_bitmap;
static GBitmap *icon_battery, *icon_battery_low;
static GBitmap *icon_battery_charge;
static GBitmap *icon_bt_disconected;

static GPath *minute_hand_path, *minute_hand_path_inner, *hour_hand_path, *hour_hand_path_inner;

static Time s_last_time, s_anim_time;
static char s_weekday_buffer[8], s_month_buffer[8], s_day_in_month_buffer[3];
static bool s_animating, s_connected;

static uint8_t battery_level;
static bool battery_plugged;



static void bg_update_proc(Layer *layer, GContext *ctx) {
  GRect bounds = layer_get_bounds(layer);
  GPoint center = grect_center_point(&bounds);

  BatteryChargeState state = battery_state_service_peek();
  bool plugged = state.is_plugged;
  int perc = state.charge_percent;
  int batt_hours = (int)(12.0F * ((float)perc / 100.0F)) + 1;
  

  /*for(int h = 0; h < 12; h++) {
    for(int y = 0; y < THICKNESSMARKS; y++) {
      for(int x = 0; x < THICKNESSMARKS; x++) {
        GPoint point = (GPoint) {
          //int32_t second_angle = TRIG_MAX_ANGLE * t.tm_sec / 60;
          //secondHand.x = (sin_lookup(second_angle) * secondHandLength / TRIG_MAX_RATIO) + center.x;
          .x = (int16_t)(sin_lookup(TRIG_MAX_ANGLE * h / 12) * (int32_t)(3 * HAND_LENGTH_SEC) / TRIG_MAX_RATIO) + center.x,
          //secondHand.y = (-cos_lookup(second_angle) * secondHandLength / TRIG_MAX_RATIO) + center.y;
          .y = (int16_t)(-cos_lookup(TRIG_MAX_ANGLE * h / 12) * (int32_t)(3 * HAND_LENGTH_SEC) / TRIG_MAX_RATIO) + center.y,
        };

          #ifdef PBL_COLOR
                  if(config_get(PERSIST_KEY_BATTERY)) {
                    if(h < batt_hours) {
                      if(plugged) {
                        // Charging
                        graphics_context_set_stroke_color(ctx, GColorGreen);
                      } else {
                        // Discharging at this level
                        graphics_context_set_stroke_color(ctx, GColorWhite);
                      }
                    } else {
                      // Empty segment
                      graphics_context_set_stroke_color(ctx, GColorDarkGray);
                    }
                  } else {
                    // No battery indicator
                    graphics_context_set_stroke_color(ctx, GColorWhite);
                  }
          #else
                  if(config_get(PERSIST_KEY_BATTERY)) {
                    if(h < batt_hours) {
                      // Discharging at this level
                      graphics_context_set_stroke_color(ctx, GColorWhite);
                    } else {
                      // Empty segment
                      graphics_context_set_stroke_color(ctx, GColorBlack);
                    }
                  } else {
                    // No battery indicator
                    graphics_context_set_stroke_color(ctx, GColorWhite);
                  }
          #endif
          #if defined(ANTIALIASING) && defined(PBL_COLOR)
                  graphics_draw_line_antialiased(ctx, GPoint(center.x + x, center.y + y), GPoint(point.x + x, point.y + y), GColorWhite);
          #else
                  graphics_draw_line(ctx, GPoint(center.x + x, center.y + y), GPoint(point.x + x, point.y + y));
          #endif
      }
    }
  }

  // Make markers
  graphics_context_set_fill_color(ctx, GColorBlack);
  graphics_fill_rect(ctx, GRect(MARGIN, MARGIN, bounds.size.w - (2 * MARGIN), bounds.size.h - (2 * MARGIN)), 0, GCornerNone);
  */
}

static GPoint make_hand_point(int quantity, int intervals, int len, GPoint center) {
  return (GPoint) {
    .x = (int16_t)(sin_lookup(TRIG_MAX_ANGLE * quantity / intervals) * (int32_t)len / TRIG_MAX_RATIO) + center.x,
    .y = (int16_t)(-cos_lookup(TRIG_MAX_ANGLE * quantity / intervals) * (int32_t)len / TRIG_MAX_RATIO) + center.y,
  };
}

static void draw_proc(Layer *layer, GContext *ctx) {
  GRect bounds = layer_get_bounds(layer);
  GPoint center = grect_center_point(&bounds);

  Time now;
  
  if(s_animating) {
    now = s_anim_time;
  } else {
    now = s_last_time;
  }

  int now_plus_30_seconds = now.seconds + 30;
  if (now_plus_30_seconds > 60){
    now_plus_30_seconds -= 60;
  }



 



  // Plot hand ends
  GPoint second_hand_inverse = make_hand_point(now_plus_30_seconds, 60, HAND_LENGTH_SEC_INVERSE, center);
  GPoint second_hand_inverse_circle = make_hand_point(now_plus_30_seconds, 60, HAND_LENGTH_SEC_INVERSE+4, center);
  GPoint second_hand_long = make_hand_point(now.seconds, 60, HAND_LENGTH_SEC, center);
  GPoint minute_hand_long = make_hand_point(now.minutes, 60, HAND_LENGTH_MIN, center);
  GPoint second_hand_short = make_hand_point(now.seconds, 60, (HAND_LENGTH_SEC - MARGIN + 2), center);
  GPoint minute_hand_short = make_hand_point(now.minutes, 60, (HAND_LENGTH_MIN - MARGIN + 2), center);

  // Adjust for minutes through the hour
  float minute_angle = TRIG_MAX_ANGLE * now.minutes / 60;
  float hour_angle = TRIG_MAX_ANGLE * now.hours / 12;
  hour_angle += (minute_angle / TRIG_MAX_ANGLE) * (TRIG_MAX_ANGLE / 12);

  // Hour is more accurate
  GPoint hour_hand_long = (GPoint) {
    .x = (int16_t)(sin_lookup(hour_angle) * (int32_t)HAND_LENGTH_HOUR / TRIG_MAX_RATIO) + center.x,
    .y = (int16_t)(-cos_lookup(hour_angle) * (int32_t)HAND_LENGTH_HOUR / TRIG_MAX_RATIO) + center.y,
  };
  GPoint hour_hand_short = (GPoint) {
    .x = (int16_t)(sin_lookup(hour_angle) * (int32_t)(HAND_LENGTH_HOUR - MARGIN + 2) / TRIG_MAX_RATIO) + center.x,
    .y = (int16_t)(-cos_lookup(hour_angle) * (int32_t)(HAND_LENGTH_HOUR - MARGIN + 2) / TRIG_MAX_RATIO) + center.y,
  };

  // Draw hands
  for(int y = 0; y < THICKNESS; y++) {
    for(int x = 0; x < THICKNESS; x++) {
#ifdef PBL_COLOR
      graphics_context_set_stroke_color(ctx, GColorDarkGray);
#elif PBL_BW 
      graphics_context_set_stroke_color(ctx, GColorWhite);
#endif
#if defined(ANTIALIASING) && defined(PBL_COLOR)
      graphics_draw_line_antialiased(ctx, GPoint(center.x + x, center.y + y), GPoint(minute_hand_short.x + x, minute_hand_short.y + y), GColorDarkGray);
      graphics_draw_line_antialiased(ctx, GPoint(center.x + x, center.y + y), GPoint(hour_hand_short.x + x, hour_hand_short.y + y), GColorDarkGray);
      graphics_context_set_stroke_color(ctx, GColorWhite);
      graphics_draw_line_antialiased(ctx, GPoint(minute_hand_short.x + x, minute_hand_short.y + y), GPoint(minute_hand_long.x + x, minute_hand_long.y + y), GColorDarkGray);
      graphics_draw_line_antialiased(ctx, GPoint(hour_hand_short.x + x, hour_hand_short.y + y), GPoint(hour_hand_long.x + x, hour_hand_long.y + y), GColorDarkGray);
#else
      /*graphics_draw_line(ctx, GPoint(center.x + x, center.y + y), GPoint(minute_hand_short.x + x, minute_hand_short.y + y));
      graphics_draw_line(ctx, GPoint(center.x + x, center.y + y), GPoint(hour_hand_short.x + x, hour_hand_short.y + y));
      graphics_context_set_stroke_color(ctx, GColorWhite);
      graphics_draw_line(ctx, GPoint(minute_hand_short.x + x, minute_hand_short.y + y), GPoint(minute_hand_long.x + x, minute_hand_long.y + y));
      graphics_draw_line(ctx, GPoint(hour_hand_short.x + x, hour_hand_short.y + y), GPoint(hour_hand_long.x + x, hour_hand_long.y + y));
      */
       gpath_rotate_to(hour_hand_path, hour_angle);
      graphics_context_set_fill_color(ctx, GColorWhite);
      graphics_context_set_stroke_color(ctx, GColorBlack);
      gpath_draw_filled(ctx, hour_hand_path);
      gpath_draw_outline(ctx, hour_hand_path);


      if (HAND_TYPE == 1){
       gpath_rotate_to(hour_hand_path_inner, hour_angle);
      graphics_context_set_fill_color(ctx, GColorBlack);
      gpath_draw_filled(ctx, hour_hand_path_inner);
      }

      // Draw minute hand with path
      gpath_rotate_to(minute_hand_path, minute_angle);
      graphics_context_set_fill_color(ctx, GColorWhite);
      graphics_context_set_stroke_color(ctx, GColorBlack);
      gpath_draw_filled(ctx, minute_hand_path);
      gpath_draw_outline(ctx, minute_hand_path);

       if (HAND_TYPE == 1){
      gpath_rotate_to(minute_hand_path_inner, minute_angle);
      graphics_context_set_fill_color(ctx, GColorBlack);
      gpath_draw_filled(ctx, minute_hand_path_inner);
      }
     

      
      // End draw hand
#endif
    }
  }

  // Draw seconds hand
  if(config_get(PERSIST_KEY_SECOND_HAND)) {
    for(int y = 0; y < THICKNESS - 1; y++) {
      for(int x = 0; x < THICKNESS - 1; x++) {
        #ifdef PBL_COLOR
                graphics_context_set_stroke_color(ctx, GColorDarkCandyAppleRed);
        #elif PBL_BW
                graphics_context_set_stroke_color(ctx, GColorWhite);
        #endif
        #if defined(ANTIALIASING) && defined(PBL_COLOR)
                graphics_draw_line_antialiased(ctx, GPoint(center.x + x, center.y + y), GPoint(second_hand_short.x + x, second_hand_short.y + y), GColorDarkCandyAppleRed);
        #else
                 graphics_draw_circle(ctx, GPoint(second_hand_inverse_circle.x + 1, second_hand_inverse_circle.y + 1), 4);
 
                 graphics_draw_line(ctx, GPoint(center.x + x, center.y + y), GPoint(second_hand_inverse.x + x, second_hand_inverse.y + y));
                 graphics_draw_line(ctx, GPoint(center.x + x, center.y + y), GPoint(second_hand_short.x + x, second_hand_short.y + y));
        #endif

                // Draw second hand tip
        #ifdef PBL_COLOR
                graphics_context_set_stroke_color(ctx, GColorChromeYellow);
        #elif PBL_BW
                graphics_context_set_stroke_color(ctx, GColorWhite);
        #endif
        #if defined(ANTIALIASING) && defined(PBL_COLOR)
                graphics_draw_line_antialiased(ctx, GPoint(second_hand_short.x + x, second_hand_short.y + y), GPoint(second_hand_long.x + x, second_hand_long.y + y), GColorChromeYellow);
        #else
                graphics_draw_line(ctx, GPoint(second_hand_short.x + x, second_hand_short.y + y), GPoint(second_hand_long.x + x, second_hand_long.y + y));
        #endif
      }
    }
  }

  // Center
  graphics_context_set_stroke_color(ctx, GColorBlack);
  graphics_context_set_fill_color(ctx, GColorWhite);
#if defined(ANTIALIASING) && defined(PBL_COLOR)
  graphics_fill_circle_antialiased(ctx, GPoint(center.x + 1, center.y + 1), 4, GColorWhite);
#else
  //graphics_draw_circle(ctx, GPoint(center.x + 1, center.y + 1), 4);
  //graphics_fill_circle(ctx, GPoint(center.x + 1, center.y + 1), 3);
  //graphics_context_set_fill_color(ctx, GColorBlack);
  //graphics_fill_circle(ctx, GPoint(center.x + 1, center.y + 1), 1);
    graphics_context_set_fill_color(ctx, GColorBlack);
    graphics_context_set_stroke_color(ctx, GColorWhite);
    graphics_draw_circle(ctx, GPoint(center.x + 1, center.y + 1), 4);
    graphics_fill_circle(ctx, GPoint(center.x + 1, center.y + 1), 3);
    graphics_context_set_fill_color(ctx, GColorWhite);
    graphics_fill_circle(ctx, GPoint(center.x + 1, center.y + 1), 1);
  
#endif
/*
  // Draw black if disconnected
  if(config_get(PERSIST_KEY_BT) && !s_connected) {

// Show something if disconected
#if defined(ANTIALIASING) && defined(PBL_COLOR)
    graphics_fill_circle_antialiased(ctx, GPoint(center.x + 1, center.y + 1), 3, GColorBlack);
#else
    graphics_context_set_fill_color(ctx, GColorBlack);
    graphics_context_set_stroke_color(ctx, GColorWhite);
    graphics_draw_circle(ctx, GPoint(center.x + 1, center.y + 1), 4);
    graphics_fill_circle(ctx, GPoint(center.x + 1, center.y + 1), 3);
#endif


  }
  */
 // gpath_rotate_to(minute_hand_path, (TRIG_MAX_ANGLE / 360) * angle);

 

}

static void anim_handler(void *context) {
  bool changed = false;
  s_last_time.hours -= (s_last_time.hours > 12) ? 12 : 0;
  if(s_anim_time.hours < s_last_time.hours) {
    s_anim_time.hours++;
    changed = true;
  }
  if(s_anim_time.minutes < s_last_time.minutes) {
    s_anim_time.minutes++;
    changed = true;
  }
  if(s_anim_time.seconds < s_last_time.seconds) {
    s_anim_time.seconds++;
    changed = true;
  }

  if(changed) {
    layer_mark_dirty(s_canvas_layer);
    app_timer_register(ANIM_DELTA, anim_handler, NULL);
  } else {
    s_animating = false;
  }
}

static void tick_handler(struct tm *tick_time, TimeUnits changed) {
  s_last_time.days = tick_time->tm_mday;
  s_last_time.hours = tick_time->tm_hour;
  s_last_time.minutes = tick_time->tm_min;
  s_last_time.seconds = tick_time->tm_sec;

  
  snprintf(s_day_in_month_buffer, sizeof(s_day_in_month_buffer), "%d", s_last_time.days);
  strftime(s_weekday_buffer, sizeof(s_weekday_buffer), "%a", tick_time);
  strftime(s_month_buffer, sizeof(s_month_buffer), "%b", tick_time);

  text_layer_set_text(s_weekday_layer, s_weekday_buffer);
  text_layer_set_text(s_day_in_month_layer, s_day_in_month_buffer);
  text_layer_set_text(s_month_layer, s_month_buffer);

  // Finally
  layer_mark_dirty(s_canvas_layer);

    // Get weather update every 30 minutes
  if(tick_time->tm_min % 60 == 0) {
    // Begin dictionary
    DictionaryIterator *iter;
    app_message_outbox_begin(&iter);

    // Add a key-value pair
    dict_write_uint8(iter, 0, 0);

    // Send the message!
    app_message_outbox_send();
  }
}

static void bt_handler(bool connected) {
  // Notify disconnection
  if(!connected && s_connected) {
    vibes_long_pulse();
  }

  s_connected = connected;
  layer_mark_dirty(s_battery_layer);
}

/*static void batt_handler(BatteryChargeState state) {
  layer_mark_dirty(s_canvas_layer);
}
*/
void battery_state_handler(BatteryChargeState charge) {
  battery_level = charge.charge_percent;
  battery_plugged = charge.is_plugged;
  layer_mark_dirty(s_battery_layer);
 // if (!battery_plugged && battery_level < 20)
 //   conserve_power(true);
 // else
 //   conserve_power(false);
}


void battery_layer_update_callback(Layer *layer, GContext *ctx) {

  graphics_context_set_compositing_mode(ctx, GCompOpAssign);

  if (!battery_plugged) {
    if (!battery_plugged){
      if (battery_level < 20){
        graphics_draw_bitmap_in_rect(ctx, icon_battery_low, GRect(105, 0, 24, 12));
      }else{
        graphics_draw_bitmap_in_rect(ctx, icon_battery, GRect(105, 0, 24, 12));
      }
      graphics_context_set_stroke_color(ctx, GColorBlack);
      graphics_context_set_fill_color(ctx, GColorWhite);
      graphics_fill_rect(ctx, GRect(112, 4, (uint8_t)((battery_level / 100.0) * 11.0), 4), 0, GCornerNone);
    }
  } else {
    graphics_draw_bitmap_in_rect(ctx, icon_battery_charge, GRect(105, 0, 24, 12));
  }


   if(config_get(PERSIST_KEY_BT) && !s_connected) {
      graphics_draw_bitmap_in_rect(ctx, icon_bt_disconected, GRect(22, 0, 24, 12));    
   }


}

static void window_load(Window *window) {

  icon_battery = gbitmap_create_with_resource(RESOURCE_ID_BATTERY_ICON);
  icon_battery_low = gbitmap_create_with_resource(RESOURCE_ID_BATTERY_ICON_LOW);
  icon_battery_charge = gbitmap_create_with_resource(RESOURCE_ID_BATTERY_CHARGE);
  icon_bt_disconected = gbitmap_create_with_resource(RESOURCE_ID_BLUETOOTH_DISCONNECTED);

  if (HAND_TYPE == 1){
    minute_hand_path = gpath_create(&MINUTE_HAND_PATH_POINTS);
  }else{
    minute_hand_path = gpath_create(&MINUTE_HAND_PATH_POINTS_FLAT);
  }
  gpath_move_to(minute_hand_path, grect_center_point(&GRECT_FULL_WINDOW));

  minute_hand_path_inner = gpath_create(&MINUTE_HAND_PATH_INNER_POINTS);
  gpath_move_to(minute_hand_path_inner, grect_center_point(&GRECT_FULL_WINDOW));

  if (HAND_TYPE == 1){
    hour_hand_path = gpath_create(&HOUR_HAND_PATH_POINTS);
  }else{
    hour_hand_path = gpath_create(&HOUR_HAND_PATH_POINTS_FLAT);
  }

  gpath_move_to(hour_hand_path, grect_center_point(&GRECT_FULL_WINDOW));

  hour_hand_path_inner = gpath_create(&HOUR_HAND_PATH_INNER_POINTS);
  gpath_move_to(hour_hand_path_inner, grect_center_point(&GRECT_FULL_WINDOW));

  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);

  s_background_bitmap = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_BACKGROUND_MAIN_NUMBERS_DATE);
  s_bitmapbackground_layer = bitmap_layer_create(GRect(0,0,144,168));
  bitmap_layer_set_bitmap(s_bitmapbackground_layer, s_background_bitmap);
  layer_add_child(window_layer, bitmap_layer_get_layer(s_bitmapbackground_layer));

  s_bg_layer = layer_create(bounds);
  layer_set_update_proc(s_bg_layer, bg_update_proc);
  layer_add_child(window_layer, s_bg_layer);

  s_weekday_layer = text_layer_create(GRect(94, 59, 44, 40));
  text_layer_set_text_alignment(s_weekday_layer, GTextAlignmentCenter);
  text_layer_set_font(s_weekday_layer, fonts_get_system_font(FONT_KEY_GOTHIC_14_BOLD));
  text_layer_set_text_color(s_weekday_layer, GColorWhite);
  text_layer_set_background_color(s_weekday_layer, GColorClear);

  s_day_in_month_layer = text_layer_create(GRect(94, 69, 44, 40));
  text_layer_set_text_alignment(s_day_in_month_layer, GTextAlignmentCenter);
  text_layer_set_font(s_day_in_month_layer, fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD));
#ifdef PBL_COLOR
  text_layer_set_text_color(s_day_in_month_layer, GColorChromeYellow);
#elif PBL_BW
  text_layer_set_text_color(s_day_in_month_layer, GColorBlack);
#endif
  text_layer_set_background_color(s_day_in_month_layer, GColorClear);


  

  s_month_layer = text_layer_create(GRect(94, 93, 44, 40));
  text_layer_set_text_alignment(s_month_layer, GTextAlignmentCenter);
  text_layer_set_font(s_month_layer, fonts_get_system_font(FONT_KEY_GOTHIC_14_BOLD));
  text_layer_set_text_color(s_month_layer, GColorWhite);
  text_layer_set_background_color(s_month_layer, GColorClear);

  if(config_get(PERSIST_KEY_DAY)) {
    layer_add_child(window_layer, text_layer_get_layer(s_day_in_month_layer));
  }
  if(config_get(PERSIST_KEY_DATE)) {
    layer_add_child(window_layer, text_layer_get_layer(s_weekday_layer));
    layer_add_child(window_layer, text_layer_get_layer(s_month_layer));
  }


  //Layer de bateria
  BatteryChargeState initial = battery_state_service_peek();
  battery_level = initial.charge_percent;
  battery_plugged = initial.is_plugged;
  s_battery_layer = layer_create(GRect(0,20,144,12));
  layer_set_update_proc(s_battery_layer, &battery_layer_update_callback);
  layer_add_child(window_layer, s_battery_layer);

  // Layer del clima
  s_weather_layer = text_layer_create(GRect(0, 90, 144, 50));
  text_layer_set_background_color(s_weather_layer, GColorClear);
  text_layer_set_text_color(s_weather_layer, GColorWhite);
  text_layer_set_font(s_weather_layer, fonts_get_system_font(FONT_KEY_GOTHIC_14));
  text_layer_set_text_alignment(s_weather_layer, GTextAlignmentCenter);
  text_layer_set_text(s_weather_layer, "--");
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_weather_layer));


  s_canvas_layer = layer_create(bounds);
  layer_set_update_proc(s_canvas_layer, draw_proc);
  layer_add_child(window_layer, s_canvas_layer);
}

static void window_unload(Window *window) {
  gbitmap_destroy(s_background_bitmap);
  gbitmap_destroy(icon_battery);
  gbitmap_destroy(icon_battery_low);
  gbitmap_destroy(icon_battery_charge);
  gbitmap_destroy(icon_bt_disconected);
  layer_destroy(s_canvas_layer);
  layer_destroy(s_bg_layer);
  layer_destroy(s_battery_layer);

  text_layer_destroy(s_weekday_layer);
  text_layer_destroy(s_day_in_month_layer);
  text_layer_destroy(s_month_layer);
  text_layer_destroy(s_weather_layer);

  gpath_destroy(minute_hand_path);
  gpath_destroy(minute_hand_path_inner);
  gpath_destroy(hour_hand_path_inner);
  gpath_destroy(hour_hand_path);

  // Self destroying
  window_destroy(s_main_window);
}

void main_window_push() {
  if(config_get(PERSIST_KEY_SECOND_HAND)) {
    tick_timer_service_subscribe(SECOND_UNIT, tick_handler);
  } else {
    tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);
  }
  battery_state_service_subscribe (&battery_state_handler);

  s_main_window = window_create();
  window_set_background_color(s_main_window, GColorBlack);
  window_set_window_handlers(s_main_window, (WindowHandlers) {
    .load = window_load,
    .unload = window_unload,
  });
  window_stack_push(s_main_window, true);

  // TODO make charging pref
  //battery_state_service_subscribe(batt_handler);

  time_t t = time(NULL);
  struct tm *tm_now = localtime(&t);
  tick_handler(tm_now, SECOND_UNIT);
  s_animating = false;
  app_timer_register(1000, anim_handler, NULL);

  if(config_get(PERSIST_KEY_BT)) {
    bluetooth_connection_service_subscribe(bt_handler);
    bt_handler(bluetooth_connection_service_peek());
  }
}

static void inbox_received_callback(DictionaryIterator *iterator, void *context) {

   // Store incoming information
  static char temperature_buffer[8];
  static char conditions_buffer[32];
  static char humidity_buffer[32];
  static char wind_speed_buffer[32];
  static char sunrise_buffer[6];
  static char sunset_buffer[6];
  static char weather_layer_buffer[64];
 // Read first item
  Tuple *t = dict_read_first(iterator);

  // For all items
  while(t != NULL) {
    // Which key was received?
    switch(t->key) {
    case KEY_TEMPERATURE:
            snprintf(temperature_buffer, sizeof(temperature_buffer), "%dC", (int)t->value->int32);

      break;
    case KEY_CONDITIONS:
              snprintf(conditions_buffer, sizeof(conditions_buffer), "%s", t->value->cstring);

      break;
     case KEY_HUMIDITY:
              snprintf(humidity_buffer, sizeof(humidity_buffer), "%d %%", (int)t->value->int32);

      break;
     case KEY_WIND:
              snprintf(wind_speed_buffer, sizeof(wind_speed_buffer), "%s kph", t->value->cstring);

      break; 
       case KEY_SUNRISE:
              snprintf(sunrise_buffer, sizeof(sunrise_buffer), "%s", t->value->cstring);

      break;   
       case KEY_SUNSET:
              snprintf(sunset_buffer, sizeof(sunset_buffer), "%s", t->value->cstring);

      break;      
    default:
      APP_LOG(APP_LOG_LEVEL_ERROR, "Key %d not recognized!", (int)t->key);
      break;
    }

    // Look for next item
    t = dict_read_next(iterator);
  }

 snprintf(weather_layer_buffer, sizeof(weather_layer_buffer), "%s %s\n%s %s\n%s - %s", temperature_buffer, conditions_buffer,humidity_buffer,wind_speed_buffer,sunrise_buffer, sunset_buffer);
  text_layer_set_text(s_weather_layer, weather_layer_buffer);
 /* Tuple *t = dict_read_first(iter);
  while(t) {
    persist_write_bool(t->key, strcmp(t->value->cstring, "true") == 0 ? true : false);
    t = dict_read_next(iter);
  }

  // Refresh live store
  config_init();
  vibes_short_pulse();
  */
}

static void inbox_dropped_callback(AppMessageResult reason, void *context) {
  APP_LOG(APP_LOG_LEVEL_ERROR, "Message dropped!");
}

static void outbox_failed_callback(DictionaryIterator *iterator, AppMessageResult reason, void *context) {
  APP_LOG(APP_LOG_LEVEL_ERROR, "Outbox send failed!");
}

static void outbox_sent_callback(DictionaryIterator *iterator, void *context) {
  APP_LOG(APP_LOG_LEVEL_INFO, "Outbox send success!");
}


void comm_init() {
  app_message_register_inbox_received(inbox_received_callback);
  app_message_register_inbox_dropped(inbox_dropped_callback);
  app_message_register_outbox_failed(outbox_failed_callback);
  app_message_register_outbox_sent(outbox_sent_callback);

  app_message_open(app_message_inbox_size_maximum(), app_message_outbox_size_maximum());

}