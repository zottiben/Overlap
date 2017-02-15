#include "settings.h"

Settings globalSettings;

void Settings_init() {
  Settings_loadFromStorage();
}

void Settings_deinit() {
  Settings_saveToStorage();
}

void Settings_loadFromStorage() {
  if(persist_exists(SETTING_TIME_COLOR_KEY) && persist_exists(SETTING_TIME_BG_COLOR_KEY)) {
    persist_read_data(SETTING_TIME_COLOR_KEY, &globalSettings.timeColor, sizeof(GColor));
    persist_read_data(SETTING_TIME_BG_COLOR_KEY, &globalSettings.timeBgColor, sizeof(GColor));
  } else {
    globalSettings.timeColor = GColorBlack;
    
    #ifdef PBL_COLOR
      globalSettings.timeBgColor = GColorVividCerulean;
    #else
      globalSettings.timeBgColor = GColorWhite;
    #endif
  }
  
  globalSettings.showMeridian        = persist_read_bool(SETTING_MERIDIAN_KEY);
  globalSettings.showLeadingZero     = persist_read_bool(SETTING_LEADING_ZERO_KEY);
  globalSettings.sizeIncrease        = persist_read_bool(SETTING_SIZE_INCREASE_KEY);
  globalSettings.blink               = persist_read_bool(SETTING_BLINK_KEY);
}

void Settings_saveToStorage() {
  persist_write_data(SETTING_TIME_COLOR_KEY, &globalSettings.timeColor, sizeof(GColor));
  persist_write_data(SETTING_TIME_BG_COLOR_KEY, &globalSettings.timeBgColor, sizeof(GColor));
  persist_write_bool(SETTING_LEADING_ZERO_KEY, globalSettings.showLeadingZero);
  persist_write_bool(SETTING_MERIDIAN_KEY, globalSettings.showMeridian);
  persist_write_bool(SETTING_SIZE_INCREASE_KEY, globalSettings.sizeIncrease);
  persist_write_bool(SETTING_BLINK_KEY, globalSettings.blink);
}