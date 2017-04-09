#include <stdint.h>
#include <MCP342X.h>

bool init_temp(MCP342X *adc);
void read_temp(uint16_t &temp);