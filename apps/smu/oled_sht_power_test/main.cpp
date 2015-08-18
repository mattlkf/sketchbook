#include <Arduino.h>
#include <Wire.h>
#include <GOFi2cOLED.h>
#include <SHT2x.h> 

#define SERIAL_OUT
#define DELAY_MILLISECONDS 1000

#define LEDPIN 13

#define SHTPIN 9
#define OLEDPIN_0 3
#define OLEDPIN_1 4
#define OLEDPIN_2 5
#define OLEDPIN_3 6

GOFi2cOLED oled;
 
void setupOLED(){
    oled.init(0x3C);  //initialze  OLED display, default slave address is 0x3D

    oled.clearDisplay();

    oled.setTextSize(1);
    oled.setTextColor(WHITE);
    oled.setCursor(0,0);
}



void setup(){
    pinMode(LEDPIN, OUTPUT);
    digitalWrite(LEDPIN, HIGH);
    delay(200);
    digitalWrite(LEDPIN, LOW);


    pinMode(SHTPIN, OUTPUT);
    digitalWrite(SHTPIN, HIGH);

    delay(100); // this seems to be the minimum delay tolerable. Otherwise we cannot turn on the screen.
    // // Suspect it is because the humidity sensor drains some current (inrush?) when starting up.

    // delay(25);

    pinMode(OLEDPIN_0, OUTPUT);
    pinMode(OLEDPIN_1, OUTPUT);
    pinMode(OLEDPIN_2, OUTPUT);
    pinMode(OLEDPIN_3, OUTPUT);
    digitalWrite(OLEDPIN_0, HIGH);
    digitalWrite(OLEDPIN_1, HIGH);
    digitalWrite(OLEDPIN_2, HIGH);
    digitalWrite(OLEDPIN_3, HIGH);

    // delay(25);

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