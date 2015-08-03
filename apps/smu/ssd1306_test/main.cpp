#include <Arduino.h>
#include <Wire.h>
#include <GOFi2cOLED.h>
 
GOFi2cOLED oled;
 
void setupOLED()
{
  oled.init(0x3C);  //initialze  OLED display, default slave address is 0x3D
 
  oled.display(); // show splashscreen
  delay(2000);
 
  oled.clearDisplay();
 
  oled.setTextSize(1);
  oled.setTextColor(WHITE);
  oled.setCursor(0,0);
  oled.println("Hello, world!");
  oled.println("Matthew Lee");
  oled.println("Kay Fei :)");
  oled.println("Betta Splendens");
  oled.display();
 
  oled.setHorizontalScrollProperties(Scroll_Right,1, 2, Scroll_2Frames);
  // oled.setHorizontalScrollProperties(Scroll_Left,2, 4, Scroll_2Frames);
  oled.activateScroll();

}

void setup(){
	Serial.begin(9600);
	Serial.println("Hi!");

	setupOLED();
}

int main(){	
	init();
	setup();


	while(1){
	}
}