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
        // Serial.println("Sensor Init Successful");
    }
    else{
        // Serial.println("Sensor Init Unsuccessful");
    }
}

int main(){
    init();
    setup();

    while(1){
        // Serial.print("Red: "); Serial.println(RGB_sensor.readRed(),HEX);
        // Serial.print("Green: "); Serial.println(RGB_sensor.readGreen(),HEX);
        // Serial.print("Blue: "); Serial.println(RGB_sensor.readBlue(),HEX);

        uint16_t red   = RGB_sensor.readRed();
        uint16_t green = RGB_sensor.readGreen();
        uint16_t blue  = RGB_sensor.readBlue();

        Serial.print(red);
        Serial.print(" ");
        Serial.print(green);
        Serial.print(" ");
        Serial.print(blue);
        Serial.println();


        delay(100);

        // Serial.print("Red: "); Serial.println(RGB_sensor.readRed());
        // Serial.print("Green: "); Serial.println(RGB_sensor.readGreen());
        // Serial.print("Blue: "); Serial.println(RGB_sensor.readBlue());

        // Serial.println();

    }
    return 0;
}