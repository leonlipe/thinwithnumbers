#include "main_window.h"
#include <pebble-generic-weather/pebble-generic-weather.h>
#include <pebble-events/pebble-events.h>

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
#define CELCIUS 27
#define POLLTIME 28
#define KEY_TEMPERATUREF 29
#define KEY_CONDITIONS_ID 30
#define KEY_CONDITIONS_ICON 31
#define READY 102
#define KEY_weatherApi 101
#define KEY_weatherProvider 100

#define DEBUG false

static GenericWeatherStatus weather_last_status;


static Window *s_main_window;
static Layer *s_canvas_layer, *s_bg_layer, *s_battery_layer;
static TextLayer *s_weekday_layer, *s_day_in_month_layer, *s_month_layer, *s_weather_hum_layer , *s_weather_sun_layer,*s_weather_temp_layer,  *s_12_layer, *s_9_layer, *s_6_layer;
static TextLayer *s_m_5_layer, *s_m_10_layer,  *s_m_20_layer,  *s_m_25_layer,  *s_m_35_layer,  *s_m_40_layer,  *s_m_50_layer,  *s_m_55_layer, *s_weather_icon;

static GBitmap *icon_battery, *icon_battery_low, *icon_battery_charge,*icon_bt_disconected;

static Time s_last_time;
static unsigned int s_last_unit_weather_change = 0;
static char s_weekday_buffer[8], s_month_buffer[8], s_day_in_month_buffer[3];
static bool s_connected;

static uint8_t battery_level;
static bool battery_plugged;
static GFont s_visitor_14_font,s_visitor_20_font, s_weather_12_font;


static void clear_weather_text(){
  snprintf(temp_text, sizeof(temp_text), "%s", "");
  snprintf(sun_text, sizeof(sun_text), "%s", "");
  snprintf(hum_text, sizeof(hum_text),  "%s", "");
  snprintf(cond_icon_text, sizeof(cond_icon_text),  "%s", "");

}

static char * obtain_weather_icon(int code, int day){
  char * resultado;
    resultado = "\uf03d";
  //if (code >= 200 && code <300){
  switch (code){
    case  GenericWeatherConditionClearSky:
      if (day){
        resultado = "\uf00d";
      }else{
        resultado = "\uf02e";
      }
    break;
  case GenericWeatherConditionFewClouds:
      if (day){
       resultado = "\uf002";
      }else{
        resultado = "\uf031";
      }
    break;
  case GenericWeatherConditionScatteredClouds:
       
    resultado = "\uf041";
  
    break;
  case GenericWeatherConditionBrokenClouds:
    resultado = "\uf013";
      
    break;
  case GenericWeatherConditionShowerRain:
       if (day){
         resultado="\uf019";
        }else{
          resultado="\uf019";
        }
  
    break;
  case GenericWeatherConditionRain:
        if (day){
          resultado = "\uf009";
        }else{
          resultado = "\uf026";
        }
    break;
  case GenericWeatherConditionThunderstorm:
      if (day){
          resultado="\uf01e";
        }else{
          resultado="\uf01e";
        }
    break;
  case GenericWeatherConditionSnow:
        if (day){
            resultado = "\uf01b";
          }else{
            resultado = "\uf01b";
          }
    break;
  case GenericWeatherConditionMist:
        if (day){
          resultado = "\uf014";
        }else{
          resultado = "\uf014";
        }
    break;
  case GenericWeatherConditionUnknown:
       resultado = "\uf03d";
    break;
  default:
      resultado = "\uf03d";



  }

  
  return resultado;
  
}


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


static void weather_callback(GenericWeatherInfo *info, GenericWeatherStatus status){
  char sunset[10];
  char sunrise[10];
  if (DEBUG)
        APP_LOG(APP_LOG_LEVEL_DEBUG, "API:%s",weatherApi);
 
  clear_weather_text();
  switch(status) {
    case GenericWeatherStatusAvailable:
    {
      weather_last_status = GenericWeatherStatusAvailable;  
      if (DEBUG)
        APP_LOG(APP_LOG_LEVEL_DEBUG, "Weather units config: %d",config_get(PERSIST_CELCIUS));
      if (config_get(PERSIST_CELCIUS)){
        snprintf(temp_text, sizeof(temp_text),"%dC",info->temp_c);
      }else{
        snprintf(temp_text, sizeof(temp_text),"%dF",info->temp_f);
      }

       // snprintf(sunset, sizeof(sunset), "%02d:%02d", hour, minute, second);
       if (config_get(PERSIST_SUNSET))
         strftime(sunset, sizeof(sunset), clock_is_24h_style() ? "%H:%M" : "%I:%M", localtime(&info->timesunset));
       if (config_get(PERSIST_SUNRISE))
         strftime(sunrise, sizeof(sunrise), clock_is_24h_style() ? "%H:%M" : "%I:%M", localtime(&info->timesunrise));
       snprintf(sun_text, sizeof(sun_text),"%s %s",sunrise,sunset);
       snprintf(hum_text, sizeof(hum_text), "%s", info->name);
       snprintf(cond_icon_text, sizeof(cond_icon_text), "%s", obtain_weather_icon(info->condition, info->day));


      if (DEBUG)
        APP_LOG(APP_LOG_LEVEL_DEBUG, "Generic weather fetch callback: %s",temp_text);
    }
      break;
    case GenericWeatherStatusNotYetFetched:
    weather_last_status = GenericWeatherStatusNotYetFetched;  
      if (DEBUG)
        APP_LOG(APP_LOG_LEVEL_DEBUG, "GenericWeatherStatusNotYetFetched");
      snprintf(temp_text, sizeof(temp_text),"%s","");
      //text_layer_set_text(s_text_layer, "GenericWeatherStatusNotYetFetched");
      break;
    case GenericWeatherStatusBluetoothDisconnected:
    weather_last_status = GenericWeatherStatusBluetoothDisconnected;  
    if (DEBUG)
        APP_LOG(APP_LOG_LEVEL_DEBUG, "GenericWeatherStatusBluetoothDisconnected");
      snprintf(temp_text, sizeof(temp_text),"%s","blu");
      //text_layer_set_text(s_text_layer, "GenericWeatherStatusBluetoothDisconnected");
      break;
    case GenericWeatherStatusPending:
    weather_last_status = GenericWeatherStatusPending;  
    if (DEBUG)
        APP_LOG(APP_LOG_LEVEL_DEBUG, "GenericWeatherStatusPending");
      snprintf(temp_text, sizeof(temp_text),"%s","");
      //text_layer_set_text(s_text_layer, "GenericWeatherStatusPending");
      break;
    case GenericWeatherStatusFailed:
    weather_last_status = GenericWeatherStatusFailed;  
    if (DEBUG)
        APP_LOG(APP_LOG_LEVEL_DEBUG, "GenericWeatherStatusFailed");
      snprintf(temp_text, sizeof(temp_text),"%s","");
      //text_layer_set_text(s_text_layer, "GenericWeatherStatusFailed");
      break;
    case GenericWeatherStatusBadKey:
    weather_last_status = GenericWeatherStatusBadKey;  
    if (DEBUG)
        APP_LOG(APP_LOG_LEVEL_DEBUG, "GenericWeatherStatusBadKey");
        snprintf(temp_text, sizeof(temp_text),"%s","");
      //text_layer_set_text(s_text_layer, "GenericWeatherStatusBadKey");
      break;
    case GenericWeatherStatusLocationUnavailable:
    weather_last_status = GenericWeatherStatusLocationUnavailable;  
    if (DEBUG)
        APP_LOG(APP_LOG_LEVEL_DEBUG, "GenericWeatherStatusLocationUnavailable");
        snprintf(temp_text, sizeof(temp_text),"%s","");
      //text_layer_set_text(s_text_layer, "GenericWeatherStatusLocationUnavailable");
      break;
  }

  

}


static void review_layers(){
   if (config_get(PERSIST_TEMPERATURE)){
    layer_set_hidden(text_layer_get_layer(s_weather_temp_layer),false);
  }else{
    layer_set_hidden(text_layer_get_layer(s_weather_temp_layer),true);
  }
  if (config_get(PERSIST_HUMIDITY)){
    layer_set_hidden(text_layer_get_layer(s_weather_hum_layer),false);
  }else{
    layer_set_hidden(text_layer_get_layer(s_weather_hum_layer),true);
  }
  if (config_get(PERSIST_SUNRISE) || config_get(PERSIST_SUNSET)){
    layer_set_hidden(text_layer_get_layer(s_weather_sun_layer),false);
  }else{
    layer_set_hidden(text_layer_get_layer(s_weather_sun_layer),true);
  }
  
   if (config_get(PERSIST_CONDITIONS)){
    layer_set_hidden(text_layer_get_layer(s_weather_icon),false);
  }else{
    layer_set_hidden(text_layer_get_layer(s_weather_icon),true);
  }
}  


/*
 Update proc
*/
static void bg_update_proc(Layer *layer, GContext *ctx) {
  GRect bounds = layer_get_bounds(layer);
  GPoint center = grect_center_point(&bounds);
  graphics_context_set_stroke_color(ctx, GColorWhite);

  for(int h = 0; h < 60; h++) {   
        GPoint point = (GPoint) {
          .x = (int16_t)(sin_lookup(TRIG_MAX_ANGLE * h / 60) * (int32_t)(3 * config_get(PERSIST_HAND_LENGTH_SEC)) / TRIG_MAX_RATIO) + center.x,
          .y = (int16_t)(-cos_lookup(TRIG_MAX_ANGLE * h / 60) * (int32_t)(3 * config_get(PERSIST_HAND_LENGTH_SEC)) / TRIG_MAX_RATIO) + center.y,
        };
        graphics_draw_line(ctx, GPoint(center.x, center.y), GPoint(point.x, point.y));
    
  }

  // Make square around screen for minute marks
  graphics_context_set_fill_color(ctx, GColorBlack);
  graphics_fill_rect(ctx, GRect(1, 1, bounds.size.w - (2), bounds.size.h - (2)), 0, GCornerNone);


  graphics_context_set_stroke_color(ctx, GColorWhite);


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
              
            for(int y = 0; y < THICKNESSMARKS; y++) {
              for(int x = 0; x < THICKNESSMARKS; x++) {
                graphics_draw_line(ctx, GPoint(point02.x + x, point02.y + y), GPoint(point.x + x, point.y + y));
              }
            }
    
  }

  // Make markers
  graphics_context_set_fill_color(ctx, GColorBlack);
 
  if (config_get(PERSIST_KEY_DAY)==1){
    graphics_context_set_fill_color(ctx, GColorWhite);
    graphics_fill_rect(ctx, GRect(100, 75, 25,22), 1, GCornersAll);  
  }
  
 
  review_layers();
// Could print weather conditions


  

}

static GPoint make_hand_point(int quantity, int intervals, int len, GPoint center) {
  return (GPoint) {
    .x = (int16_t)(sin_lookup(TRIG_MAX_ANGLE * quantity / intervals) * (int32_t)len / TRIG_MAX_RATIO) + center.x,
    .y = (int16_t)(-cos_lookup(TRIG_MAX_ANGLE * quantity / intervals) * (int32_t)len / TRIG_MAX_RATIO) + center.y,
  };
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


  // Draw hands
  // int y = 0;
  graphics_context_set_stroke_color(ctx, GColorWhite);
  for(int y = 0; y < THICKNESS; y++) {
    for(int x = 0; x < THICKNESS; x++) {
      graphics_draw_line(ctx, GPoint(center.x + x, center.y + y), GPoint(minute_hand_long.x + x, minute_hand_long.y + y));
      graphics_draw_line(ctx, GPoint(center.x + x, center.y + y), GPoint(minute_hand_inverse.x + x, minute_hand_inverse.y + y));
      graphics_draw_line(ctx, GPoint(center.x + x, center.y + y), GPoint(hour_hand_long.x + x, hour_hand_long.y + y));
      graphics_draw_line(ctx, GPoint(center.x + x, center.y + y), GPoint(hour_hand_inverse.x + x, hour_hand_inverse.y + y));
    }
  }

  // Draw seconds hand
  
  #if defined(PBL_COLOR)
    graphics_context_set_stroke_color(ctx, GColorRed);
  #else
    graphics_context_set_stroke_color(ctx, GColorWhite);
  #endif                 
  
  if(config_get(PERSIST_KEY_SECOND_HAND)) {
    for(int y = 0; y < THICKNESS_SECONDS; y++) {
      for(int x = 0; x < THICKNESS_SECONDS; x++) {       
        graphics_draw_line(ctx, GPoint(center.x + x, center.y+y ), GPoint(second_hand_inverse.x + x, second_hand_inverse.y+y ));
        graphics_draw_line(ctx, GPoint(center.x + x, center.y+y ), GPoint(second_hand_long.x + x, second_hand_long.y+y ));
      }
    }
  //  handsSeparators(ctx, now.seconds);
 
  // Draw circle for seconds hand
 graphics_draw_circle(ctx, GPoint(second_hand_inverse_circle.x + 1, second_hand_inverse_circle.y + 1), 4);
  #if defined(PBL_COLOR)
    graphics_context_set_stroke_color(ctx, GColorBlack);
  #else
    graphics_context_set_stroke_color(ctx, GColorBlack);
  #endif      
 
 graphics_draw_circle(ctx, GPoint(second_hand_inverse_circle.x + 1, second_hand_inverse_circle.y + 1), 3);
 graphics_draw_circle(ctx, GPoint(second_hand_inverse_circle.x + 1, second_hand_inverse_circle.y + 1), 5);
 #if defined(PBL_COLOR)
    graphics_context_set_stroke_color(ctx, GColorRed);
  #else
    graphics_context_set_stroke_color(ctx, GColorWhite);
  #endif     
 
 }

  // Center
  graphics_context_set_stroke_color(ctx, GColorBlack);
  graphics_context_set_fill_color(ctx, GColorWhite);
  graphics_draw_circle(ctx, GPoint(center.x + 1, center.y + 1), 4);
  graphics_fill_circle(ctx, GPoint(center.x + 1, center.y + 1), 3);
  graphics_context_set_fill_color(ctx, GColorBlack);
  graphics_fill_circle(ctx, GPoint(center.x + 1, center.y + 1), 1);
  
}




static void tick_handler(struct tm *tick_time, TimeUnits changed) {
  s_last_time.days = tick_time->tm_mday;
  s_last_time.hours = tick_time->tm_hour;
  s_last_time.minutes = tick_time->tm_min;
  s_last_time.seconds = tick_time->tm_sec;
  
  snprintf(s_day_in_month_buffer, sizeof(s_day_in_month_buffer), "%d", s_last_time.days);
  strftime(s_weekday_buffer, sizeof(s_weekday_buffer), "%a", tick_time);
  strftime(s_month_buffer, sizeof(s_month_buffer), "%b", tick_time);  

  // Finally
  layer_mark_dirty(s_canvas_layer);

  unsigned int seconds_for_poll = config_get(PERSIST_POLLTIME)*60;
  unsigned int now = mktime(tick_time);
  if( (now > s_last_unit_weather_change+seconds_for_poll) || s_last_unit_weather_change == 0)  {
    

    if (config_get(PERSIST_TEMPERATURE) || config_get(PERSIST_HUMIDITY) || config_get(PERSIST_SUNRISE) || config_get(PERSIST_SUNSET)){
      s_last_unit_weather_change = mktime(tick_time);
        
        
        if(DEBUG)
          APP_LOG(APP_LOG_LEVEL_INFO,"Entra a callback de clima");
         generic_weather_fetch(weather_callback);  
    }
  }
}

static void bt_handler(bool connected) {
  s_connected = connected;
  layer_mark_dirty(s_battery_layer);
}


void battery_state_handler(BatteryChargeState charge) {
  battery_level = charge.charge_percent;
  battery_plugged = charge.is_plugged;
  layer_mark_dirty(s_battery_layer);
 
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


  s_visitor_14_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_VISITOR_14));
  s_visitor_20_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_VISITOR_20));
  s_weather_12_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_WEATHER_14));

  icon_battery = gbitmap_create_with_resource(RESOURCE_ID_BATTERY_ICON);
  icon_battery_low = gbitmap_create_with_resource(RESOURCE_ID_BATTERY_ICON_LOW);
  icon_battery_charge = gbitmap_create_with_resource(RESOURCE_ID_BATTERY_CHARGE);
  icon_bt_disconected = gbitmap_create_with_resource(RESOURCE_ID_BLUETOOTH_DISCONNECTED);

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
  
  text_layer_set_text(s_weekday_layer, s_weekday_buffer);
  text_layer_set_text(s_day_in_month_layer, s_day_in_month_buffer);
  text_layer_set_text(s_month_layer, s_month_buffer);
  
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
  s_battery_layer = layer_create(GRect(0,30,144,106));
  layer_set_update_proc(s_battery_layer, &battery_layer_update_callback);
  layer_add_child(window_layer, s_battery_layer);

  // Layer del clima
  s_weather_temp_layer = text_layer_create(GRect(0, 90, 144, 50));
  text_layer_set_background_color(s_weather_temp_layer, GColorClear);
  text_layer_set_text_color(s_weather_temp_layer, GColorWhite);
  text_layer_set_font(s_weather_temp_layer, s_visitor_20_font);
  text_layer_set_text_alignment(s_weather_temp_layer, GTextAlignmentCenter);
  text_layer_set_text(s_weather_temp_layer, temp_text);
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_weather_temp_layer));

  s_weather_hum_layer = text_layer_create(GRect(0, 110, 144, 50));
  text_layer_set_background_color(s_weather_hum_layer, GColorClear);
  text_layer_set_text_color(s_weather_hum_layer, GColorWhite);
  text_layer_set_font(s_weather_hum_layer, s_visitor_14_font);
  text_layer_set_text_alignment(s_weather_hum_layer, GTextAlignmentCenter);
  text_layer_set_text(s_weather_hum_layer, hum_text);
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_weather_hum_layer));
 
  s_weather_sun_layer = text_layer_create(GRect(0, 120, 144, 50));
  text_layer_set_background_color(s_weather_sun_layer, GColorClear);
  text_layer_set_text_color(s_weather_sun_layer, GColorWhite);
  text_layer_set_font(s_weather_sun_layer, s_visitor_14_font);
  text_layer_set_text_alignment(s_weather_sun_layer, GTextAlignmentCenter);
  text_layer_set_text(s_weather_sun_layer, sun_text);
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

  s_weather_icon = text_layer_create(GRect(35, 95, 20, 25));
  text_layer_set_text_alignment(s_weather_icon, GTextAlignmentCenter);
  text_layer_set_font(s_weather_icon, s_weather_12_font);
  text_layer_set_text_color(s_weather_icon, GColorWhite);
  text_layer_set_background_color(s_weather_icon, GColorClear);
  text_layer_set_text_alignment(s_weather_icon, GTextAlignmentCenter);
  text_layer_set_text(s_weather_icon, cond_icon_text);
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_weather_icon));

  

  if (config_get(PERSIST_NUMBERS)){
    layer_set_hidden(text_layer_get_layer(s_12_layer),false);
    layer_set_hidden(text_layer_get_layer(s_6_layer),false);
    layer_set_hidden(text_layer_get_layer(s_9_layer),false);
  }else{
    layer_set_hidden(text_layer_get_layer(s_12_layer),true);
    layer_set_hidden(text_layer_get_layer(s_6_layer),true);
    layer_set_hidden(text_layer_get_layer(s_9_layer),true);
  } 


  s_canvas_layer = layer_create(bounds);
  layer_set_update_proc(s_canvas_layer, draw_proc);
  layer_add_child(window_layer, s_canvas_layer);

  //s_inverter_layer = inverter_layer_create(bounds);

  
  

  //layer_add_child(window_layer,inverter_layer_get_layer(s_inverter_layer));
}

static void window_unload(Window *window) {
  gbitmap_destroy(icon_battery);
  gbitmap_destroy(icon_battery_low);
  gbitmap_destroy(icon_battery_charge);
  gbitmap_destroy(icon_bt_disconected);


  fonts_unload_custom_font(s_visitor_14_font);
  fonts_unload_custom_font(s_visitor_20_font);
  fonts_unload_custom_font(s_weather_12_font);


  layer_destroy(s_canvas_layer);
  layer_destroy(s_bg_layer);
  layer_destroy(s_battery_layer);
//  inverter_layer_destroy(s_inverter_layer);

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
 

  text_layer_destroy(s_weather_icon);

  generic_weather_deinit();

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

  time_t t = time(NULL);
  struct tm *tm_now = localtime(&t);
  tick_handler(tm_now, SECOND_UNIT);

  if(config_get(PERSIST_KEY_BT)) {
    bluetooth_connection_service_subscribe(bt_handler);
    bt_handler(bluetooth_connection_service_peek());
  }
}


static void inbox_received_callback(DictionaryIterator *iterator, void *context) {
  bool updated_config = false;
  if (DEBUG)
  APP_LOG(APP_LOG_LEVEL_INFO, "Entra a inbox received call");
 

 // Read first item
  Tuple *t = dict_read_first(iterator);

  // For all items
  while(t != NULL) {
    // Which key was received?
    switch(t->key) {                
      case KEY_weatherApi:
            updated_config = true;
            persist_write_string(PERSIST_WEATHER_API, t->value->cstring);
            persist_read_string(PERSIST_WEATHER_API, weatherApi, sizeof(weatherApi));             
            generic_weather_set_api_key(weatherApi);

      break;
      case KEY_weatherProvider:
              updated_config = true;
              if(DEBUG)
                APP_LOG(APP_LOG_LEVEL_INFO, "Provider actual: %d",weatherProvider);
              persist_write_int(PERSIST_SERVICE_PROVIDER, (int)t->value->cstring[0]-'0');
                            if(DEBUG)
                APP_LOG(APP_LOG_LEVEL_INFO, "Provider del config: %d",(int)t->value->cstring[0]-'0');

              weatherProvider = persist_read_int(PERSIST_SERVICE_PROVIDER);
              if(DEBUG)
                APP_LOG(APP_LOG_LEVEL_INFO, "Provider nuevo: %d",weatherProvider);
              generic_weather_set_provider(weatherProvider);

      break;
      case KEY_DATE:
              updated_config = true;
              config_set(PERSIST_KEY_DATE,(int)t->value->int32);
      break;
       case KEY_DAY:
              updated_config = true;
              config_set(PERSIST_KEY_DAY, (int)t->value->int32);              
      break; 
       case KEY_BT:
              updated_config = true;
              config_set(PERSIST_KEY_BT, (int)t->value->int32);              
      break;   
       case KEY_BATTERY:
              updated_config = true;
              config_set(PERSIST_KEY_BATTERY, (int)t->value->int32);              
      break;  
       case KEY_SECOND_HAND:
              updated_config = true;
              config_set(PERSIST_KEY_SECOND_HAND, (int)t->value->int32);              
      break;

      case TEMPERATURE:
              updated_config = true;
              config_set(PERSIST_TEMPERATURE, (int)t->value->int32);  
              config_set(PERSIST_CONDITIONS, (int)t->value->int32);        

      break;   
      case CONDITIONS:
              updated_config = true;
              config_set(PERSIST_CONDITIONS, (int)t->value->int32);     

      break;   
      case HUMIDITY:
              updated_config = true;
              config_set(PERSIST_HUMIDITY, (int)t->value->int32);     

      break;   
      case WIND:
              updated_config = true;
              config_set(PERSIST_WIND, (int)t->value->int32);     

      break;   
      case SUNSET:
              updated_config = true;
              config_set(PERSIST_SUNSET, (int)t->value->int32);     

      break;   
      case SUNRISE:
              updated_config = true;
              config_set(PERSIST_SUNRISE, (int)t->value->int32);     

      break;   
      case DIGITALTIME:
              updated_config = true;
              config_set(PERSIST_DIGITALTIME, (int)t->value->int32);     

      break;   
      case INVERTED:
              updated_config = true;
              config_set(PERSIST_INVERTED, (int)t->value->int32); 
      break;
      case NUMBERS:
              updated_config = true;
              config_set(PERSIST_NUMBERS, (int)t->value->int32);     
      break;
      case CELCIUS:
              updated_config = true;
              config_set(PERSIST_CELCIUS, (int)t->value->int32);     
      break;
      case POLLTIME:
              updated_config = true;
              config_set(PERSIST_POLLTIME, (int)t->value->int32);     
      break;

      case READY:
        if (config_get(PERSIST_TEMPERATURE) || config_get(PERSIST_HUMIDITY) || config_get(PERSIST_SUNRISE) || config_get(PERSIST_SUNSET)){
             generic_weather_fetch(weather_callback);  
           }
      break;
      

      default:
      if (DEBUG)
        APP_LOG(APP_LOG_LEVEL_ERROR, "Key %d not recognized!", (int)t->key);
      break;
    }



      
    // Look for next item
    t = dict_read_next(iterator);
  }


   if(updated_config){
    if (DEBUG)
      APP_LOG(APP_LOG_LEVEL_INFO,"Entra a updated config");
     if (config_get(PERSIST_TEMPERATURE) || config_get(PERSIST_HUMIDITY) || config_get(PERSIST_SUNRISE) || config_get(PERSIST_SUNSET)){
       generic_weather_fetch(weather_callback); 
      if (DEBUG)
        APP_LOG(APP_LOG_LEVEL_INFO,"Entra a update weather");
     }
     review_layers();
   }
 
}



void comm_init() {
  events_app_message_request_inbox_size(1024);
  events_app_message_request_outbox_size(1024);
  events_app_message_register_inbox_received(inbox_received_callback,NULL);
  events_app_message_open();
  

}

void main_init(){
  s_last_unit_weather_change = 0;
  clear_weather_text();
  generic_weather_init();
  generic_weather_set_provider(weatherProvider);
  generic_weather_set_api_key(weatherApi);
  generic_weather_set_feels_like(false);

   

}