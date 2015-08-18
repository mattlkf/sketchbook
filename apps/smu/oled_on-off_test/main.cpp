#include <Arduino.h>
#include <Wire.h>
#include <GOFi2cOLED.h>
#include <SHT2x.h> 

#include <Narcoleptic.h>

#define DELAY_MILLISECONDS 10000

#define LEDPIN 13

#define OLEDPIN 6

GOFi2cOLED oled;
 
void setupOLED(){
    oled.init(0x3C);  //initialze  OLED display, default slave address is 0x3D

    oled.display(); // splash screen

    delay(500);

    oled.clearDisplay();

    oled.setTextSize(1);
    oled.setTextColor(WHITE);
    oled.setCursor(0,0);

    oled.display();
}



void setup(){
    Wire.begin();

    pinMode(OLEDPIN, OUTPUT);
    digitalWrite(OLEDPIN, HIGH);

    delay(200);

    setupOLED();

    oled.sendCommand(GOFi2cOLED_Display_Off_Cmd);    /*display OFF*/

}

int main(){ 
    init();
    setup();

    uint8_t ctr = 0;

    while(1){
        oled.sendCommand(GOFi2cOLED_Display_On_Cmd);    /*display ON*/

        delay(100);
        // show something
        oled.clearDisplay();
        oled.setCursor(0,0);
        oled.print(ctr);
        oled.display();

        delay(500);

        oled.sendCommand(GOFi2cOLED_Display_Off_Cmd);    /*display ON*/

        // Sleep!!

        Narcoleptic.disableMillis();
        Narcoleptic.disableWire();

        // this is necessary to bring current consumption down from 0.35mA to 0.15mA in sleep
        digitalWrite(A4, LOW); // SDA low
        digitalWrite(A5, LOW); // SCL low

        // Sleep
        Narcoleptic.delay(DELAY_MILLISECONDS);
        // Re-enable stuff needed during awake-time
        Narcoleptic.enableWire(); // i2c
        Narcoleptic.enableMillis(); // without this, i2c doesn't work


        ctr++;
    }
}