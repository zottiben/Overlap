#pragma once
  
#include <pebble.h>
  
#define FONT_ID_LECO 1
  
typedef struct {
  int currentNum;
  GColor bgColor;
  GColor fgColor;
  GColor midColor1;
  GColor midColor2;
  GPoint position;
  uint32_t currentNumberId;
  GBitmap* currentNumber;
  BitmapLayer* imageLayer;
  int currentSize;
} ClockNumber;

void ClockNumber_setNumber(ClockNumber* this, int number, int size);
void ClockNumber_setBlank(ClockNumber* this);
void ClockNumber_setColor(ClockNumber* this, GColor fg, GColor bg);
void ClockNumber_offset(ClockNumber* this, int posOffset);

void ClockNumber_construct(ClockNumber* this, GPoint pos, int size);
void ClockNumber_destruct(ClockNumber* this);

void imageRender(ClockNumber* this);