#include <Arduino.h>
#include <Wire.h>
#include <OzOLED.h>

void setupOLED(){

  OzOled.init();  //initialze SEEED OLED display
  OzOled.printString("Hello World!"); //Print the String

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