#include <stdint.h>
#include <MCP342X.h>

bool init_turbidity(MCP342X *adc);
void read_turbidity(uint16_t &turbidity);