/*
 * Testing the MCP9700A temperature sensor (10.0mV/C gradient, 500mV at 0 deg)
 */

#include <Arduino.h>

#define TEMP_PIN A0
#define V_MAX 3.2

#define V_DEG_0 0.500
#define TEMP_COEFF 0.01

void setup(){
    Serial.begin(115200);
}

float temp(){
    uint16_t adc_reading = analogRead(TEMP_PIN);
    Serial.println(adc_reading);
    return (((float) adc_reading / 1024.0) * V_MAX - V_DEG_0) / TEMP_COEFF;
}

int main(){
    init();
    setup();

    while(1){
        Serial.print("Temp: ");
        Serial.println(temp());

        // Serial.print("A0: ");
        // Serial.println(analogRead(A0));

        // Serial.print("A4: ");
        // Serial.println(analogRead(A4));

        // Serial.print("A5: ");
        // Serial.println(analogRead(A5));

        delay(500);
    }
    return 0;
}