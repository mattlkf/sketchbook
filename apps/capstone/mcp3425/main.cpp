#include <Arduino.h>
#include <Wire.h>
#include  <MCP342X.h>


// ADC object
MCP342X MCP_ADC;

void setup(){
  Wire.begin();  // join I2C bus
  TWBR = 12;  // 400 kHz (maximum)

  Serial.begin(115200);

  Serial.println("Starting up");
  Serial.println("Testing device connection...");
    Serial.println(MCP_ADC.testConnection() ? "MCP342X connection successful" : "MCP342X connection failed");
    
  MCP_ADC.configure(MCP342X_MODE_CONTINUOUS |
                    MCP342X_CHANNEL_1 |
                    MCP342X_SIZE_16BIT |
                    MCP342X_GAIN_1X
                 );

  Serial.println(MCP_ADC.getConfigRegShdw(), HEX);

}

float raw_to_voltage(int16_t adc_reading){
  // Assumes we are in 16-bit mode
  return (float)adc_reading / (float)(0x7FFF) * 2.048;
}

int main(){
  init();

  setup();

  while(1){
    static int16_t result;

    int16_t inbuilt_adc;
    inbuilt_adc = analogRead(A0);
    
    MCP_ADC.startConversion();
    MCP_ADC.getResult(&result);

    // Serial.print(result);    
    Serial.print(raw_to_voltage(result));
    Serial.print("  ");
    Serial.print((float)inbuilt_adc / 1023.0 * 3.7);
    Serial.print("  ");
    // Serial.println(result, HEX);
    Serial.println();

    delay(500);
  }

  return 0;
}