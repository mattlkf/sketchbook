/*
 * Testing the SparkFunISL29125 color sensor library
 */

#include <Arduino.h>
#include "Wire.h"
#include "SparkFunISL29125.h"

// Declare sensor object
SFE_ISL29125 RGB_sensor;

void setup() {
    Serial.begin(115200);

      // Initialize the ISL29125 with simple configuration so it starts sampling
    if (RGB_sensor.init()){
        Serial.println("Sensor Init Successful");
    }
    else{
        Serial.println("Sensor Init Unsuccessful");
    }
}

int main(){
    init();
    setup();

    while(1){
        Serial.print("Red: "); Serial.println(RGB_sensor.readRed(),HEX);
        Serial.print("Green: "); Serial.println(RGB_sensor.readGreen(),HEX);
        Serial.print("Blue: "); Serial.println(RGB_sensor.readBlue(),HEX);
        Serial.println();
        delay(2000);
    }
    return 0;
}