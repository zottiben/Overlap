#pragma once
#include <pebble.h>

#define KEY_SETTING_TIME_COLOR 6
#define KEY_SETTING_TIME_BG_COLOR 7
#define KEY_SETTING_LANGUAGE_ID 8
#define KEY_SETTING_LEADING_ZERO 9
#define KEY_SETTING_MERIDIAN 10
#define KEY_SETTING_SIZE_INCREASE 11
#define KEY_SETTING_BLINK 12
  
void (*message_processed_callback)(void);

void app_init(void (*message_processed_callback)(void));
void inbox_received_callback(DictionaryIterator *iter, void *context);
void inbox_dropped_callback(AppMessageResult reason, void *context);
void outbox_failed_callback(DictionaryIterator *iter, AppMessageResult reason, void *context);
void outbox_sent_callback(DictionaryIterator *iter, void *context);