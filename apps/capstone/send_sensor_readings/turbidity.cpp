#include "turbidity.h"

static MCP342X *node_adc;

#define PIN A0

bool init_turbidity(MCP342X *adc){
  node_adc = adc;

  // Todo: check if the turbidity sensor returns a sensible value
  return true;
}

void read_turbidity(uint16_t &turbidity){

  // Because the two-channel MCP3426 has not been ordered yet, we are using
  // the Arduino's inbuilt ADC for now.

  // We do have the single-channel MCP3425, but that is being used by the
  // temperature sensor.

  turbidity = analogRead(PIN);
}