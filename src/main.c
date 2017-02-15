#include <pebble.h>
#include <ctype.h>
#include <math.h>

#include "clock_numbers.h"
#include "app.h"
#include "settings.h"
#include "languages.h"

static Window* mainWindow;

static bool bluetoothConnected;

static ClockNumber clockNumbers[4];
static GPoint numberPos[4];

static TextLayer *s_time_meridian;
static BitmapLayer* s_time_meridian_zoom_layer;
static Layer *s_time_meridian_zoom;

static BitmapLayer* midBarOverlay;
static Layer *numberFixOverlay;
static GPath *numberFixOverlay_path;

void sep_update(Layer *layer, GContext *ctx) {
	GPathInfo POINTS_PATH_INFO = {
	  .num_points = 4,
	  .points = (GPoint []) {{15, 0}, {0, 15}, {15, 15}, {15, 0}}
	};
	GPathInfo POINTS_PATH_INFO_LARGE = {
	  .num_points = 4,
	  .points = (GPoint []) {{21, 0}, {0, 21}, {22, 22}, {22, 0}}
	};

	if(globalSettings.sizeIncrease) {
	  numberFixOverlay_path = gpath_create(&POINTS_PATH_INFO_LARGE);
	} else {
	  numberFixOverlay_path = gpath_create(&POINTS_PATH_INFO);
	}
	graphics_context_set_fill_color(ctx, globalSettings.timeColor);
	gpath_draw_filled(ctx, numberFixOverlay_path);
}

void update_clock();
void screenRedraw();
void tick_handler(struct tm *tick_time, TimeUnits units_changed);
void bluetoothStateChanged(bool newState);

void update_clock() {
  
  time_t rawTime;
  struct tm* timeInfo;
  
  time(&rawTime);
  timeInfo = localtime(&rawTime);
  
   // Use for screenshots
   // timeInfo->tm_hour = 7;
   // timeInfo->tm_min = 35;
   
  int hour = timeInfo->tm_hour;
  
  if(!clock_is_24h_style()) {
    
  	if(hour > 12) {
  		hour %= 12;
  	} else if(timeInfo->tm_hour == 0) {
  		hour = 12;
  	}

    if(globalSettings.showMeridian && !globalSettings.sizeIncrease) {
      layer_set_hidden((Layer *)s_time_meridian_zoom_layer, true);
      layer_set_hidden((Layer *)s_time_meridian_zoom, true);
      layer_set_hidden((Layer *)s_time_meridian, false);
      if(timeInfo->tm_hour >= 12 && timeInfo->tm_hour != 24) {
        text_layer_set_text(s_time_meridian, "PM");
      } else {
        text_layer_set_text(s_time_meridian, "AM");
      }
    } else if(globalSettings.showMeridian && globalSettings.sizeIncrease) {
      layer_set_hidden((Layer *)s_time_meridian, true);
      layer_set_hidden((Layer *)s_time_meridian_zoom_layer, false);
      layer_set_hidden((Layer *)s_time_meridian_zoom, false);
      if(timeInfo->tm_hour >= 12 && timeInfo->tm_hour != 24) {
        #ifdef PBL_PLATFORM_CHALK
        layer_set_frame((Layer *)s_time_meridian_zoom, GRect(154, 130, 10, 10));
        layer_set_frame((Layer *)s_time_meridian_zoom_layer, GRect(153, 136, 10, 10));
        #else
        layer_set_frame((Layer *)s_time_meridian_zoom, GRect(136, 150, 8, 8));
        layer_set_frame((Layer *)s_time_meridian_zoom_layer, GRect(136, 150, 8, 8));
        #endif
      } else {
        #ifdef PBL_PLATFORM_CHALK
        layer_set_frame((Layer *)s_time_meridian_zoom, GRect(154, 38, 10, 10));
        layer_set_frame((Layer *)s_time_meridian_zoom_layer, GRect(153, 38, 10, 10));
        #else
        layer_set_frame((Layer *)s_time_meridian_zoom, GRect(1, 11, 8, 8));
        layer_set_frame((Layer *)s_time_meridian_zoom_layer, GRect(136, 11, 8, 8 ));
        #endif
      }
      
      if(globalSettings.blink) {
        layer_set_hidden(bitmap_layer_get_layer(s_time_meridian_zoom_layer), timeInfo->tm_sec % 2);
      }
      
    } else {
      layer_set_hidden((Layer *)s_time_meridian, true);
      layer_set_hidden((Layer *)s_time_meridian_zoom_layer, true);
      layer_set_hidden((Layer *)s_time_meridian_zoom, true);
    }
    
  } else { 
    layer_set_hidden((Layer *)s_time_meridian_zoom_layer, true);
    layer_set_hidden((Layer *)s_time_meridian_zoom, true);
    layer_set_hidden((Layer *)s_time_meridian, true);
  }
  
  if(timeInfo->tm_min % 10 == 4) {
    layer_set_hidden((Layer *)midBarOverlay, true);
    layer_set_hidden(numberFixOverlay, false);
  } else {
    layer_set_hidden(numberFixOverlay, true);
    layer_set_hidden((Layer *)midBarOverlay, false);
  }

  if(globalSettings.showLeadingZero || hour / 10 != 0) {
    ClockNumber_setNumber(&clockNumbers[0], hour / 10, globalSettings.sizeIncrease); 
  } else {
    ClockNumber_setBlank(&clockNumbers[0]);
  }
  
  ClockNumber_setNumber(&clockNumbers[1], hour % 10, globalSettings.sizeIncrease);
  ClockNumber_setNumber(&clockNumbers[2], timeInfo->tm_min / 10, globalSettings.sizeIncrease);
  ClockNumber_setNumber(&clockNumbers[3], timeInfo->tm_min % 10, globalSettings.sizeIncrease);
  
}

void screenRedraw() {
  
	if(globalSettings.blink) {
		tick_timer_service_unsubscribe();
    	tick_timer_service_subscribe(SECOND_UNIT, tick_handler);
  	} else {
  		tick_timer_service_unsubscribe();
    	tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);
  	}

  	for(int i = 3; i > -1; i--) {
    	ClockNumber_setColor(&clockNumbers[i], globalSettings.timeColor, globalSettings.timeBgColor);
  	}
  
  	text_layer_set_text_color(s_time_meridian, globalSettings.timeColor);
  	window_set_background_color(mainWindow, globalSettings.timeBgColor);
  
  	layer_set_update_proc(numberFixOverlay, sep_update);
  	bitmap_layer_set_background_color(midBarOverlay, globalSettings.timeColor);
  
  	bitmap_layer_set_background_color(s_time_meridian_zoom_layer, globalSettings.timeColor);

   if(globalSettings.sizeIncrease) { 
    #ifdef PBL_PLATFORM_CHALK
    layer_set_frame((Layer *)numberFixOverlay, GRect(110,81,25,25));
    layer_set_frame(bitmap_layer_get_layer(midBarOverlay), GRect(91,81,48,17));
    layer_set_frame(bitmap_layer_get_layer(clockNumbers[0].imageLayer), GRect(28, 16, 58, 82));
    layer_set_frame(bitmap_layer_get_layer(clockNumbers[1].imageLayer), GRect(91, 16, 58, 82));
    layer_set_frame(bitmap_layer_get_layer(clockNumbers[2].imageLayer), GRect(28, 81, 58, 82));
    layer_set_frame(bitmap_layer_get_layer(clockNumbers[3].imageLayer), GRect(91, 81, 58, 82));
    #else
    layer_set_frame((Layer *)numberFixOverlay, GRect(93,76,25,25));
    layer_set_frame(bitmap_layer_get_layer(midBarOverlay), (GRect(74,77,48,16)));
    layer_set_frame(bitmap_layer_get_layer(clockNumbers[0].imageLayer), GRect(11, 11, 58, 82));
    layer_set_frame(bitmap_layer_get_layer(clockNumbers[1].imageLayer), GRect(74, 11, 58, 82));
    layer_set_frame(bitmap_layer_get_layer(clockNumbers[2].imageLayer), GRect(11, 76, 58, 82));
    layer_set_frame(bitmap_layer_get_layer(clockNumbers[3].imageLayer), GRect(74, 76, 58, 82));
    #endif
  } else {
    
    #ifdef PBL_PLATFORM_CHALK
    layer_set_frame((Layer *)numberFixOverlay, GRect(113,81,25,25));
    layer_set_frame(bitmap_layer_get_layer(midBarOverlay), GRect(93,83,48,14));
    layer_set_frame(bitmap_layer_get_layer(clockNumbers[0].imageLayer), GRect(40, 29, 48, 68));
    layer_set_frame(bitmap_layer_get_layer(clockNumbers[1].imageLayer), GRect(93, 29, 48, 68));
    layer_set_frame(bitmap_layer_get_layer(clockNumbers[2].imageLayer), GRect(40, 83, 48, 68));
    layer_set_frame(bitmap_layer_get_layer(clockNumbers[3].imageLayer), GRect(93, 83, 48, 68));
    #else
    layer_set_frame((Layer *)numberFixOverlay, GRect(93,76,25,25));
    layer_set_frame(bitmap_layer_get_layer(midBarOverlay), (GRect(74,77,48,14)));
    layer_set_frame(bitmap_layer_get_layer(clockNumbers[0].imageLayer), GRect(21, 23, 48, 68));
    layer_set_frame(bitmap_layer_get_layer(clockNumbers[1].imageLayer), GRect(74, 23, 48, 68));
    layer_set_frame(bitmap_layer_get_layer(clockNumbers[2].imageLayer), GRect(21, 77, 48, 68));
    layer_set_frame(bitmap_layer_get_layer(clockNumbers[3].imageLayer), GRect(74, 77, 48, 68));
    #endif
  }

  update_clock();
  
}

static void main_window_load(Window *window) {
   // Use these for testing //
   // #ifdef PBL_COLOR
   // globalSettings.timeColor = GColorCadetBlue;
   // globalSettings.timeBgColor = GColorLiberty;
   // #else 
   // globalSettings.timeColor = GColorBlack;
   // globalSettings.timeBgColor = GColorWhite;
   // #endif
   
   // globalSettings.sizeIncrease = false;
   // globalSettings.showLeadingZero = true;
   // globalSettings.showMeridian = true;
   // globalSettings.blink = false;

	#ifdef PBL_ROUND
		GPoint numberPos[4] = { GPoint(40,23), GPoint(88,23), GPoint(21,77), GPoint(74,77)};
	#else
		if(globalSettings.sizeIncrease) {
			GPoint numberPos[4] = { GPoint(11,11), GPoint(74,11), GPoint(11,75), GPoint(74,75)};
		} else {
			GPoint numberPos[4] = { GPoint(21,23), GPoint(74,23), GPoint(21,77), GPoint(74,77)};
		}
	#endif

	ClockNumber_construct(&clockNumbers[0], numberPos[0], globalSettings.sizeIncrease);
	ClockNumber_construct(&clockNumbers[1], numberPos[1], globalSettings.sizeIncrease);
	ClockNumber_construct(&clockNumbers[2], numberPos[2], globalSettings.sizeIncrease);
	ClockNumber_construct(&clockNumbers[3], numberPos[3], globalSettings.sizeIncrease);
  
  	for(int i = 3; i > -1; i--) {
    	ClockNumber_setColor(&clockNumbers[i], globalSettings.timeColor, globalSettings.timeBgColor);
    	layer_add_child(window_get_root_layer(window), bitmap_layer_get_layer(clockNumbers[i].imageLayer));
  	}
  
  s_time_meridian_zoom = layer_create(GRect(134, 11, 10, 10));
  s_time_meridian_zoom_layer = bitmap_layer_create(GRect(134, 11, 10, 10));
  layer_mark_dirty(s_time_meridian_zoom);
  layer_mark_dirty((Layer *)s_time_meridian_zoom_layer);
  layer_add_child(window_get_root_layer(window), s_time_meridian_zoom);
  layer_add_child(window_get_root_layer(window), bitmap_layer_get_layer(s_time_meridian_zoom_layer));
  
  
  #ifdef PBL_PLATFORM_CHALK
    if(globalSettings.sizeIncrease) {
      numberFixOverlay = layer_create(GRect(110,81,25,25));
      midBarOverlay = bitmap_layer_create(GRect(93,77,48,17));
    } else {
      numberFixOverlay = layer_create(GRect(113,81,25,25));
      midBarOverlay = bitmap_layer_create(GRect(91,77,48,14));
    }
  
  #else
    if(globalSettings.sizeIncrease) {
    	numberFixOverlay = layer_create(GRect(93,76,25,25));
	  	midBarOverlay = bitmap_layer_create(GRect(74,77,48,16));
    } else {
    	numberFixOverlay = layer_create(GRect(93,76,25,25));
	  	midBarOverlay = bitmap_layer_create(GRect(74,77,48,14));
    }
  #endif

  layer_mark_dirty(numberFixOverlay);
  layer_mark_dirty((Layer *)midBarOverlay);
  layer_add_child(window_get_root_layer(window), numberFixOverlay);
  layer_add_child(window_get_root_layer(window), bitmap_layer_get_layer(midBarOverlay));
  
  #ifdef PBL_PLATFORM_CHALK
  s_time_meridian = text_layer_create(GRect(147,79,30,30));
  #else
  s_time_meridian = text_layer_create(GRect(102,142,30,30));
  #endif
  
  layer_set_hidden(text_layer_get_layer(s_time_meridian), false);
  text_layer_set_text_color(s_time_meridian, globalSettings.timeColor);
  text_layer_set_background_color(s_time_meridian, GColorClear);
  text_layer_set_font(s_time_meridian, fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD));
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_time_meridian));
  
  window_set_background_color(window, globalSettings.timeBgColor);
  
  screenRedraw();
  update_clock();
  
}

static void main_window_unload(Window *window) {
  for(int i = 3; i > -1; i--) {
    ClockNumber_destruct(&clockNumbers[i]);
  }

  layer_destroy(numberFixOverlay);
  bitmap_layer_destroy(midBarOverlay);
  gpath_destroy(numberFixOverlay_path);
  text_layer_destroy(s_time_meridian);
  
  layer_destroy(s_time_meridian_zoom);
  bitmap_layer_destroy(s_time_meridian_zoom_layer);
}

void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
  update_clock();
}

void bluetoothStateChanged(bool newState) {
	if(bluetoothConnected && !newState) {
		static uint32_t const segments[] = {80,110,80,110,80,520,80,110,80,110,80};
		VibePattern pat = {
			.durations = segments,
			.num_segments = ARRAY_LENGTH(segments),
		};
		vibes_enqueue_custom_pattern(pat);
	}

	bluetoothConnected = newState;
}

static void init() {
  setlocale(LC_ALL, "");
  
  Settings_init();
  app_init(screenRedraw);
  
  mainWindow = window_create();
  
  window_set_window_handlers(mainWindow, (WindowHandlers) {
    .load = main_window_load,
    .unload = main_window_unload
  });
  
  window_stack_push(mainWindow, true);

  if(globalSettings.blink) {
    tick_timer_service_subscribe(SECOND_UNIT, tick_handler);
  } else {
    tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);
  }

  bool connected = bluetooth_connection_service_peek();
  bluetoothStateChanged(connected);
  bluetooth_connection_service_subscribe(bluetoothStateChanged);
  
}

static void deinit() {
  window_destroy(mainWindow);
  window_stack_remove(mainWindow, true);
  Settings_deinit();

  bluetooth_connection_service_unsubscribe();
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}