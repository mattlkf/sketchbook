#include <Arduino.h>
#include <Wire.h>
#include <GOFi2cOLED.h>
#include <SPI.h>
#include <Mirf.h>
#include <nRF24L01.h>
#include <MirfHardwareSpiDriver.h>

// MIRF stuff
#define PAYLOADLEN 32 //max length
#define RXNAME "serv1" // 5 bytes
#define TXNAME "serv1" // 5 bytes

uint8_t data[PAYLOADLEN]; // a data buffer

void setupMirf(){
  Mirf.spi = &MirfHardwareSpi;
  Mirf.init();
    Mirf.setRADDR((byte *) RXNAME);
    Mirf.setTADDR((byte *) TXNAME);
    Mirf.payload = PAYLOADLEN;
    Mirf.config();
}

// OLED stuff

GOFi2cOLED oled;
 
void setupOLED()
{
  oled.init(0x3C);  //initialze  OLED display, default slave address is 0x3D

// disable splashscreen 
  // oled.display(); // show splashscreen
  // delay(2000);
 
  oled.clearDisplay();
 
  oled.setTextSize(1);
  oled.setTextColor(WHITE);
  oled.setCursor(0,0);
  oled.println("nRF24L01+ Testing");
  // oled.println("   Matthew Lee   ");
  oled.display();
 
  oled.setHorizontalScrollProperties(Scroll_Right,0, 1, Scroll_2Frames);
  oled.activateScroll();

  delay(3000);
}

void setup(){
    Serial.begin(9600);
    Serial.println("Hi!");

    setupMirf();
    setupOLED();
}

uint8_t received(){
  return (uint8_t)(!Mirf.isSending() && Mirf.dataReady());
}

void updateStats(){

  if(millis() - lastTime > SAMPLE_INT){
    lastTime = millis();
  }


}

uint8_t refreshtime(){
  static lastTime = 0;
  if(millis() - lastTime > )
}

int main(){ 
    init();
    setup();

    while(1){
      if(received()) updateStats();

      if(refreshtime())displayStats();
    }
}