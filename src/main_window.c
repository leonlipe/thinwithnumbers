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
#define KEY_DATE 6
#define KEY_DAY 7
#define KEY_BT 8
#define KEY_BATTERY 9
#define KEY_SECOND_HAND 10
#define BACKTYPE 11
#define MARGIN 12
#define HAND_LENGTH_SEC 13
#define HAND_LENGTH_SEC_INVERSE 14
#define HAND_LENGTH_MIN 15
#define HAND_LENGTH_HOUR 16
#define HAND_TYPE 17
#define TEMPERATURE 18
#define CONDITIONS 19
#define HUMIDITY 20
#define WIND 21
#define SUNSET 22
#define SUNRISE 23
#define DIGITALTIME 24
#define INVERTED 25
#define NUMBERS 26



const GPathInfo MINUTE_HAND_PATH_POINTS_FLAT = { 4, (GPoint[] ) { 
  { HOUR_HAND_TICKNESS*-1, 0 },
  { HOUR_HAND_TICKNESS, 0 }, 
  { HOUR_HAND_TICKNESS, -70 }, 
  { HOUR_HAND_TICKNESS*-1, -70 }, 
} };

const GPathInfo HOUR_HAND_PATH_POINTS_FLAT = { 4, (GPoint[] ) { 
  { HOUR_HAND_TICKNESS*-1, 0 },
  { HOUR_HAND_TICKNESS, 0 }, 
  { HOUR_HAND_TICKNESS, -50 }, 
  { HOUR_HAND_TICKNESS*-1, -50 }, 
} };

static Window *s_main_window;
static Layer *s_canvas_layer, *s_bg_layer, *s_battery_layer;
static TextLayer *s_weekday_layer, *s_day_in_month_layer, *s_month_layer, *s_weather_hum_layer , *s_weather_sun_layer,*s_weather_temp_layer, *s_time_layer, *s_12_layer, *s_9_layer, *s_6_layer;
static TextLayer *s_m_5_layer, *s_m_10_layer,  *s_m_20_layer,  *s_m_25_layer,  *s_m_35_layer,  *s_m_40_layer,  *s_m_50_layer,  *s_m_55_layer;
static InverterLayer *s_inverter_layer;

static GBitmap *icon_battery, *icon_battery_low, *icon_battery_blank,*icon_battery_charge,*icon_bt_disconected;


static GPath *minute_hand_path, *hour_hand_path;

static Time s_last_time;
static uint8_t s_last_unit_weather_change;
static char s_weekday_buffer[8], s_month_buffer[8], s_day_in_month_buffer[3], s_time_buffer[12];
static bool s_connected;

static uint8_t battery_level;
static bool battery_plugged;
static GFont s_time_font, s_visitor_14_font,s_visitor_20_font,s_visitor_24_font;


static int32_t getMarkSize(int h){
  int32_t resultado = 75;
  switch(h){
    case 0  :
       resultado = 80;
       break; 
    case 1  :
       resultado = 95;
       break; 
    case 2  :
       resultado = 78;
       break;    
    case 3  :
       resultado = 68;
       break;    
    case 4  :
       resultado = 78;
       break;    
    case 5  :
       resultado = 93;
       break;    
    case 6  :
       resultado = 80;
       break;    
    case 7  :
       resultado = 93;
       break;    
    case 8  :
       resultado = 79;
       break;    
    case 9  :
       resultado = 69;
       break;    
    case 10  :
       resultado = 79 ;
       break;    
    case 11  :
       resultado = 95;
       break;       
} 
return resultado;
}


static void bg_update_proc(Layer *layer, GContext *ctx) {
  GRect bounds = layer_get_bounds(layer);
  GPoint center = grect_center_point(&bounds);
  graphics_context_set_stroke_color(ctx, GColorWhite);

  
  if (config_get(PERSIST_BACKTYPE) == 1){

    for(int h = 0; h < 60; h++) {   
          GPoint point = (GPoint) {
            //int32_t second_angle = TRIG_MAX_ANGLE * t.tm_sec / 60;
            //secondHand.x = (sin_lookup(second_angle) * secondHandLength / TRIG_MAX_RATIO) + center.x;
            .x = (int16_t)(sin_lookup(TRIG_MAX_ANGLE * h / 60) * (int32_t)(3 * config_get(PERSIST_HAND_LENGTH_SEC)) / TRIG_MAX_RATIO) + center.x,
            //secondHand.y = (-cos_lookup(second_angle) * secondHandLength / TRIG_MAX_RATIO) + center.y;
            .y = (int16_t)(-cos_lookup(TRIG_MAX_ANGLE * h / 60) * (int32_t)(3 * config_get(PERSIST_HAND_LENGTH_SEC)) / TRIG_MAX_RATIO) + center.y,
          };
          GPoint point02 = (GPoint) {
            //int32_t second_angle = TRIG_MAX_ANGLE * t.tm_sec / 60;
            //secondHand.x = (sin_lookup(second_angle) * secondHandLength / TRIG_MAX_RATIO) + center.x;
            .x = (int16_t)(sin_lookup(TRIG_MAX_ANGLE * h / 60) * (int32_t)(60) / TRIG_MAX_RATIO) + center.x,
            //secondHand.y = (-cos_lookup(second_angle) * secondHandLength / TRIG_MAX_RATIO) + center.y;
            .y = (int16_t)(-cos_lookup(TRIG_MAX_ANGLE * h / 60) * (int32_t)(60) / TRIG_MAX_RATIO) + center.y,
          };               
            #if defined(ANTIALIASING) && defined(PBL_COLOR)
                    graphics_draw_line_antialiased(ctx, GPoint(point02.x , point02.y ), GPoint(point.x, point.y), GColorWhite);
            #else
                  graphics_draw_line(ctx, GPoint(point02.x, point02.y), GPoint(point.x, point.y));
            #endif
      
    }

    // Make markers
    graphics_context_set_fill_color(ctx, GColorBlack);
    graphics_fill_rect(ctx, GRect(1, 1, bounds.size.w - (2), bounds.size.h - (2)), 0, GCornerNone);
  }


graphics_context_set_stroke_color(ctx, GColorWhite);

if (config_get(PERSIST_BACKTYPE) == 0 || config_get(PERSIST_BACKTYPE) == 1){
  for(int h = 0; h < 12; h++) { 



        GPoint point = (GPoint) {
          //int32_t second_angle = TRIG_MAX_ANGLE * t.tm_sec / 60;
          //secondHand.x = (sin_lookup(second_angle) * secondHandLength / TRIG_MAX_RATIO) + center.x;
          .x = (int16_t)(sin_lookup(TRIG_MAX_ANGLE * h / 12) * (int32_t)(3 * config_get(PERSIST_HAND_LENGTH_SEC)) / TRIG_MAX_RATIO) + center.x,
          //secondHand.y = (-cos_lookup(second_angle) * secondHandLength / TRIG_MAX_RATIO) + center.y;
          .y = (int16_t)(-cos_lookup(TRIG_MAX_ANGLE * h / 12) * (int32_t)(3 * config_get(PERSIST_HAND_LENGTH_SEC)) / TRIG_MAX_RATIO) + center.y,
        };

        GPoint point02 = (GPoint) {
          //int32_t second_angle = TRIG_MAX_ANGLE * t.tm_sec / 60;
          //secondHand.x = (sin_lookup(second_angle) * secondHandLength / TRIG_MAX_RATIO) + center.x;
          .x = (int16_t)(sin_lookup(TRIG_MAX_ANGLE * h / 12) * getMarkSize(h) / TRIG_MAX_RATIO) + center.x,
          //secondHand.y = (-cos_lookup(second_angle) * secondHandLength / TRIG_MAX_RATIO) + center.y;
          .y = (int16_t)(-cos_lookup(TRIG_MAX_ANGLE * h / 12) * getMarkSize(h) / TRIG_MAX_RATIO) + center.y,
        };

      /*  GPoint point03 = (GPoint) {
          //int32_t second_angle = TRIG_MAX_ANGLE * t.tm_sec / 60;
          //secondHand.x = (sin_lookup(second_angle) * secondHandLength / TRIG_MAX_RATIO) + center.x;
          .x = (int16_t)(sin_lookup(TRIG_MAX_ANGLE * h / 12) * 72 / TRIG_MAX_RATIO) + center.x,
          //secondHand.y = (-cos_lookup(second_angle) * secondHandLength / TRIG_MAX_RATIO) + center.y;
          .y = (int16_t)(-cos_lookup(TRIG_MAX_ANGLE * h / 12) * 72 / TRIG_MAX_RATIO) + center.y,
        };

         GPoint point04 = (GPoint) {
          //int32_t second_angle = TRIG_MAX_ANGLE * t.tm_sec / 60;
          //secondHand.x = (sin_lookup(second_angle) * secondHandLength / TRIG_MAX_RATIO) + center.x;
          .x = (int16_t)(sin_lookup(TRIG_MAX_ANGLE * h / 12) * 77 / TRIG_MAX_RATIO) + center.x,
          //secondHand.y = (-cos_lookup(second_angle) * secondHandLength / TRIG_MAX_RATIO) + center.y;
          .y = (int16_t)(-cos_lookup(TRIG_MAX_ANGLE * h / 12) * 77 / TRIG_MAX_RATIO) + center.y,
        };*/
              
            for(int y = 0; y < THICKNESSMARKS; y++) {
              for(int x = 0; x < THICKNESSMARKS; x++) {
                graphics_draw_line(ctx, GPoint(point02.x + x, point02.y + y), GPoint(point.x + x, point.y + y));
               // graphics_draw_line(ctx, GPoint(point03.x + x, point03.y + y), GPoint(point04.x + x, point04.y + y));
              }
            }
    
  }

  // Make markers
  graphics_context_set_fill_color(ctx, GColorBlack);
  //graphics_fill_rect(ctx, GRect(config_get(PERSIST_MARGIN), config_get(PERSIST_MARGIN), bounds.size.w - (2*config_get(PERSIST_MARGIN)), bounds.size.h - (2*config_get(PERSIST_MARGIN))), 0, GCornerNone);
} 
 
  if (config_get(PERSIST_KEY_DAY)==1){
    graphics_context_set_fill_color(ctx, GColorWhite);
    graphics_fill_rect(ctx, GRect(100, 75, 25,22), 1, GCornersAll);  
  }
  

}



static GPoint make_hand_point(int quantity, int intervals, int len, GPoint center) {
  return (GPoint) {
    .x = (int16_t)(sin_lookup(TRIG_MAX_ANGLE * quantity / intervals) * (int32_t)len / TRIG_MAX_RATIO) + center.x,
    .y = (int16_t)(-cos_lookup(TRIG_MAX_ANGLE * quantity / intervals) * (int32_t)len / TRIG_MAX_RATIO) + center.y,
  };
}

static void handsSeparators(GContext *ctx, int32_t units){
  if (units == 0){
    graphics_context_set_stroke_color(ctx, GColorBlack);
    graphics_draw_line(ctx, GPoint(72, 13), GPoint(72+THICKNESS, 13));
  }
  if (units == 15){
    graphics_context_set_stroke_color(ctx, GColorBlack);
    graphics_draw_line(ctx, GPoint(135, 83), GPoint(135, 83+THICKNESS));
  }
  if (units == 30){
    graphics_context_set_stroke_color(ctx, GColorBlack);
    graphics_draw_line(ctx, GPoint(72, 157), GPoint(72+THICKNESS, 157));
  }
  if (units == 45){
    graphics_context_set_stroke_color(ctx, GColorBlack);
    graphics_draw_line(ctx, GPoint(2, 83), GPoint(2, 83+THICKNESS));
  }
}
 
int inverse_hand(int actual_time){
  int new_time = actual_time + 30;
  if (new_time > 60){
    new_time -= 60;
  }
  return new_time;
}  

int inverse_hand_hour(int actual_time){
  int new_time = actual_time + 6;
  if (new_time > 12){
    new_time -= 12;
  }
  return new_time;  
}


static void draw_proc(Layer *layer, GContext *ctx) {
  GRect bounds = layer_get_bounds(layer);
  GPoint center = grect_center_point(&bounds);

  Time now = s_last_time;


 

  // Plot hand ends
  GPoint second_hand_inverse = make_hand_point(inverse_hand(now.seconds), 60, config_get(PERSIST_HAND_LENGTH_SEC_INVERSE), center);
  GPoint second_hand_inverse_circle = make_hand_point(inverse_hand(now.seconds), 60, config_get(PERSIST_HAND_LENGTH_SEC_INVERSE)+4, center);
  GPoint second_hand_long = make_hand_point(now.seconds, 60, config_get(PERSIST_HAND_LENGTH_SEC), center);
  GPoint minute_hand_long = make_hand_point(now.minutes, 60, config_get(PERSIST_HAND_LENGTH_MIN), center);
  //GPoint second_hand_short = make_hand_point(now.seconds, 60, (config_get(PERSIST_HAND_LENGTH_SEC) - config_get(PERSIST_MARGIN) + 2), center);
  GPoint minute_hand_inverse = make_hand_point(inverse_hand(now.minutes), 60, 10, center);

  // Adjust for minutes through the hour
  float minute_angle = TRIG_MAX_ANGLE * now.minutes / 60; //now.minutes
  float hour_angle = TRIG_MAX_ANGLE * now.hours / 12; //now.hours
  hour_angle += (minute_angle / TRIG_MAX_ANGLE) * (TRIG_MAX_ANGLE / 12);
  float hour_angle_inverse = TRIG_MAX_ANGLE * inverse_hand_hour(now.hours) / 12; //now.hours
  hour_angle_inverse += (minute_angle / TRIG_MAX_ANGLE) * (TRIG_MAX_ANGLE / 12);

  // Hour is more accurate
  GPoint hour_hand_long = (GPoint) {
    .x = (int16_t)(sin_lookup(hour_angle) * (int32_t)config_get(PERSIST_HAND_LENGTH_HOUR) / TRIG_MAX_RATIO) + center.x,
    .y = (int16_t)(-cos_lookup(hour_angle) * (int32_t)config_get(PERSIST_HAND_LENGTH_HOUR) / TRIG_MAX_RATIO) + center.y,
  };
  GPoint hour_hand_inverse = (GPoint) {
    .x = (int16_t)(sin_lookup(hour_angle_inverse) * (int32_t)(8) / TRIG_MAX_RATIO) + center.x,
    .y = (int16_t)(-cos_lookup(hour_angle_inverse) * (int32_t)(8) / TRIG_MAX_RATIO) + center.y,
  };


 if (config_get(PERSIST_HAND_TYPE) == 2){
  // Draw hands
 // int y = 0;
  for(int y = 0; y < THICKNESS; y++) {
    for(int x = 0; x < THICKNESS; x++) {
      graphics_context_set_stroke_color(ctx, GColorWhite);
      graphics_draw_line(ctx, GPoint(center.x + x, center.y + y), GPoint(minute_hand_long.x + x, minute_hand_long.y + y));
      graphics_draw_line(ctx, GPoint(center.x + x, center.y + y), GPoint(minute_hand_inverse.x + x, minute_hand_inverse.y + y));
      graphics_draw_line(ctx, GPoint(center.x + x, center.y + y), GPoint(hour_hand_long.x + x, hour_hand_long.y + y));
      graphics_draw_line(ctx, GPoint(center.x + x, center.y + y), GPoint(hour_hand_inverse.x + x, hour_hand_inverse.y + y));
      //graphics_draw_line(ctx, GPoint(minute_hand_short.x + x, minute_hand_short.y + y), GPoint(minute_hand_long.x + x, minute_hand_long.y + y));
      //graphics_draw_line(ctx, GPoint(hour_hand_short.x + x, hour_hand_short.y + y), GPoint(hour_hand_long.x + x, hour_hand_long.y + y));
    }
  }
    /*for(int y = 1; y < 3; y++) {
    for(int x = 1; x < 3; x++) {
      graphics_context_set_stroke_color(ctx, GColorWhite);
      graphics_draw_line(ctx, GPoint(center.x + x, center.y + y), GPoint(minute_hand_short.x + x, minute_hand_short.y + y));
      graphics_draw_line(ctx, GPoint(center.x + x, center.y + y), GPoint(hour_hand_short.x + x, hour_hand_short.y + y));
    }
  }*/

 // handsSeparators(ctx, now.minutes);
}else {
        gpath_rotate_to(hour_hand_path, hour_angle);
        graphics_context_set_fill_color(ctx, GColorWhite);
        graphics_context_set_stroke_color(ctx, GColorBlack);
        gpath_draw_filled(ctx, hour_hand_path);
        gpath_draw_outline(ctx, hour_hand_path);   

        // Draw minute hand with path
        gpath_rotate_to(minute_hand_path, minute_angle);
        graphics_context_set_fill_color(ctx, GColorWhite);
        graphics_context_set_stroke_color(ctx, GColorBlack);
        gpath_draw_filled(ctx, minute_hand_path);
        gpath_draw_outline(ctx, minute_hand_path);

       
}
  // Draw seconds hand
                   
  if(config_get(PERSIST_KEY_SECOND_HAND)) {
    for(int y = 0; y < THICKNESS_SECONDS; y++) {
      for(int x = 0; x < THICKNESS_SECONDS; x++) {
       
        graphics_context_set_stroke_color(ctx, GColorWhite);
        graphics_draw_line(ctx, GPoint(center.x + x, center.y+y ), GPoint(second_hand_inverse.x + x, second_hand_inverse.y+y ));
        graphics_draw_line(ctx, GPoint(center.x + x, center.y+y ), GPoint(second_hand_long.x + x, second_hand_long.y+y ));
      }
    }
  //  handsSeparators(ctx, now.seconds);
  }
  // Draw circle for seconds hand
 graphics_draw_circle(ctx, GPoint(second_hand_inverse_circle.x + 1, second_hand_inverse_circle.y + 1), 4);
 graphics_context_set_stroke_color(ctx, GColorBlack);
 graphics_draw_circle(ctx, GPoint(second_hand_inverse_circle.x + 1, second_hand_inverse_circle.y + 1), 3);
 graphics_draw_circle(ctx, GPoint(second_hand_inverse_circle.x + 1, second_hand_inverse_circle.y + 1), 5);
 graphics_context_set_stroke_color(ctx, GColorWhite);


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
   // graphics_context_set_fill_color(ctx, GColorBlack);
   // graphics_context_set_stroke_color(ctx, GColorWhite);
    graphics_draw_circle(ctx, GPoint(center.x + 1, center.y + 1), 4);
    graphics_fill_circle(ctx, GPoint(center.x + 1, center.y + 1), 3);
    graphics_context_set_fill_color(ctx, GColorBlack);
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



static void tick_handler(struct tm *tick_time, TimeUnits changed) {
  s_last_time.days = tick_time->tm_mday;
  s_last_time.hours = tick_time->tm_hour;
  s_last_time.minutes = tick_time->tm_min;
  s_last_time.seconds = tick_time->tm_sec;


  /*if(s_last_time.hours > 22 || (s_last_time.hours >=0 and s_last_time.hours < 6) ) {
    tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);
  } else {
    tick_timer_service_subscribe(SECOND_UNIT, tick_handler);
  }*/

  
  snprintf(s_day_in_month_buffer, sizeof(s_day_in_month_buffer), "%d", s_last_time.days);
  strftime(s_weekday_buffer, sizeof(s_weekday_buffer), "%a", tick_time);
  strftime(s_month_buffer, sizeof(s_month_buffer), "%b", tick_time);
  if (config_get(PERSIST_DIGITALTIME)){
    strftime(s_time_buffer, sizeof(s_time_buffer), "%l:%M%P", tick_time);
  }
  text_layer_set_text(s_weekday_layer, s_weekday_buffer);
  text_layer_set_text(s_day_in_month_layer, s_day_in_month_buffer);
  text_layer_set_text(s_month_layer, s_month_buffer);
  text_layer_set_text(s_time_layer, s_time_buffer);

  // Finally
  layer_mark_dirty(s_canvas_layer);

    // Get weather update every 30 minutes
   //  APP_LOG(APP_LOG_LEVEL_INFO, "Before:");
  if(tick_time->tm_hour != s_last_unit_weather_change) {
    // APP_LOG(APP_LOG_LEVEL_INFO, "Send:");
    if (config_get(PERSIST_TEMPERATURE || config_get(PERSIST_CONDITIONS) || config_get(PERSIST_HUMIDITY
      || config_get(PERSIST_WIND || config_get(PERSIST_SUNRISE || config_get(PERSIST_SUNSET)))))){
        s_last_unit_weather_change = tick_time->tm_hour;
        // Begin dictionary
        DictionaryIterator *iter;
        app_message_outbox_begin(&iter);
        // Add a key-value pair
        dict_write_uint8(iter, 0, 0);
        // Send the message!
        app_message_outbox_send();
    }
  }
}

static void bt_handler(bool connected) {
  // Notify disconnection
/*  if(!connected && s_connected) {
    vibes_long_pulse();
  }
*/
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
      if (battery_level < 20){
        graphics_draw_bitmap_in_rect(ctx, icon_battery_low, GRect(61, 0, 24, 12));
        graphics_context_set_stroke_color(ctx, GColorBlack);
        graphics_context_set_fill_color(ctx, GColorWhite);
        graphics_fill_rect(ctx, GRect(68, 4, (uint8_t)((battery_level / 100.0) * 11.0), 4), 0, GCornerNone);
      }else{
        if(config_get(PERSIST_KEY_BATTERY)){
          graphics_draw_bitmap_in_rect(ctx, icon_battery, GRect(61, 0, 24, 12));
          graphics_context_set_stroke_color(ctx, GColorBlack);
          graphics_context_set_fill_color(ctx, GColorWhite);
          graphics_fill_rect(ctx, GRect(68, 4, (uint8_t)((battery_level / 100.0) * 11.0), 4), 0, GCornerNone);
        }
      }
        
      
    

    
  } else {
    graphics_draw_bitmap_in_rect(ctx, icon_battery_charge, GRect(61, 0, 24, 12));
  }


   if(config_get(PERSIST_KEY_BT) && !s_connected) {
      graphics_draw_bitmap_in_rect(ctx, icon_bt_disconected, GRect(42, 0, 24, 12));    
   }


}

static void window_load(Window *window) {


  s_time_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_VISITOR_20));
  s_visitor_14_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_VISITOR_14));
  s_visitor_24_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_VISITOR_24));
  s_visitor_20_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_VISITOR_20));

  icon_battery = gbitmap_create_with_resource(RESOURCE_ID_BATTERY_ICON);
  icon_battery_low = gbitmap_create_with_resource(RESOURCE_ID_BATTERY_ICON_LOW);
  icon_battery_blank = gbitmap_create_with_resource(RESOURCE_ID_BATTERY_ICON_BLANK);
  icon_battery_charge = gbitmap_create_with_resource(RESOURCE_ID_BATTERY_CHARGE);
  icon_bt_disconected = gbitmap_create_with_resource(RESOURCE_ID_BLUETOOTH_DISCONNECTED);

  
  minute_hand_path = gpath_create(&MINUTE_HAND_PATH_POINTS_FLAT);
  gpath_move_to(minute_hand_path, grect_center_point(&GRECT_FULL_WINDOW));
  hour_hand_path = gpath_create(&HOUR_HAND_PATH_POINTS_FLAT);
  gpath_move_to(hour_hand_path, grect_center_point(&GRECT_FULL_WINDOW));

  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);

  s_bg_layer = layer_create(bounds);
  layer_set_update_proc(s_bg_layer, bg_update_proc);
  layer_add_child(window_layer, s_bg_layer);

  s_weekday_layer = text_layer_create(GRect(90, 59, 44, 40));
  text_layer_set_text_alignment(s_weekday_layer, GTextAlignmentCenter);
  text_layer_set_font(s_weekday_layer, fonts_get_system_font(FONT_KEY_GOTHIC_14_BOLD));
  text_layer_set_text_color(s_weekday_layer, GColorWhite);
  text_layer_set_background_color(s_weekday_layer, GColorClear);

  s_day_in_month_layer = text_layer_create(GRect(90, 69, 44, 40));
  text_layer_set_text_alignment(s_day_in_month_layer, GTextAlignmentCenter);
  text_layer_set_font(s_day_in_month_layer, fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD));
  text_layer_set_text_color(s_day_in_month_layer, GColorBlack);
  text_layer_set_background_color(s_day_in_month_layer, GColorClear);


  

  s_month_layer = text_layer_create(GRect(90, 94, 44, 40));
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
  s_battery_layer = layer_create(GRect(0,60,144,106));
  layer_set_update_proc(s_battery_layer, &battery_layer_update_callback);
  layer_add_child(window_layer, s_battery_layer);

  // Layer del clima
  s_weather_temp_layer = text_layer_create(GRect(0, 90, 144, 50));
  text_layer_set_background_color(s_weather_temp_layer, GColorClear);
  text_layer_set_text_color(s_weather_temp_layer, GColorWhite);
  text_layer_set_font(s_weather_temp_layer, s_visitor_20_font);
  text_layer_set_text_alignment(s_weather_temp_layer, GTextAlignmentCenter);
  text_layer_set_text(s_weather_temp_layer, "");
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_weather_temp_layer));

  s_weather_hum_layer = text_layer_create(GRect(0, 110, 144, 50));
  text_layer_set_background_color(s_weather_hum_layer, GColorClear);
  text_layer_set_text_color(s_weather_hum_layer, GColorWhite);
  text_layer_set_font(s_weather_hum_layer, s_visitor_14_font);
  text_layer_set_text_alignment(s_weather_hum_layer, GTextAlignmentCenter);
  text_layer_set_text(s_weather_hum_layer, "");
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_weather_hum_layer));
 
  s_weather_sun_layer = text_layer_create(GRect(0, 120, 144, 50));
  text_layer_set_background_color(s_weather_sun_layer, GColorClear);
  text_layer_set_text_color(s_weather_sun_layer, GColorWhite);
  text_layer_set_font(s_weather_sun_layer, s_visitor_14_font);
  text_layer_set_text_alignment(s_weather_sun_layer, GTextAlignmentCenter);
  text_layer_set_text(s_weather_sun_layer, "");
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_weather_sun_layer));
  
  // Layer de los minutos

  s_m_5_layer = text_layer_create(GRect(110, 10, 85, 28));
  text_layer_set_text_alignment(s_m_5_layer, GTextAlignmentCenter);
  text_layer_set_font(s_m_5_layer, fonts_get_system_font(FONT_KEY_GOTHIC_14));
  text_layer_set_text_color(s_m_5_layer, GColorWhite);
  text_layer_set_background_color(s_m_5_layer, GColorClear);
  text_layer_set_text_alignment(s_m_5_layer, GTextAlignmentLeft);
  text_layer_set_text(s_m_5_layer, "05");
  layer_set_hidden(text_layer_get_layer(s_m_5_layer),config_get(PERSIST_MINUTES));
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_m_5_layer));

  s_m_10_layer = text_layer_create(GRect(120, 40, 85, 28));
  text_layer_set_text_alignment(s_m_10_layer, GTextAlignmentCenter);
  text_layer_set_font(s_m_10_layer, fonts_get_system_font(FONT_KEY_GOTHIC_14));
  text_layer_set_text_color(s_m_10_layer, GColorWhite);
  text_layer_set_background_color(s_m_10_layer, GColorClear);
  text_layer_set_text_alignment(s_m_10_layer, GTextAlignmentLeft);
  text_layer_set_text(s_m_10_layer, "10");
  layer_set_hidden(text_layer_get_layer(s_m_10_layer),config_get(PERSIST_MINUTES));
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_m_10_layer));

  s_m_20_layer = text_layer_create(GRect(120, 110, 85, 28));
  text_layer_set_text_alignment(s_m_20_layer, GTextAlignmentCenter);
  text_layer_set_font(s_m_20_layer, fonts_get_system_font(FONT_KEY_GOTHIC_14));
  text_layer_set_text_color(s_m_20_layer, GColorWhite);
  text_layer_set_background_color(s_m_20_layer, GColorClear);
  text_layer_set_text_alignment(s_m_20_layer, GTextAlignmentLeft);
  text_layer_set_text(s_m_20_layer, "20");
  layer_set_hidden(text_layer_get_layer(s_m_20_layer),config_get(PERSIST_MINUTES));
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_m_20_layer));

   s_m_25_layer = text_layer_create(GRect(110, 140, 85, 28));
  text_layer_set_text_alignment(s_m_25_layer, GTextAlignmentCenter);
  text_layer_set_font(s_m_25_layer, fonts_get_system_font(FONT_KEY_GOTHIC_14));
  text_layer_set_text_color(s_m_25_layer, GColorWhite);
  text_layer_set_background_color(s_m_25_layer, GColorClear);
  text_layer_set_text_alignment(s_m_25_layer, GTextAlignmentLeft);
  text_layer_set_text(s_m_25_layer, "25");
  layer_set_hidden(text_layer_get_layer(s_m_25_layer),config_get(PERSIST_MINUTES));
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_m_25_layer));

  s_m_35_layer = text_layer_create(GRect(25, 140, 85, 28));
  text_layer_set_text_alignment(s_m_35_layer, GTextAlignmentCenter);
  text_layer_set_font(s_m_35_layer, fonts_get_system_font(FONT_KEY_GOTHIC_14));
  text_layer_set_text_color(s_m_35_layer, GColorWhite);
  text_layer_set_background_color(s_m_35_layer, GColorClear);
  text_layer_set_text_alignment(s_m_35_layer, GTextAlignmentLeft);
  text_layer_set_text(s_m_35_layer, "35");
   layer_set_hidden(text_layer_get_layer(s_m_35_layer),config_get(PERSIST_MINUTES));
 layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_m_35_layer));

  s_m_40_layer = text_layer_create(GRect(15, 110, 85, 28));
  text_layer_set_text_alignment(s_m_40_layer, GTextAlignmentCenter);
  text_layer_set_font(s_m_40_layer, fonts_get_system_font(FONT_KEY_GOTHIC_14));
  text_layer_set_text_color(s_m_40_layer, GColorWhite);
  text_layer_set_background_color(s_m_40_layer, GColorClear);
  text_layer_set_text_alignment(s_m_40_layer, GTextAlignmentLeft);
  text_layer_set_text(s_m_40_layer, "40");
    layer_set_hidden(text_layer_get_layer(s_m_40_layer),config_get(PERSIST_MINUTES));
layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_m_40_layer));

  s_m_50_layer = text_layer_create(GRect(15, 40, 85, 28));
  text_layer_set_text_alignment(s_m_50_layer, GTextAlignmentCenter);
  text_layer_set_font(s_m_50_layer, fonts_get_system_font(FONT_KEY_GOTHIC_14));
  text_layer_set_text_color(s_m_50_layer, GColorWhite);
  text_layer_set_background_color(s_m_50_layer, GColorClear);
  text_layer_set_text_alignment(s_m_50_layer, GTextAlignmentLeft);
  text_layer_set_text(s_m_50_layer, "50");
   layer_set_hidden(text_layer_get_layer(s_m_50_layer),config_get(PERSIST_MINUTES));
 layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_m_50_layer));

  s_m_55_layer = text_layer_create(GRect(25, 10, 85, 28));
  text_layer_set_text_alignment(s_m_55_layer, GTextAlignmentCenter);
  text_layer_set_font(s_m_55_layer, fonts_get_system_font(FONT_KEY_GOTHIC_14));
  text_layer_set_text_color(s_m_55_layer, GColorWhite);
  text_layer_set_background_color(s_m_55_layer, GColorClear);
  text_layer_set_text_alignment(s_m_55_layer, GTextAlignmentLeft);
  text_layer_set_text(s_m_55_layer, "55");
  layer_set_hidden(text_layer_get_layer(s_m_55_layer),config_get(PERSIST_MINUTES));
 layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_m_55_layer));

  // Layer de las horas
  s_12_layer = text_layer_create(GRect(65, 10, 85, 28));
  text_layer_set_text_alignment(s_12_layer, GTextAlignmentCenter);
  text_layer_set_font(s_12_layer,fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD));
  text_layer_set_text_color(s_12_layer, GColorWhite);
  text_layer_set_background_color(s_12_layer, GColorClear);
  text_layer_set_text_alignment(s_12_layer, GTextAlignmentLeft);
  text_layer_set_text(s_12_layer, "12");
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_12_layer));

  s_6_layer = text_layer_create(GRect(70, 140, 85, 28));
  text_layer_set_text_alignment(s_6_layer, GTextAlignmentCenter);
  text_layer_set_font(s_6_layer, fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD));
  text_layer_set_text_color(s_6_layer, GColorWhite);
  text_layer_set_background_color(s_6_layer, GColorClear);
  text_layer_set_text_alignment(s_6_layer, GTextAlignmentLeft);
  text_layer_set_text(s_6_layer, "6");
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_6_layer));

  s_9_layer = text_layer_create(GRect(15, 73, 85, 28));
  text_layer_set_text_alignment(s_9_layer, GTextAlignmentCenter);
  text_layer_set_font(s_9_layer, fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD));
  text_layer_set_text_color(s_9_layer, GColorWhite);
  text_layer_set_background_color(s_9_layer, GColorClear);
  text_layer_set_text_alignment(s_9_layer, GTextAlignmentLeft);
  text_layer_set_text(s_9_layer, "9");
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_9_layer));

  if (config_get(PERSIST_NUMBERS)){
    layer_set_hidden(text_layer_get_layer(s_12_layer),false);
    if (config_get(PERSIST_DIGITALTIME)){
      layer_set_hidden(text_layer_get_layer(s_6_layer),true);
    }else{
      layer_set_hidden(text_layer_get_layer(s_6_layer),false);
    }
    layer_set_hidden(text_layer_get_layer(s_9_layer),false);
  }else{
    layer_set_hidden(text_layer_get_layer(s_12_layer),true);
    layer_set_hidden(text_layer_get_layer(s_6_layer),true);
    layer_set_hidden(text_layer_get_layer(s_9_layer),true);
  } 


  // layer de la hora
  s_time_layer = text_layer_create(GRect(0, 135, 144, 30));
  text_layer_set_background_color(s_time_layer, GColorClear);
  text_layer_set_text_color(s_time_layer, GColorWhite);
  text_layer_set_font(s_time_layer,s_time_font);
  text_layer_set_text_alignment(s_time_layer, GTextAlignmentCenter);
  text_layer_set_text(s_time_layer, "");
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_time_layer));

 

  s_canvas_layer = layer_create(bounds);
  layer_set_update_proc(s_canvas_layer, draw_proc);
  layer_add_child(window_layer, s_canvas_layer);

  s_inverter_layer = inverter_layer_create(bounds);

  if (config_get(PERSIST_INVERTED)){
    layer_set_hidden(inverter_layer_get_layer(s_inverter_layer),false);
  }else{
    layer_set_hidden(inverter_layer_get_layer(s_inverter_layer),true);
  }
  

  layer_add_child(window_layer,inverter_layer_get_layer(s_inverter_layer));
}

static void window_unload(Window *window) {
  gbitmap_destroy(icon_battery);
  gbitmap_destroy(icon_battery_low);
  gbitmap_destroy(icon_battery_blank);
  gbitmap_destroy(icon_battery_charge);
  gbitmap_destroy(icon_bt_disconected);


  fonts_unload_custom_font(s_time_font);
  fonts_unload_custom_font(s_visitor_14_font);
  fonts_unload_custom_font(s_visitor_20_font);
  fonts_unload_custom_font(s_visitor_24_font);


  layer_destroy(s_canvas_layer);
  layer_destroy(s_bg_layer);
  layer_destroy(s_battery_layer);
  inverter_layer_destroy(s_inverter_layer);

  text_layer_destroy(s_weekday_layer);
  text_layer_destroy(s_day_in_month_layer);
  text_layer_destroy(s_month_layer);
  text_layer_destroy(s_weather_temp_layer);
  text_layer_destroy(s_weather_hum_layer);
  text_layer_destroy(s_weather_sun_layer);
  text_layer_destroy(s_12_layer);
  text_layer_destroy(s_9_layer);
  text_layer_destroy(s_6_layer);
  text_layer_destroy(s_m_5_layer);
  text_layer_destroy(s_m_10_layer);
  text_layer_destroy(s_m_20_layer);
  text_layer_destroy(s_m_25_layer);
  text_layer_destroy(s_m_35_layer);
  text_layer_destroy(s_m_40_layer);
  text_layer_destroy(s_m_50_layer);
  text_layer_destroy(s_m_55_layer);
 

  gpath_destroy(minute_hand_path);
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
  static char sunrise_buffer[10];
  static char sunset_buffer[10];
  static char weather_temp_layer_buffer[20];
  static char weather_hum_layer_buffer[20];
  static char weather_sun_layer_buffer[20];
 // Read first item
  Tuple *t = dict_read_first(iterator);

  // For all items
  while(t != NULL) {
    // Which key was received?
    switch(t->key) {
    case KEY_TEMPERATURE:
            if (config_get(PERSIST_TEMPERATURE)){
              snprintf(temperature_buffer, sizeof(temperature_buffer), "%dC", (int)t->value->int32);
            }else{
              snprintf(temperature_buffer, sizeof(temperature_buffer),"%s","");
            }

      break;
    case KEY_CONDITIONS:
            if (config_get(PERSIST_CONDITIONS)){
              snprintf(conditions_buffer, sizeof(conditions_buffer), "%s", t->value->cstring);
            }else{
              snprintf(conditions_buffer, sizeof(conditions_buffer), "%s","");
            }

      break;
     case KEY_HUMIDITY:
            if (config_get(PERSIST_HUMIDITY)){
              snprintf(humidity_buffer, sizeof(humidity_buffer), "%d%%", (int)t->value->int32);
            }else{
              snprintf(humidity_buffer, sizeof(humidity_buffer), "%s","");
            }

      break;
     case KEY_WIND:
            if (config_get(PERSIST_WIND)){
              snprintf(wind_speed_buffer, sizeof(wind_speed_buffer), "%s kph", t->value->cstring);
            }else{
              snprintf(wind_speed_buffer, sizeof(wind_speed_buffer), "%s","");
            }

      break; 
       case KEY_SUNRISE:
            if (config_get(PERSIST_SUNRISE)){
              snprintf(sunrise_buffer, sizeof(sunrise_buffer), "%s", t->value->cstring);
            }else{
              snprintf(sunrise_buffer, sizeof(sunrise_buffer), "%s","");
            }

      break;   
       case KEY_SUNSET:
            if (config_get(PERSIST_SUNSET)){
              snprintf(sunset_buffer, sizeof(sunset_buffer), "%s", t->value->cstring);
            }else{
              snprintf(sunset_buffer, sizeof(sunset_buffer), "%s","");
            }
      break;      
      case KEY_DATE:
              config_set(PERSIST_KEY_DATE,(int)t->value->int32);
      break;
       case KEY_DAY:
              config_set(PERSIST_KEY_DAY, (int)t->value->int32);              
      break; 
       case KEY_BT:
              config_set(PERSIST_KEY_BT, (int)t->value->int32);              
      break;   
       case KEY_BATTERY:
              config_set(PERSIST_KEY_BATTERY, (int)t->value->int32);              
      break;  
       case KEY_SECOND_HAND:
              config_set(PERSIST_KEY_SECOND_HAND, (int)t->value->int32);              
      break;
       case BACKTYPE:
              if (config_get(PERSIST_BACKTYPE) != (int)t->value->int32){
                layer_mark_dirty(s_bg_layer);
              }
              config_set(PERSIST_BACKTYPE, (int)t->value->int32);              
      break;  
       case MARGIN:
              config_set(PERSIST_MARGIN, (int)t->value->int32);
      break;     
       case HAND_LENGTH_SEC:
              config_set(PERSIST_HAND_LENGTH_SEC, (int)t->value->int32);                        
      break;         
       case HAND_LENGTH_SEC_INVERSE:
              config_set(PERSIST_HAND_LENGTH_SEC_INVERSE, (int)t->value->int32);                        
      break;         
       case HAND_LENGTH_MIN:
              config_set(PERSIST_HAND_LENGTH_MIN, (int)t->value->int32);                        
      break;         
       case HAND_LENGTH_HOUR:
              config_set(PERSIST_HAND_LENGTH_HOUR, (int)t->value->int32);     

      break;     
      case HAND_TYPE:
              config_set(PERSIST_HAND_TYPE, (int)t->value->int32);     

      break;   
      case TEMPERATURE:
              config_set(PERSIST_TEMPERATURE, (int)t->value->int32);     

      break;   
      case CONDITIONS:
              config_set(PERSIST_CONDITIONS, (int)t->value->int32);     

      break;   
      case HUMIDITY:
              config_set(PERSIST_HUMIDITY, (int)t->value->int32);     

      break;   
      case WIND:
              config_set(PERSIST_WIND, (int)t->value->int32);     

      break;   
      case SUNSET:
              config_set(PERSIST_SUNSET, (int)t->value->int32);     

      break;   
      case SUNRISE:
              config_set(PERSIST_SUNRISE, (int)t->value->int32);     

      break;   
      case DIGITALTIME:
              config_set(PERSIST_DIGITALTIME, (int)t->value->int32);     

      break;   
      case INVERTED:
              config_set(PERSIST_INVERTED, (int)t->value->int32);     

      break;
      case NUMBERS:
              config_set(PERSIST_NUMBERS, (int)t->value->int32);     

      break;


      default:
        APP_LOG(APP_LOG_LEVEL_ERROR, "Key %d not recognized!", (int)t->key);
      break;
    }



      
    // Look for next item
    t = dict_read_next(iterator);
  }

  snprintf(weather_temp_layer_buffer, sizeof(weather_temp_layer_buffer), "%s", temperature_buffer);
  text_layer_set_text(s_weather_temp_layer, weather_temp_layer_buffer);
  snprintf(weather_hum_layer_buffer, sizeof(weather_hum_layer_buffer), "Hum. %s", humidity_buffer);
  text_layer_set_text(s_weather_hum_layer, weather_hum_layer_buffer);
  snprintf(weather_sun_layer_buffer, sizeof(weather_sun_layer_buffer), "%s %s", sunrise_buffer, sunset_buffer);
  text_layer_set_text(s_weather_sun_layer, weather_sun_layer_buffer);
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

void main_init(){
  s_last_unit_weather_change = 0;
   

}