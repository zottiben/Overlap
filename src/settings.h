#pragma once
#include <pebble.h>

typedef struct {
  GColor timeColor;
  GColor timeBgColor;
  uint8_t languageId;
  bool showLeadingZero;
  bool showMeridian;
  bool sizeIncrease;
  bool blink;
} Settings;

extern Settings globalSettings;

#define SETTING_TIME_COLOR_KEY 17
#define SETTING_TIME_BG_COLOR_KEY 18
#define SETTING_LANGUAGE_ID_KEY 19
#define SETTING_LEADING_ZERO_KEY 20
#define SETTING_MERIDIAN_KEY 21
#define SETTING_SIZE_INCREASE_KEY 22
#define SETTING_BLINK_KEY 23
  
void Settings_init();
void Settings_deinit();
void Settings_loadFromStorage();
void Settings_saveToStorage();
void Settings_migrateLegacySettings();