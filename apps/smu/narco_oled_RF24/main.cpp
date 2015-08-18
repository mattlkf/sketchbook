#include <Arduino.h>
#include <Wire.h>
#include <GOFi2cOLED.h>
#include <SHT2x.h> 

#include <SPI.h>
#include "RF24.h"

#include <Narcoleptic.h>

#define DELAY_MILLISECONDS 10000

#define LEDPIN 13

#define OLEDPIN 6

#define NRF_PIN 9

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

/****************** User Config ***************************/
// Choose the names of the transmiting Pipe and the reading Pipe.
#define RXNAME "serv1" // 5 bytes
#define TXNAME "serv1" // 5 bytes

#define PAYLOADLEN 32

uint8_t data[PAYLOADLEN]; // a data buffer

/* Hardware configuration: Set up nRF24L01 radio on SPI bus plus pins 7 & 8 */
RF24 radio(8,7); //(ce,csn)
/**********************************************************/

void setupRF24(){
    radio.begin();

    // Set the PA Level low to prevent power supply related issues since this is a
    // getting_started sketch, and the likelihood of close proximity of the devices. RF24_PA_MAX is default.
    radio.setPALevel(RF24_PA_LOW);

    // Open a writing and reading pipe
    radio.openWritingPipe((byte *) TXNAME);
    radio.openReadingPipe(1, (byte *) RXNAME);

    // Start the radio listening for data
    radio.startListening();

    // Turn off th{e radio
    radio.powerDown();
}


void setup(){
    Wire.begin();

    pinMode(OLEDPIN, OUTPUT);
    digitalWrite(OLEDPIN, HIGH);

    pinMode(NRF_PIN, OUTPUT);
    digitalWrite(NRF_PIN, HIGH);


    delay(200);

    setupOLED();

    oled.sendCommand(GOFi2cOLED_Display_Off_Cmd);    /*display OFF*/

    setupRF24();

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