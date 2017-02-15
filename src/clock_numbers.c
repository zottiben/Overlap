#include <pebble.h>
#include "clock_numbers.h"

void imageRender(ClockNumber* this);

uint32_t ClockNumber_ids[2][10] = {
  {
    RESOURCE_ID_CLOCK_NUMBER_0,
    RESOURCE_ID_CLOCK_NUMBER_1,
    RESOURCE_ID_CLOCK_NUMBER_2,
    RESOURCE_ID_CLOCK_NUMBER_3,
    RESOURCE_ID_CLOCK_NUMBER_4,
    RESOURCE_ID_CLOCK_NUMBER_5,
    RESOURCE_ID_CLOCK_NUMBER_6,
    RESOURCE_ID_CLOCK_NUMBER_7,
    RESOURCE_ID_CLOCK_NUMBER_8,
    RESOURCE_ID_CLOCK_NUMBER_9
  },
  {
    RESOURCE_ID_CLOCK_NUMBER_LARGE_0,
    RESOURCE_ID_CLOCK_NUMBER_LARGE_1,
    RESOURCE_ID_CLOCK_NUMBER_LARGE_2,
    RESOURCE_ID_CLOCK_NUMBER_LARGE_3,
    RESOURCE_ID_CLOCK_NUMBER_LARGE_4,
    RESOURCE_ID_CLOCK_NUMBER_LARGE_5,
    RESOURCE_ID_CLOCK_NUMBER_LARGE_6,
    RESOURCE_ID_CLOCK_NUMBER_LARGE_7,
    RESOURCE_ID_CLOCK_NUMBER_LARGE_8,
    RESOURCE_ID_CLOCK_NUMBER_LARGE_9
  }  
};

void ClockNumber_setNumber(ClockNumber* this, int number, int size) {
  if(this->currentNum != number || this->currentSize != size) {
    GBitmap* oldNumber  = this->currentNumber;
    
    this->currentNumberId = ClockNumber_ids[size][number];
    this->currentNumber = gbitmap_create_with_resource(this->currentNumberId);
    this->currentNum = number;
    this->currentSize = size;
    
    imageRender(this);
    
    bitmap_layer_set_bitmap(this->imageLayer, this->currentNumber);

    gbitmap_destroy(oldNumber);
  
  }
  
  layer_set_hidden((Layer *)this->imageLayer, false);
}

void ClockNumber_setBlank(ClockNumber* this) {
  layer_set_hidden((Layer *)this->imageLayer, true);
}

void ClockNumber_offset(ClockNumber* this, int posOffset) {
  layer_set_frame((Layer *)this->imageLayer, GRect(this->position.x + posOffset, this->position.y, 48, 68));
}

void ClockNumber_setColor(ClockNumber* this, GColor fg, GColor bg) {
  this->fgColor = fg;
  this->bgColor = bg;
  
  #ifdef PBL_COLOR
    int colorIncrementR = (fg.r * 85 - bg.r * 85) / 3;
    int colorIncrementG = (fg.g * 85 - bg.g * 85) / 3;
    int colorIncrementB = (fg.b * 85 - bg.b * 85) / 3;
  
    this->midColor1 = GColorFromRGB(fg.r * 85 - colorIncrementR,
                                    fg.g * 85 - colorIncrementG,
                                    fg.b * 85 - colorIncrementB);
  
    this->midColor2 = GColorFromRGB(bg.r * 85 - colorIncrementR,
                                    bg.g * 85 - colorIncrementG,
                                    bg.b * 85 - colorIncrementB);
  #endif 
  
  imageRender(this);
}

void ClockNumber_construct(ClockNumber* this, GPoint pos, int size) {
  this->currentNum = -1;
  this->bgColor = GColorWhite;
  this->fgColor = GColorBlack;
  this->position = pos;
  
  if(size == 1) {
    this->imageLayer = bitmap_layer_create(GRect(pos.x, pos.y, 58, 82));
  } else {
    this->imageLayer = bitmap_layer_create(GRect(pos.x, pos.y, 48, 68));
  }
  
  
  ClockNumber_setBlank(this);
  ClockNumber_setNumber(this, 1, 0);
  ClockNumber_setColor(this, GColorBlack, GColorWhite);
}

void ClockNumber_destruct(ClockNumber* this) {
  bitmap_layer_destroy(this->imageLayer);
  gbitmap_destroy(this->currentNumber);
}

void imageRender(ClockNumber* this) {
  #ifdef PBL_COLOR
    GColor* pal = gbitmap_get_palette(this->currentNumber);

    pal[0] = this->fgColor;
    pal[1] = this->bgColor;

  #else
    if(gcolor_equal(this->bgColor, GColorBlack)) {
      bitmap_layer_set_compositing_mode(this->imageLayer, GCompOpAssignInverted);
    } else {
      bitmap_layer_set_compositing_mode(this->imageLayer, GCompOpAssign);
    }
  #endif
}