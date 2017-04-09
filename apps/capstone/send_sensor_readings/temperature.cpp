#include "temperature.h"

static MCP342X *node_adc;

bool init_temp(MCP342X *adc){
  node_adc = adc;

  // Todo: check if the temperature sensor returns a sensible value
  return true;
}

void read_temp(uint16_t &temp){
  node_adc->startConversion();
  node_adc->getResult((int16_t *)&temp);
}