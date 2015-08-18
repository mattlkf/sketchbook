#include <Arduino.h>
#include <Wire.h>
#include <GOFi2cOLED.h>
#include <SHT2x.h> 

#define SERIAL_OUT
#define DELAY_MILLISECONDS 1000

GOFi2cOLED oled;
 
void setupOLED(){
    oled.init(0x3C);  //initialze  OLED display, default slave address is 0x3D

    // oled.display(); // show splashscreen
    // delay(2000);

    oled.clearDisplay();

    oled.setTextSize(1);
    oled.setTextColor(WHITE);
    oled.setCursor(0,0);

    // oled.setHorizontalScrollProperties(Scroll_Left,2, 4, Scroll_2Frames);
    // oled.activateScroll();
}



void setup(){
    pinMode(5, OUTPUT);
    digitalWrite(5, HIGH);

    // pinMode(3, INPUT);

#ifdef SERIAL_OUT
    Serial.begin(9600);
#endif

    setupOLED();
    Wire.begin();
}

int main(){	
    init();
    setup();


    while(1){
        float h = SHT2x.GetHumidity();
        float t = SHT2x.GetTemperature();

#ifdef SERIAL_OUT
        // Serial.print("Humidity(%RH): ");
        // Serial.println(h);
        // Serial.print("Temperature(C): ");
        // Serial.println(t);


        Serial.print(h);
        Serial.print(", ");
        Serial.print(t);
        Serial.println();
#endif

        oled.clearDisplay();
        oled.setCursor(0,0);
        oled.print("Humidity (%RH): ");
        oled.print(h);
        oled.print("Temp (°C): ");
        oled.print(t);
        oled.display();

        delay(DELAY_MILLISECONDS);
    }
}