#include <Arduino.h>
#include <Wire.h>
#include <GOFi2cOLED.h>
#include <SHT2x.h> 

#define SERIAL_OUT
#define DELAY_MILLISECONDS 1000
#define BUTTON_PRESS_MAX 2000

#define INPIN 3

GOFi2cOLED oled;
 
void setupOLED(){
    oled.init(0x3C);  //initialze  OLED display, default slave address is 0x3D

    oled.clearDisplay();
    oled.setTextSize(2);
    oled.setTextColor(WHITE);
}



void setup(){
    pinMode(5, OUTPUT);
    digitalWrite(5, HIGH);

#ifdef SERIAL_OUT
    Serial.begin(115200);
#endif

    pinMode(INPIN, INPUT);

    setupOLED();
    Wire.begin();
}

int main(){	
    init();
    setup();

    uint32_t lastpress = 0;

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
        oled.print("RH %:");
        oled.print(h);
        oled.print("Temp:");
        oled.print(t);
        oled.display();

        // poll for button presses
        uint32_t tt = millis();
        
        while(millis() - tt < DELAY_MILLISECONDS){
            if(digitalRead(INPIN) == LOW){
                if(millis() - lastpress > BUTTON_PRESS_MAX){
                    lastpress = millis();

                    // action to take on button press
                    Serial.println("press-event!");
                    oled.println("Press!");
                    oled.display();
                }
            }
        }
    }
}