#include "app.h"
#include "settings.h"
  
void app_init(void (*processed_callback)(void)) {
  message_processed_callback = processed_callback;
  
  app_message_register_inbox_received(inbox_received_callback);
  app_message_register_inbox_dropped(inbox_dropped_callback);
  app_message_register_outbox_failed(outbox_failed_callback);
  app_message_register_outbox_sent(outbox_sent_callback);
  
  app_message_open(420, 420);
  
  APP_LOG(APP_LOG_LEVEL_DEBUG, "Watch messaging is started!");
  app_message_register_inbox_received(inbox_received_callback);
}

void inbox_received_callback(DictionaryIterator *iter, void *context) {
  Tuple *timeColor_t = dict_find(iter, KEY_SETTING_TIME_COLOR);
  Tuple *timeBgColor_t = dict_find(iter, KEY_SETTING_TIME_BG_COLOR);
  Tuple *leadingZero_t = dict_find(iter, KEY_SETTING_LEADING_ZERO);
  Tuple *meridian_t = dict_find(iter, KEY_SETTING_MERIDIAN);
  Tuple *sizeIncrease_t = dict_find(iter, KEY_SETTING_SIZE_INCREASE);
  Tuple *blink_t = dict_find(iter, KEY_SETTING_BLINK);
  
  if(timeColor_t != NULL) {
    #ifdef PBL_COLOR
      globalSettings.timeColor = GColorFromHEX(timeColor_t->value->int32);
    #else
      globalSettings.timeColor = (timeColor_t->value->int32 == 0) ? GColorBlack : GColorWhite;
    #endif
  }
  
  if(timeBgColor_t != NULL) {
    #ifdef PBL_COLOR
      globalSettings.timeBgColor = GColorFromHEX(timeBgColor_t->value->int32);
    #else
      globalSettings.timeBgColor = (timeBgColor_t->value->int32 == 0) ? GColorBlack : GColorWhite;
    #endif
  }
  
  if(leadingZero_t != NULL) {
    globalSettings.showLeadingZero = (bool)leadingZero_t->value->int8;
  }
  
  if(meridian_t != NULL) {
    globalSettings.showMeridian = (bool)meridian_t->value->int8;
  }
  
  if(sizeIncrease_t != NULL) {
    globalSettings.sizeIncrease = (bool)sizeIncrease_t->value->int8;
  }
  
  if(blink_t != NULL) {
    globalSettings.blink = (bool)blink_t->value->int8;
  }
  
  Settings_saveToStorage();
  
  message_processed_callback();
  
}

void inbox_dropped_callback(AppMessageResult reason, void *context) {
  APP_LOG(APP_LOG_LEVEL_ERROR, "Message dropped!");
}

void outbox_failed_callback(DictionaryIterator *iter, AppMessageResult reason, void *context) {
  APP_LOG(APP_LOG_LEVEL_ERROR, "Outbox send failed! %d %d %d", reason, APP_MSG_SEND_TIMEOUT, APP_MSG_SEND_REJECTED);
}

void outbox_sent_callback(DictionaryIterator *iter, void *context) {
  APP_LOG(APP_LOG_LEVEL_INFO, "Outbox send success!");
}
