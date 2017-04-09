#include "color.h"
#include "SparkFunISL29125.h"

// Declare sensor object
static SFE_ISL29125 RGB_sensor;

bool init_color(){
  // Return true if init was successful
  return RGB_sensor.init();
}

void read_color(uint16_t &red, uint16_t &green, uint16_t &blue){
  red   = RGB_sensor.readRed();
  green = RGB_sensor.readGreen();
  blue  = RGB_sensor.readBlue();
}